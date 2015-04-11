/*
 * Copyright 2004 Ravindra Jaju (jaju [AT] it iitb ac in)
 *
 * This file is part of the HyParSuite.
 *
 * HyParSuite is free software.  You can redistribute it and/or modify 
 * it under the terms of version 1 of the Affero General Public License
 * (AGPL) as published by Affero, Inc.
 *
 * HyParSuite is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero General Public License for more details.
 *
 * You should have received a copy of the Affero General Public License in
 * the COPYING file that comes with HyParSuite. If not, write to 
 * Affero, Inc., 510 Third Street, Suite 225, San Francisco, CA 94107, USA.
 */

#include <hypar/localdefs.h>
#include <hypar/MmapWrapper.hpp>
#include <hypar/ToWideCharConverter.hpp>
#include <hypar/WideCharEntityStream.hpp>
#include <hypar/HyXML.hpp>
#include <hypar/HashUtils.hpp>


#define DEBUG 1
#include <hypar/Debug.hpp>

#include <iostream>
#include <fstream>
#include <queue>
#include <string>

BEGIN_C_DECLS
#include <assert.h>
#include <stdio.h>
#include <locale.h>
END_C_DECLS

using namespace std;
#if __GNUC__ > 2
using namespace __gnu_cxx;
#endif

typedef hash_set<const _char *, hashcase<const _char *>, eqcase> StringSet;

const _char *ignoreTagArray[] =
{
	L("script"),
	0
};

const _char *specialTagArray[] =
{
	L("a"),
	0
};

StringSet ignoreTagSet;
StringSet::iterator itsx;
static const _char *pCurrentTag = 0;

HyXML::EntityCbRetval processTag(HyXML::Tag *pTag)
{
	if (!pTag->m_bIsEndTag)
	{
		itsx = ignoreTagSet.find(pTag->m_pName);
		if (itsx != ignoreTagSet.end())
		{
			pCurrentTag = *itsx;
			return HyXML::IGNORE_CHILDREN;
		}
	}
//cout<<baseUrl<<endl;
	string pn = pTag->m_pName;
	if(pn =="a")
	{
		cout<<pn<<endl;

		list<pair<_char *, _char *> >::iterator it = pTag->m_attrList.begin(); 
		while(it != pTag->m_attrList.end()) 
		{ 
			if(strcmp(it->first,"href")==0)
			{
				string href = it->second;
				if(href.size()<11 || href.substr(0,11)!="javascript:")
					cout<<href<<endl;
			}
				//cout<<it->first<<"="<<it->second<<endl;
			it++;
		} 

		//cout<<pTag->findProperty("href")<<" {}"<<endl;
		//if(value!=NULL)
		//{
		//	cout<<value<<endl;
		
		//}
		//for(int i=0 ; i < pTag->m_attrList.size();i++)
		//{
		//	if(pTag->m_attrList[i].first=="herf")
		//		cout<<pTag->m_attrList[i].second<<endl;
		//}
		//cout<<pTag->m_pName<<endl;
	}
	else	
	{
	  //cout<<pTag->m_pName<<" []"<<endl;
	}
	return HyXML::OK;
}

bool
processText(_char *pText)
{
	return true;
}

bool
processComment(_char *pComment)
{
	return true;
}

void usage(int argc, char *argv[])
{
	cerr << "Usage: " << argv[0] << " <file>" << endl;
}


int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		usage(argc, argv);
		return -1;
	}
	const _char **p = ignoreTagArray;
	while (*p)
	{
		ignoreTagSet.insert(*p);
		p++;
	}
	setlocale(LC_ALL, "en_IN.UTF-8");
	MmapWrapper mmapWrapper(argv[1]);
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
	HyXML h;
	//h.m_baseUrl = "cc";
	h.tagCb = &processTag;
	h.textCb = &processText;
	h.commentCb = &processComment;
	HyXML::DOMNode node(HyXML::DOMNode::ELEMENT, L("root"));
	node.m_iLevel = -1;
	

	//string cc = "cccc";
	h.parse(pBuffer, &node);
		
	_string text = L("");
	node.m_pChild->m_pChild->toString(text);

	char *pLocalLocaleBuffer = new char[text.size()*4 + 4];
	wcstombs(pLocalLocaleBuffer, text.c_str(), text.size()*4 + 4);
	//cout << pLocalLocaleBuffer << endl;

	delete pBuffer;
	return 0;
}
