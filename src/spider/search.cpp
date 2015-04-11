/*
Author:			support@shootsoft.net
Date:			2008-12-29
FileName:		search.cpp
Description:	本地和CGI程序入口的检索
Envirment:	Ubtuntu 8.04  G++
*/

#include <dlfcn.h>
#include <assert.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#define DEBUG 0
#define N 1000

using namespace std;
//链表节点
struct Node;
typedef struct Node* PNode;

struct Node
{
	int docNum;
	PNode next;
};
typedef struct Node* DocNode;


//记录每个文档的pageRank值
map<int,double> pageRanks;
//声明文件索引信息
map<string,DocNode>  fileIndex;
//文档列表
map<int,string> fileList;

//创建节点
DocNode createDocNode(int value)
{
	DocNode node = (DocNode)malloc(sizeof(struct Node));
	//DocNode* node = new DocNode();
	if(node!=NULL)
	{
		node->next = NULL;
		node->docNum = value;
	}
	else cout<<"Out of space!\n"<<endl;
	return node;
}

//查找节点是否存在 -1 不存在 1 存在
int valueExist(DocNode node,int docNum)
{
	DocNode x = node;
	while(x!=NULL && x->docNum != docNum)
	{
		x = x->next;
	}
	if(x==NULL) return -1;
	else return 1;
}

//显示一个链表
void displayDocList(DocNode list)
{
	//如果链表为空
	if(list==NULL)cout<<"NULL"<<endl;
	DocNode node = list;
	while(node->next!=NULL)
	{
		cout<<node->docNum<<" ";
		node=node->next;
	}
	cout<<node->docNum<<endl;
}


//根据一个值获取一个节点,同时这个节点会被原节点列表中移除
int removeNode(DocNode  node, int docNum)
{
	//node有可能为空!
	if(node == NULL) return 0;
	DocNode pre = node;
	DocNode x = node;
	while(x!=NULL && x->docNum != docNum)
	{
		x = x->next;
		//第一次pre不移动
		//从第二次开始,pre移动
		if(pre->next!=x)
		{
			pre= pre->next;
		}
	}
	//没有找到结果
	if(x==NULL) return 0;
	else
	{	
		//找到结果有三种情况:
		//1.如果是头结点并且没有下一个节点了(此链表只有一个节点)
		if(x==node && x->next == NULL)
		{
			delete(node);
		}
		//2.这种情况是x是头结点,并且包含下一个
		else if(x==node && x->next!=NULL)
		{
			node = node->next;
			x->next = NULL;
#if DEBUG
			cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxx"<<endl;
			cout<<"Temp x="<<x->docNum << " Node=";
			displayDocList(node);
			cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxx"<<endl;
#endif
			delete(x);
		}
		else
		{			
			//3.找到结果,将节点断开出来
			pre->next = x->next;
			x->next = NULL;
			delete(x);
		}
		return 1;
	}
}

//因为所有操作均是建立在链表的操作基础之上,所以必须把链表结果拷贝一份,以防止破坏原有的结构
vector<DocNode> copy(vector<DocNode> nodes)
{
	vector<DocNode> result;
	//cout<<"nodes.size()="<<nodes.size()<<"Ok"<<endl;
	for(int i =0; i <nodes.size(); i++ )
	{
		//拷贝头节点
		DocNode head = nodes[i];		
		//cout<<"nodes["<<i<<"]->docNum = "<<head->docNum<<" OK!"<<endl;
		DocNode node = createDocNode(head->docNum);
		//cout<<"Copy head "<<i<<" OK!"<<endl;
		//准备好要移动的指针
		DocNode temp = node;
		DocNode probe = nodes[i]->next;
		//开始遍历
		while(probe!=NULL)
		{
			temp->next =  createDocNode(probe->docNum);
			temp = temp->next;
			probe = probe->next;
		}
		//推入结果列表
		result.push_back(node);
	}
	//cout<<"result.size()="<<result.size()<<"Ok"<<endl;
	return result;
}

