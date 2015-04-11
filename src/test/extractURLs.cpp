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
#include <hypar/URL.hpp>
#include <hypar/HashUtils.hpp>
#include <hypar/TextUtils.hpp>


#define DEBUG 1
#include <hypar/Debug.hpp>

#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <list>

BEGIN_C_DECLS
#include <assert.h>
#include <stdio.h>
#include <locale.h>
END_C_DECLS

using namespace std;
#if __GNUC__ > 2
using namespace __gnu_cxx;
#endif
list<_char *> URLList;
unsigned int iMaxURLLen = 0;

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
	setlocale(LC_ALL, "UTF-8");
	MmapWrapper mmapWrapper(argv[1]);
	_char *pBuffer = 0;
#ifdef HYPAR_WIDE_CHAR
	ToWideCharConverter toWideCharConverter("UTF-8");
	pBuffer = reinterpret_cast<_char *> (toWideCharConverter.convert(
			mmapWrapper.getBuffer(), mmapWrapper.getBufferLength()
			));
#else
	pBuffer = new char[mmapWrapper.getBufferLength() + 1];
	bzero(pBuffer, mmapWrapper.getBufferLength() + 1);
	memcpy(pBuffer, mmapWrapper.getBuffer(), mmapWrapper.getBufferLength());
#endif
	URLCollector::m_baseHref = "http://www.163.com";
	URLCollector url_collector(pBuffer);
	list<string>::iterator urlx = url_collector.m_URLList.begin();
	while (urlx != url_collector.m_URLList.end())
	{
		cout << (*urlx).c_str() << endl;
		urlx++;
	}
	delete pBuffer;
	return 0;
}
