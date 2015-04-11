/*
Author:			support@shootsoft.net
Date:			2008-12-29
FileName:		spider.cpp
Description:	多线程爬虫，使用hypar2分析Url，本地文件系统存储文件
Envirment:	Ubtuntu 8.04  G++

*/

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <queue>
#include <dlfcn.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
//自定义头文件
#include "getcontent.h"
//URL分析
#include <hypar/URL.hpp>

//定义总共下载的URL数量
#define TOTAL_URLS 1500
//定义线程数
#define THREAD_COUNT 10

//使用标准库的命名空间
using namespace std;

//未处理队列
queue<string> unporcessed;
//已处理队列,int表示是否处理成功
map<string,int> processed;
//未处理队列线程锁
pthread_mutex_t unporcessed_lock;
//已处理队列线程锁
pthread_mutex_t processed_lock;
//URL分析线程锁
pthread_mutex_t url_lock;

//本地路径
string local;
//已经处理的Url个数,处理不成功的不计数
int totalUrl;

//初始化未处理队列，加载Url地址列表
void initQueue(string starturl)
{
	//打开urls.txt
	ifstream fin(starturl.c_str());	
	if(!fin)
	{
		cerr<<starturl << "\t" << "Error!"<< endl;
	}
	else
	{
		string line;
		//按行读取内容
		while(!fin.eof())
		{
			getline(fin,line);
			//url长度
			if(line.size()<10)
				break;
			//加入未处理队列
			unporcessed.push(line);
			cout<<"Read:\t"<<line<<endl;
		}
	}
	fin.close();	
}

//按照url和本地路径转换文件，同时创建本地路径
string convert(string url, string local, int createDir)
{
	//首先拼接合法的本地路径
	string urlpath = url.substr(url.find("//") + 2);
	if(local[local.size()-1]!='/')
		local+="/";
	urlpath = local + urlpath;
	//加上默认的文件名
	if(urlpath[urlpath.size()-1]=='/')
		urlpath+="default.html";
	if(createDir!=0)
	{
		//准备递归创建文件夹
		string left = "/";
		string right = urlpath.substr(1,urlpath.find_last_of("/"));
		//cout<<right<<endl;
		while(right.find("/")!=-1)
		{
			//每次取出一级目录，直接尝试创建目录
			chdir(left.c_str());
			string name = right.substr(0,right.find("/"));			
			mkdir(name.c_str(),0700);
			//为下一次递归作准备			
			left+=name + "/";
			right.erase(0,name.size() + 1);
		}
	}
	return urlpath;
}

//保存文件到本地
void saveFile(string url, string content)
{
	string file = convert(url, local, 1);
	ofstream ofile(file.c_str());
	if(!ofile)
	{
		cerr<<file << "\t" << "Error!"<< endl;
	}
	else
	{
		ofile<<content<<endl;
	}
	ofile.close();
}


//多线程互斥等待
static void Thead_sleep(int t){
    timespec ts;
    ts.tv_sec =  t;
    ts.tv_nsec = 0; // 
    nanosleep(&ts,NULL);
}

//检查Url的合法性
int isUrlOK(string url)
{
	//cout <<url<<" "<<url.find("?")<<url.find("#")<<url.find("javascript")<<endl;
	//不能包含? # javascript
	if(url.find("?")==-1 && 
		url.find("#")==-1 && 
		url.find("javascript") ==-1 && url.find(":80") <url.size() ) return 1;

	return 0;
}