//显示一个索引
void displayMap(map<string,DocNode> nodes)
{
	map<string,DocNode>::const_iterator map_it = nodes.begin();
	while(map_it!=nodes.end())
	{
		cout<<map_it->first<<":";
		displayDocList(map_it->second);
		cout<<endl;
		++map_it;
	}
}

//分词,支持中文(二元切词)和英文(按词切分),数字混合
//返回的结果字符串中每个单词都是是唯一的
set<string> segment(string str)
{
	//结果列表
	set<string> list;

	for(int i = 0; i < str.size();i++)
	{
		string word = "";
		int pos = i;
		//判断是否是可以打印的符号(Ascii)
		if(isprint(str[i]))
		{
			//如果可以打印,判断是否是字母
			while( isalpha(str[i]) || isdigit(str[i]) )i++;				
			//截取一个单词或数字的组合
			//比如 key1 key2 
			if(pos!=i)
			{
				word = str.substr(pos,i- pos);
#if DEBUG_SEGMENT
				cout<<word<<"\t start:"<<pos<<"\t end:"<<i<<endl;
#endif
				//i--;				
			}			
			//既不是字母也不是数字,那么可能是要被忽略的字符了:-)
			if(pos==i)
			{
#if DEBUG_SEGMENT
				cout<<str[i]<<"\tNot"<<endl;
#endif
				continue;			
			}
		}		
		else
		{
			if(str[i]<0)
			{
				//将中文按字切分
				word = str.substr(i,3);
				i+=2;
				//二元切词
				if(str[i+1]<0 && !isprint(str[i+1]))
				{
					word+=str.substr(i+1,3);
				}

#if DEBUG_SEGMENT
				cout<<word<<endl;
#endif
			}
		}
		//cout<<word<<"\t";
		if(list.count(word)==0)
			//加入列表
			list.insert(word);
	}
	cout<<endl;

	return list;
}




//搜索单词,返回文档列表
//单字/词的检索
DocNode searchSingle(string word)
{
	//判断索引中是否包含这个单词
	map<string,DocNode>::iterator it = fileIndex.find(word);
	//如果索引中已经存在这个单词,则返回文档列表
	if(it!=fileIndex.end())
	{
		//取出链表
		DocNode node = fileIndex[word];
		return node;
	}
	//如果索引中不包含这个单词
	else
	{
		return NULL;
	}
}

