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
#include <sys/types.h>
#include <sys/stat.h>
#include <hypar/localdefs.h>
#include <hypar/MmapWrapper.hpp>
#include <hypar/ToWideCharConverter.hpp>
#include <hypar/WideCharEntityStream.hpp>
#include <hypar/HyXML.hpp>
#include <hypar/HashUtils.hpp>
#include <hypar/Debug.hpp>
#include <hypar/URL.hpp>
#include <locale.h>


#define DEBUG 0
#define DEBUG_SEGMENT 0

using namespace std;
#if __GNUC__ > 2
using namespace __gnu_cxx;
#endif


//**************************************************************************************************************
//本地文件的文本内容
string localTextcontent;
string baseUrl;
vector<string> urls;

HyXML::EntityCbRetval processTag(HyXML::Tag *pTag)
{
	if(strcmp(pTag->m_pName,"script")==0 || strcmp(pTag->m_pName,"style") == 0)
		return HyXML::IGNORE_CHILDREN;
	else
	{
		string pn = pTag->m_pName;
		if(pn =="a")
		{
			list<pair<_char *, _char *> >::iterator it = pTag->m_attrList.begin(); 
			while( it != pTag->m_attrList.end() ) 
			{ 
				if(strcmp(it->first,"href")==0)
				{
					string href = it->second;
					if(href.size()<11 || href.substr(0,11)!="javascript:")
					{
						URL u(href, baseUrl);
						if (u.getParseStatus() == URL::URL_OK)
						{
							urls.push_back(string(u.getCanonical()));
							//cout<<href<<endl;
						}
					}					
				}//end if href =0
				it++;
			}//end while
		}//end if pn == a
		return HyXML::OK;
	}
	return HyXML::OK;
}

bool processText(_char *pText)
{
	localTextcontent += pText;
	localTextcontent +=" ";
	//cout<<pText<<endl;
	return true;
}

bool processComment(_char *pComment)
{
	return false;
}

//获取本地Html的内容
void getLocalHtml(const char* file)
{
	localTextcontent = "";
	urls.clear();
	setlocale(LC_ALL, "en_IN.UTF-8");
	MmapWrapper mmapWrapper(file);
	cout<<"getLocalHtml();"<<endl;
	_char *pBuffer = 0;
#ifdef HYPAR_WIDE_CHAR
	ToWideCharConverter toWideCharConverter("UTF-8");
	pBuffer = reinterpret_cast<_char *> (toWideCharConverter.convert(
			mmapWrapper.getBuffer(), mmapWrapper.getBufferLength()
			));
#else
	int len = mmapWrapper.getBufferLength() + 1;
	pBuffer = new char[len];
	bzero(pBuffer, len);
	memcpy(pBuffer, mmapWrapper.getBuffer(), len -1);
#endif
	cout<<"Pass2"<<endl;
	//cout<<"P"<<endl<<pBuffer<<endl;
	HyXML h;
	h.tagCb = &processTag;
	h.textCb = &processText;
	h.commentCb = &processComment;
	HyXML::DOMNode node(HyXML::DOMNode::ELEMENT, L("root"));
	node.m_iLevel = -1;
	cout<<"Pass3"<<endl;

	try{
		h.parse(pBuffer, &node);
	}
	catch(...){}

	cout<<"Pass4"<<endl;
	delete pBuffer;
	cout<<"Pass5"<<endl;
}

int main(int argc, char *argv[])
{
	getLocalHtml(argv[1]);
}



