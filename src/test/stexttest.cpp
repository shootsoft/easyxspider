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

string content;

HyXML::EntityCbRetval processTag(HyXML::Tag *pTag)
{
	if(strcmp(pTag->m_pName,"script")==0)
		return HyXML::IGNORE_CHILDREN;
	else
 		return HyXML::OK;
}

bool
processText(_char *pText)
{
	content += pText;
	content +=" ";
	//cout<<pText<<endl;
	return true;
}

bool
processComment(_char *pComment)
{
	return false;
}

void usage(int argc, char *argv[])
{
	cerr << "Usage: " << argv[0] << " <file>" << endl;
}

void getLocalHtml(const char* file)
{
	setlocale(LC_ALL, "en_IN.UTF-8");
	MmapWrapper mmapWrapper(file);
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
	h.tagCb = &processTag;
	h.textCb = &processText;
	h.commentCb = &processComment;
	HyXML::DOMNode node(HyXML::DOMNode::ELEMENT, L("root"));
	node.m_iLevel = -1;
	h.parse(pBuffer, &node);

	//_string text = L("");
	//node.m_pChild->m_pChild->toString(text);

	//char *pLocalLocaleBuffer = new char[text.size()*4 + 4];
	//wcstombs(pLocalLocaleBuffer, text.c_str(), text.size()*4 + 4);
	//cout << pLocalLocaleBuffer << endl;

	delete pBuffer;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		usage(argc, argv);
		return -1;
	}
	getLocalHtml(argv[1]);
	cout<<content;
	
	return 0;
}