//多字检索,自动分词
DocNode search(string word)
{
	//将搜索的关键字分词
	set<string> keys = segment(word);
	//cout<<"segment() OK"<<endl;
	//最终结果列表
	vector<DocNode> results;
	//准备临时列表
	vector<DocNode> temp;
	//结果有链表有两个指针,一个只头,一个指尾
	DocNode finalHead = NULL;
	DocNode finalTail = NULL;
	//cout<<"Out put key: ";
	//取出包含所有关键字的集合
	for(set<string>::iterator it =keys.begin(); it != keys.end(); it++)
	{
		string single= *it;
		//cout<<*it<<"|";

		//判断索引中是否包含这个单词
		map<string,DocNode>::iterator it = fileIndex.find(single);
		//如果索引中已经存在这个单词,则返回文档列表
		if(it!=fileIndex.end())
		{
			//取出链表
			DocNode node = fileIndex[single];
			temp.push_back(node);
		}
	}
	//cout<<" "<<endl;
	if(temp.size()==0)
	{
		return NULL;
	}
	//cout<<"get all docs OK\t"<<temp.size()<<endl;
	results = copy(temp);
	//cout<<"copy() OK\t"<<results.size()<<endl;


	//如果只有一个结果,那么直接返回
	if( results.size() == 1)
	{
		finalHead = results[0];
		//cout<<"Only 1 result list"<<endl;
		return finalHead;
	}

#if DEBUG
	//cout<<endl;
	//cout<<endl;
	//cout<<endl;
	for(int i=0; i < results.size() ; i++)
	{
		//只要有一个文档不包含,就设置为0
		displayDocList(results[i]);
	}
	//cout<<endl;
	//cout<<endl;
	//cout<<endl;
#endif

	//第一遍过滤, 将包含所有关键字的结果取出
	DocNode current = results[0];
	//将结果先都指向同一个节点;
	//这是一个特殊的节点,文档编号为-1
	//此结果之前的文档为精确结果,之后的为模糊搜索的结果
	finalHead = createDocNode(-1);
	finalTail = finalHead;
	//cout<<"Init OK"<<endl;
	while(current!=NULL)
	{		
		//提前准备好指针
		DocNode next = current->next;
		//int docNum = current->docNum;
		//flag = 1 表示所有结果都包含当前的docNum
		//flag = 0 表示部分结果都包含当前的docNum
		int flag = 1;
		for(int i=1; i < results.size() ; i++)
		{
			//只要有一个文档不包含,就设置为0
			if(results[i]!=NULL && results[i]->docNum== current->docNum)
			{
				//cout<<"OK?"<<endl;
				results[i]= results[i]->next;
			}
			else if(removeNode(results[i],current->docNum)==0) 
			{	
				flag = 0;				
			}
			/*			cout<<"bbbbbbbbbbbbbbbb"<<endl;	
			displayDocList(results[i]);
			cout<<"bbbbbbbbbbbbbbbb"<<endl;*/	
		}
		//所有结果都包含这个docNum
		if(flag==1)
		{
			//finalHead始终指向头
			current->next = finalHead;
			finalHead = current;				
		}
		else if(flag==0)
		{
			//finaTail始终指向尾
			finalTail->next = current;
			finalTail = current;
			current->next = NULL;
		}
		//指针向后移动
		current = next;
#if DEBUG
		//cout<<"move to next docNum OK"<<endl;
#endif
	}

#if DEBUG
	cout<<"first filter OK"<<endl;
	displayDocList(finalHead);
#endif



	//第二遍,把包含单个关键字的结果接到尾部
	for(int i=1; i < results.size() ; i++)
	{
#if DEBUG
		cout<<"===================="<<endl;
		displayDocList(results[i]);
		cout<<"===================="<<endl;
#endif
		//指向results中的某一个结果
		DocNode docNow = results[i];
		while(docNow!=NULL)
		{
			//首先准备好临时节点
			DocNode docNext = docNow->next;
			//将docNow接在finalTail的后面,finalTail始终指向最后
			finalTail->next = docNow;
			docNow->next = NULL;
			finalTail = docNow;
			//移动指针
			docNow = docNext;
		}
	}
	//cout<<"second filter OK"<<endl;
	//finalHead就是最终结果链表的头节点
	return finalHead;
}


void debugDisplayDocs(DocNode node)
{
	DocNode x = node;
	//查询结果记录器
	int i=1;
	while(x!=NULL)
	{
		cout<< x->docNum<<"\t";
		x = x->next;
		i++;
	}
	cout<<endl;
}

//将文档列表转换为文件名格式打印到屏幕上
void displayDocs(DocNode node)
{
	DocNode x = node;
	//查询结果记录器
	int i=1;
	while(x!=NULL)
	{
		if(x->docNum!=-1)
			cout<<i<<".\tPageRank:"<<pageRanks[x->docNum]<<"\tDocNum:"<< x->docNum<<"\t Name:"<<fileList[x->docNum]<<endl;
		else
			cout<<"========================================================"<<endl;
		x = x->next;
		i++;
	}
	cout<<"Finish"<<endl;
}


