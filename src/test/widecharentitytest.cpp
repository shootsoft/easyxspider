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

#define DEBUG 1
#include <hypar/Debug.hpp>

#include <iostream>
#include <fstream>
#include <queue>

BEGIN_C_DECLS
#include <assert.h>
#include <stdio.h>
#include <locale.h>
END_C_DECLS

using namespace std;

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
#ifdef HYPAR_WIDE_CHAR
	setlocale(LC_ALL, "en_IN.UTF-8");
	ToWideCharConverter toWideCharConverter("UTF-8");
	MmapWrapper mmapWrapper(argv[1]);
	_char *pBuffer = reinterpret_cast<_char *> (toWideCharConverter.convert(
			mmapWrapper.getBuffer(), mmapWrapper.getBufferLength()
			));
	WideCharEntityStream wideCharStream(
			pBuffer, _strlen(pBuffer)
			);
	HyXML h;

	_char *wchar_ptr;
	_string ws, textws;
	_string wsold;
	queue<_char *> entity_queue;

	textws = L("");
	ws = L("");
	char tempLocalLocaleBasedBuffer[2048];
	WideCharEntityStream::EntityType type;
	while ((type = wideCharStream.getNextEntity(&wchar_ptr, true))
			!= WideCharEntityStream::END)
	{
		if (type == WideCharEntityStream::TEXT)
		{
			textws.append(wchar_ptr);
		}
		switch(type)
		{
			case WideCharEntityStream::ELEMENT:
				{
					HyXML::Tag t(wchar_ptr, false);
					_string s;
					t.toString(s);
					_cout << s << endl;
				}
				break;
			case WideCharEntityStream::COMMENT:
				_cout << L("<!--") << wchar_ptr << L("-->") << endl;
				break;
			case WideCharEntityStream::TEXT:
				_cout << wchar_ptr << endl;
				break;
			default:
				break;
		}
		ws.erase();
		ws.assign(wchar_ptr);
		bzero(tempLocalLocaleBasedBuffer, 2048);
		wcstombs(tempLocalLocaleBasedBuffer, ws.c_str(), 2048);
	}
	delete pBuffer;
#endif
	return 0;
}
