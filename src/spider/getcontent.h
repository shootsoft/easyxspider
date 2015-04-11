/*
Author:			support@shootsoft.net
Date:			2008-12-29
FileName:		getcontent.h
Description:	下载页面，该源码来自网络，感谢原作者的辛勤劳动
Envirment:	Ubtuntu 8.04  G++
*/

#ifndef GETCONTENT_H_
#define GETCONTENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include  <iconv.h>  

using namespace std;

class Getcontent
{
private:

	int csize;
	int p;
	char urla[256];
public:
	char content[1024*1024+8];
	int extractUrl(char *someurl) {

	};
	Getcontent() {
	};
	void showcontent()
	{
		printf("%s\n",content);
	};
	/********************************************
	 功能：搜索字符串右边起的第一个匹配字符
	 ********************************************/
	char * Rstrchr(char * s, char x) {
		int i = strlen(s);
		if(!(*s)) return 0;
		while(s[i-1]) if(strchr(s + (i - 1), x)) return (s + (i - 1)); else i--;
		return 0;
	};

	/**************************************************************
	 功能：从字符串src中分析出网站地址和端口，并得到用户要下载的文件
	 ***************************************************************/
	void GetHost(char * src, char * web, char * file, int * port) {
		char * pA;
		char * pB;
		memset(web, 0, sizeof(web));
		memset(file, 0, sizeof(file));
		*port = 0;
		if(!(*src)) return;
		pA = src;
		if(!strncmp(pA, "http://", strlen("http://"))) pA = src+strlen("http://");
		else if(!strncmp(pA, "https://", strlen("https://"))) pA = src+strlen("https://");
		pB = strchr(pA, '/');
		if(pB) {
			memcpy(web, pA, strlen(pA) - strlen(pB));
			if(pB+1) {
				memcpy(file, pB + 1, strlen(pB) - 1);
				file[strlen(pB) - 1] = 0;
			}
		}
		else memcpy(web, pA, strlen(pA));
		if(pB) web[strlen(pA) - strlen(pB)] = 0;
		else web[strlen(pA)] = 0;
		pA = strchr(web, ':');
		if(pA) *port = atoi(pA + 1);
		else *port = 80;
	};

	int run(char* url)
	{//printf("connet to %s\n",url);


		memset(content,0,sizeof(content));
		csize=0;
		p=0;
		memset(urla,0,sizeof(urla));
		strcpy(urla,url);
		int sockfd;
		char buffer[1];
		struct sockaddr_in server_addr;
		struct hostent *host;
		int portnumber,nbytes;
		char host_addr[256];
		char host_file[1024];
		char request[1024];
		int send, totalsend;
		int i;

		GetHost(url, host_addr, host_file, &portnumber);/*分析网址、端口、文件名等*/
		//  printf("webhost:%s\n", host_addr);
		//  printf("hostfile:%s\n", host_file);
		// printf("portnumber:%d\n\n", portnumber);

		if((host=gethostbyname(host_addr))==NULL)/*取得主机IP地址*/
		{
			//fprintf(stderr,"Gethostname error, %s\n", strerror(errno));
			return(0);
		}

		/* 客户程序开始建立 sockfd描述符 */
		if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)/*建立SOCKET连接*/
		{
			//  fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
			return(0);
		}

		/* 客户程序填充服务端的资料 */
		bzero(&server_addr,sizeof(server_addr));
		server_addr.sin_family=AF_INET;
		server_addr.sin_port=htons(portnumber);
		server_addr.sin_addr=*((struct in_addr *)host->h_addr);

		/* 客户程序发起连接请求 */
		if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)/*连接网站*/
		{
			//fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
			return(0);
		}

		sprintf(request, "GET /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\nHost: %s:%d\r\nConnection: Close\r\n\r\n", host_file, host_addr, portnumber);
		//  printf("%s", request);/*准备request，将要发送给主机*/


		/*发送http请求request*/
		send = 0;totalsend = 0;
		nbytes=strlen(request);
		while(totalsend < nbytes) {
			send = write(sockfd, request + totalsend, nbytes - totalsend);
			if(send==-1) {
				//printf("send error!%s\n", strerror(errno));
				return (0);}
			totalsend+=send;
			//printf("%d bytes send OK!\n", totalsend);
		}

		// printf("\nThe following is the response header:\n");
		i=0;
		int count;
		count=0;
		/* 连接成功了，接收http响应，response */
		char header[1024];
		int acc=0;
		int checked=0;
		while((nbytes=read(sockfd,buffer,1))==1)
		{
			if(i < 4) {
				header[acc]=buffer[0];
				acc++;
				if(buffer[0] == '\r' || buffer[0] == '\n') i++;
				else i = 0;

			}
			else {
				if(checked==0)
				{	checked=1;
					header[acc]='\0';
					string ctmpstring=string(header);
					if(ctmpstring.find("Content-Type: text/html")<0)
//如果Content-Type: text/html没有的话 就返回 只处理text/html的页面
					return 0;
				}
				content[count]=buffer[0];
				i++;
				count++;
				if (count>=1024*1024)
				break;
			}
		}
		/* 结束通讯 */
		content[count]='\0';
		csize=count+1;
		close(sockfd);
		string atempstring=string(content);
		int tmpa=atempstring.find("charset=");
		if(tmpa<0)
		return 0;
		string btmpstr=atempstring.substr(tmpa+8,4);
//如果编码不对  转换编码
		if(( btmpstr.find("gb")>=0)||(btmpstr.find("GB")>=0))
		{
			char changecontent[csize];
			g2u(content,csize,changecontent,csize);
			memset(content,0,csize);
			strcpy(content,changecontent);
		}
		//printf("content is%s",content);
		return 1;
	}

	//代码转换:从一种编码转为另一种编码  
	int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
	{
		iconv_t cd;
		int rc;
		char **pin = &inbuf;
		char **pout = &outbuf;

		cd = iconv_open(to_charset,from_charset);
		if (cd==0) return -1;
		memset(outbuf,0,outlen);
		//printf("inlen=%d\n",inlen);  
		if (iconv(cd,pin,(size_t *)&inlen,pout,(size_t *)&outlen)==-1) return -1;
		iconv_close(cd);
		return 0;
	}
	//UNICODE码转为GB2312码  
	int u2g(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
	{
		return code_convert("UNICODE","gb2312",inbuf,inlen,outbuf,outlen);
		//return   code_convert("utf-8","GB2312",inbuf,inlen,outbuf,outlen);  
	}
	//GB2312码转为  UNICODE码
	int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
	{
		return code_convert("GB2312","utf-8",inbuf,inlen,outbuf,outlen);
		//return   code_convert("GB2312","UNICODE",inbuf,inlen,outbuf,outlen);  
	}
};
#endif /*GETCONTENT_H_*/