//将文档列表转换为文件名格式打印到屏幕上
void displayHtmlResult(DocNode node)
{
	DocNode x = node;
	//查询结果记录器
	int i=1;
	while(x!=NULL)
	{
		if(x->docNum!=-1)
			cout<<i<<".\tPageRank:"<<pageRanks[x->docNum]<<"\tDocNum:"<< x->docNum<<"\t Name:<a href=\""<<fileList[x->docNum]<<"\" target=\"_blank\">"<<fileList[x->docNum]<<"</a><br />"<<endl;
		else
		cout<<"<hr /><br />"<<endl;

		x = x->next;
		i++;
	}
	cout<<"Finish"<<endl;
}

//十进制数字转换16进制字母
char dec2hexChar(short int n) {
	if ( 0 <= n && n <= 9 ) {
		return char( short('0') + n );
	} else if ( 10 <= n && n <= 15 ) {
		return char( short('A') + n - 10 );
	} else {
		return char(0);
	}
} 

//十六进制字母转换十进制数字
short int hexChar2dec(char c) {
	if ( '0'<=c && c<='9' ) {
		return short(c-'0');
	} else if ( 'a'<=c && c<='f' ) {
		return ( short(c-'a') + 10 );
	} else if ( 'A'<=c && c<='F' ) {
		return ( short(c-'A') + 10 );
	} else {
		return -1;
	}
} 

//URL编码编码函数
string escapeURL(const string &URL)
{
	string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if ( 
			( '0'<=c && c<='9' ) ||
			( 'a'<=c && c<='z' ) ||
			( 'A'<=c && c<='Z' ) ||
			c=='/' || c=='.' 
			) {
				result += c;
		} else {
			int j = (short int)c;
			if ( j < 0 ) {
				j += 256;
			}
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1*16;
			result += '%';
			result += dec2hexChar(i1);
			result += dec2hexChar(i0); 
		}
	}
	return result;
} 

//Url编码解码函数
std::string deescapeURL(const std::string &URL) {
	string result = "";
	for ( unsigned int i=0; i<URL.size(); i++ ) {
		char c = URL[i];
		if ( c != '%' ) {
			result += c;
		} else {
			char c1 = URL[++i];
			char c0 = URL[++i];
			int num = 0;
			num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
			result += char(num);
		}
	}
	return result;
}

//加载索引
void loadIndex(string path)
{
	string index = path + "index.data";
	//打开文件
	ifstream fin(index.c_str());	
	if(!fin)
	{
		//如果打开失败则报错
		cerr<<index<< "\t\t\t" << "Error!"<< endl;
	}
	else
	{
		//读取文档内容到content
		string line;
		string content;
		string word;
		int intstr;
		stringstream stream;     //声明一个stringstream变量 
		while(!fin.eof())
		{
			getline(fin,line);
			if(line.size()==0)break;

			DocNode head =  createDocNode(-1);
			DocNode pnext = head;
			// '\t' ==9
			//cout<<line<<endl;			
			//cout<<line.size()<<endl
			int p = 0;
			for(int i =0 ; i < line.size(); i ++)
			{
				if(line[i]==9)
				{
					if(p==0)
					{
						//cout<<i<<endl;
						word=line.substr(0,i-p);
						//cout<<"|"<<word<<"|"<<endl;
					}
					else
					{
						//将string转换为int
						stream<<line.substr(p,i-p);
						stream>>intstr;
						//多次使用
						stream.clear(); 
						//构造一个链表
						if(pnext->docNum ==-1)
						{
							pnext->docNum = intstr;
						}
						else
						{

							DocNode node =  createDocNode(intstr);
							pnext->next = node;
							pnext=node;
						}//判断链表
						//cout<<"|"<<intstr<<"|"<<endl;
					}//判断是否第一次处理p
					p=i+1;
				}//判断是否是分割字符
			}//一行处理完毕
			fileIndex[word] = head;
		}//end while 循环读取
	}//end fin是否可读
	fin.close();	
}