//处理链接
//链接处理规则：只处理静态链接，动态链接一律不管
static void parseLink(string baseUrl, string content)
{
	//cout<<"Parse Link: "<<baseUrl<<endl;
	//锁定URL处理器
	pthread_mutex_lock (&url_lock);
	//设置根URL
	URLCollector::m_baseHref = baseUrl;
	//分析内容
	URLCollector url_collector((char *)content.c_str());
	//遍历取出
	list<string>::iterator urlx = url_collector.m_URLList.begin();
	while (urlx != url_collector.m_URLList.end())
	{
		//只处理合法的URL
		if(isUrlOK((*urlx))==1)
		{
			string url= (*urlx);
			url = url.erase(url.find(":80"),3); 
			unporcessed.push(url);
			//cout <<"OK:"<< url<< endl;
		}
		else
		{
			//cout <<"Not:"<< (*urlx)<< endl;
		}
		urlx++;
	}


	//解锁URL处理器
	pthread_mutex_unlock(&url_lock);
	//线程等待              
	Thead_sleep(2);

}


//多线程下载
static void *download_one_url(void *threadID)
{
	//下载TOTAL_URLS就停止
	while(1)
	{
		//锁定未处理队列
		pthread_mutex_lock (&unporcessed_lock);
		
		//检查边界
		if(unporcessed.empty() || totalUrl>TOTAL_URLS)
		{
			//解锁未处理队列
			pthread_mutex_unlock(&unporcessed_lock);
			break;
		}
		//从队列中取出
		string url = unporcessed.front();
		unporcessed.pop();
		//解锁未处理队列
		pthread_mutex_unlock(&unporcessed_lock);
		

		//锁定处理队列
		pthread_mutex_lock (&processed_lock);

		int pr = processed.count(url);
		//检查是否处理过
		if(pr==0)
		{
			processed[url] = 0;
		}
		//解锁处理队列
		pthread_mutex_unlock(&processed_lock);
		
		if(pr==0)
		{
			//printf("%d",(int)threadID);
			cout<<"start download :"<<url<<endl;
			Getcontent htmlget;
			int downresult = htmlget.run((char*)url.c_str());
			//下载成功
			if(downresult==1)
			{
				//锁定未处理队列
				pthread_mutex_lock (&processed_lock);
				processed[url] = 1;
				//解锁未处理队列
				cout<<"successful download("<<totalUrl<<") :"<<url<<endl; 
				pthread_mutex_unlock(&processed_lock);
				//保存内容
				saveFile(url, string(htmlget.content));
				//分析链接
				parseLink(url,string(htmlget.content));
				totalUrl++;
			}
			else
			{
				cout<<"failed download :"<<url<<endl; 
			}
		}
		//线程等待，防止独占资源         
		Thead_sleep(1);

	}
	cout<<(int*)threadID<<endl;
	return NULL;
}

//保存Url处理结果
void saveURLs()
{
	string record= local + "record.txt";
	ofstream ofile(record.c_str());
	if(!ofile)
	{
		cerr<<record << "\t" << "Error!"<< endl;
	}
	else
	{
		map<string,int>::const_iterator it = processed.begin();
		while(it!=processed.end())
		{
			ofile<<it->second<<" "<<it->first<<endl;
			cout<<it->second<<" "<<it->first<<endl;
			it++;
		}		
	}
	ofile.close();
}


int main(int argc, char** argv)
{
	pthread_t tid[THREAD_COUNT];
	int i;
	int error;
	//至少3个参数：第一个为默认，第二个为开始URL的列表文件，第三个为下载文件保存
	assert(argc==3);

	//初始化队列
	initQueue(argv[1]);
	local = argv[2];
	totalUrl =0;

	//启动多线程下载
	for(i=0; i< THREAD_COUNT; i++) {
		int* threadID = new int();
		*threadID = i;
		error = pthread_create(&tid[i],
			NULL, //默认属性
			download_one_url,
			&threadID);
		if(0 != error)
			fprintf(stderr, "Couldn't run thread number %d, errno %d\n", i, error);
		else
			fprintf(stderr, "Thread %d, OK\n", i);
	}

	//等待线程结束
	for(i=0; i< THREAD_COUNT; i++) {
		error = pthread_join(tid[i], NULL);
		fprintf(stderr, "Thread %d terminated\n", i);
	}
	saveURLs();

}