//加载Url列表
void loadUrls(string path)
{
	string fileName = path + "record.txt";
	//cout<<"<br />Debug:"<<fileName<<endl;
	//Url编号，从1开始
	int docNum = 1;
	//打开文件
	ifstream fin(fileName.c_str());	
	if(!fin)
	{
		//如果打开失败则报错
		cerr<<fileName<< "\t" << "Error!"<< endl;
		//cout<<"<br />Error<br />"<<endl;
	}
	else
	{
		//cout<<"<br />Readed"<<endl;

		//读取文档内容到content
		string line;
		while(!fin.eof())
		{
			getline(fin,line);
			//cout<<"<br />Line:"<<line<<endl;
			//printf("%d",line[0]);
			//'1' = 49
			if(docNum>N)break;
			if(line[0]==49 )
			{
				fileList[docNum] = line.substr(2);
				//cout<<line.substr(0,1)<<"="<<line.substr(2)<<endl;
				docNum++;
			}
		}
		//cout<<"共计Url"<<docNum-1<<"个"<<endl;
		//添加文档内容到索引
		//addDoc(docNum,content);
	}
	fin.close();	
}


//加载PageRank信息
void loadPageRank(string path)
{
	string fileName = path + "pageranks.txt";

	//Url编号，从1开始
	int docNum = 1;
	//打开文件
	ifstream fin(fileName.c_str());	
	if(!fin)
	{
		//如果打开失败则报错
		cerr<<fileName<< "\t" << "Error!"<< endl;
	}
	else
	{
		int docNum=1;
		while(!fin.eof())
		{
			if(docNum>N)break;
			double value;
			fin>>value;
			if(value!=0)
			{
				//cout<<"Rank:"<<value<<endl;
				pageRanks[docNum] = value;
				docNum++;

			}
			else
			{
				cout<<"docNum="<<docNum<<endl; 
			}
		}
	}
	fin.close();	
}

//将a 和b 的DocNum进行交换
void swap(DocNode a, DocNode b)
{
	int docNum = a->docNum;
	a->docNum = b ->docNum;
	b ->docNum = docNum;
}

//按照PageRank的高低排序
void SortByRank(DocNode head)
{
	if(head==NULL || head->next==NULL) return;
	////增加一个头结点，否则没法排序
	//DocNode newHead = createDocNode(-1);
	//newHead->next = head;
	DocNode start = head;
	DocNode split = NULL;
	DocNode pnext = NULL;
	DocNode pm = NULL;
	//debugDisplayDocs(head);
	//displayDocs(head);
	//int i =0;
	//首先对精确搜索结果排序
	while(start!=NULL && start->docNum!=-1)
	{
		pnext = start->next;
		pm = start;
		while( pnext!=NULL && pnext->docNum!=-1)
		{
			if( pageRanks[pnext->docNum] > pageRanks[pm->docNum] )
			{
				pm = pnext;
			}
			pnext = pnext->next;
			//cout<<i++<<"\t";
			//if(i>100)
			//{				
			//	return newHead->next;
			//}
		}
		if(pm!=start)
		{
			//cout<<"swap()"<<endl;
			swap(pm,start);
		}

		//记录分割节点的位置
		if(split == NULL && pnext!=NULL)
		{
			split=pnext;
		}
		start = start->next;
		//cout<<"2"<<endl;
	}
	if(split==NULL)	return;
	//其次对非精确搜索结果排序
	start = split->next;
	while(start!=NULL)
	{
		pnext = start->next;
		pm = start;
		while( pnext!=NULL && pnext->docNum!=-1)
		{
			if( pageRanks[pnext->docNum] > pageRanks[pm->docNum] )
			{
				pm = pnext;
			}
			pnext = pnext->next;
		}
		if(pm!=start)
		{
			swap(pm,start);
		}
		start = start->next;
		//cout<<"4"<<endl;
	}

	//return;

}


//本地程序入口
int localMain(int argc , char** argv)
{
	//参数检查,第二个路径参数必须存在
	assert(argc==2);
	//加载文档编号
	loadUrls(argv[1]);
	//加载索引信息
	loadIndex(argv[1]);
	//加载文档PageRank
	loadPageRank(argv[1]);

	//显示索引
	//displayMap(fileIndex);
	//测试
	string keys;
	//用 空格+f 结束 输入
	cout<<"请输入搜索关键字( 输入 \"\\exit\" 退出 ):"<<endl;
	getline(cin,keys); 
	while(keys!="\\exit")
	{
		cout<<"您输入的关键字为: "<<keys<<endl;
		DocNode nodes =search(keys);
		if(nodes==NULL)
			cout<<"抱歉,没有搜索到结果!"<<endl;
		else
		{
			cout<<"搜索结果:"<<endl;
			//debugDisplayDocs(nodes);
			//displayDocs(nodes);
			//debugDisplayDocs(nodes);
			//cout<<"============================================";
			SortByRank(nodes);
			//debugDisplayDocs(nodes);
			displayDocs(nodes);
		}
		cout<<"请输入搜索关键字( 输入 \"\\exit\" 退出 ):"<<endl;
		getline(cin,keys); 
	}	
	return 0;
}

//CGI程序入口
int cgiMain()
{
	int i, n;
	cout<< "Content-type: text/html\n\n";
	cout<< "<html><head>\n";
	cout<< "<title>Search Demo</title>\n";
	cout<< "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
	cout<< "</head><body>\n";
	cout<< "<form method=\"post\" action=\"/cgi-bin/search.cgi\">\n";
	cout<< "<input name=\"path\" size=\"42\"  type=\"hidden\" value=\"\"/><br />\n";
	cout<< "Input Keys:<input id=\"key\" name=\"key\" type=\"text\" />\n";
	cout<< "<input type=\"submit\" value=\"Search\" />\n";
	cout<< "</form>\n";
	//cout<< "<p>Your input is</p>";

    if( getenv("CONTENT_LENGTH") )
	{
		n = atoi( getenv("CONTENT_LENGTH") );
	}
	else
	{
		n = 0;
		fprintf( stdout, "(NULL)\n" );
	}
	string request;
	for( i=0; i<n; i++ )
	{
		request +=getc(stdin);
	}
	//request = deescapeURL(request);
	string path = request.substr(5,request.find_last_of("&") -5);
	string key =  request.substr(request.find_last_of("&") + 5);
	path=deescapeURL(path);
	key=deescapeURL(key); 
	//cout<<request<<"[]<br />"<<endl;
	//cout<<path<<"<br />"<<endl;
	//cout<<key<<"<br />"<<endl;
	//cout<< "</body>\n</html>\n";
	//获取路径文本文件
	//cout<<"Debug:"<<path<<endl;
	//加载文档编号
	loadUrls(path);
	//加载索引信息
	loadIndex(path);
	//加载文档PageRank
	loadPageRank(path);

	//vector<string> files=GetDirFiles((char*)path.c_str() );
	//for(int i =0; i < files.size(); i++)
	//{
	//	files[i] = path + files[i];
	//	//打印文件
	//	//cout << files[i]<< endl;
	//	//添加文件到索引
	//	addfile(files[i],i);
	//}
	//根据关键字检索
	DocNode nodes = search(key);
	cout<<"<p>您输入的关键字：" << key<<"</p>"<<endl;
	if(nodes==NULL)
		cout<<"抱歉,没有搜索到结果!"<<endl;
	else
	{
		SortByRank(nodes);
		cout<<"搜索结果:<br />"<<endl;
		displayHtmlResult(nodes);
	}
	return 0;
}



//默认程序入口
int main(int argc , char** argv)
{
	string cmd = argv[0];
	//根据参数判断是CGI程序还是本地程序
	if(cmd.substr(0,2)=="./")
		localMain(argc,argv);
	else
		cgiMain();
	return 0;
}
