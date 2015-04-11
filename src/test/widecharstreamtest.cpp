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
	cerr << "Usage: " << argv[0] << " <in-encoding> <file>" << endl;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		usage(argc, argv);
		return -1;
	}
#ifdef HYPAR_WIDE_CHAR
	//setlocale(LC_ALL, argv[1]);
	setlocale(LC_ALL, "en_IN.UTF-8");
	//setlocale(LC_ALL, "POSIX");
	ToWideCharConverter toWideCharConverter(argv[1]);
	MmapWrapper mmapWrapper(argv[2]);
	_char *pBuffer = reinterpret_cast<_char *> (toWideCharConverter.convert(
			mmapWrapper.getBuffer(), mmapWrapper.getBufferLength()
			));
	WideCharEntityStream wideCharStream(
			pBuffer, _strlen(pBuffer)
			);

#if 0
	_char c;
	while ((c = wideCharStream.get()) != L('\0'))
	{
		_cout << c << flush;
	}
#endif
	_char *wchar_ptr;
	_string ws, textws;
	_string wsold;
	queue<_char *> entity_queue;
	bool bFound;
	long len;
	wofstream wofs("tmp.out", ios::out);

	textws = L"";
	ws = L"";
	char tempLocalLocaleBasedBuffer[2048];
	while ((wchar_ptr = wideCharStream.getTill(
				L('<'), len, bFound, true
				))
			!= 0
		  )
	{
		textws.append(wchar_ptr, len - 1);
		if (bFound)
		{
			wsold = ws;
			wideCharStream.rewind(1);
			wchar_ptr = wideCharStream.getTill(
					L('>'), len, bFound, true
					);
			if (wchar_ptr)
			{
				ws.erase();
				ws.assign(wchar_ptr, len);
#if 0
				int i = 0;
				while (i < len)
				{
					_cout << (wint_t) wchar_ptr[i] << endl;
					_cout << wchar_ptr[i] << endl;
					i++;
				}
#endif
				wofs << ws << endl;
				bzero(tempLocalLocaleBasedBuffer, 2048);
				wcstombs(tempLocalLocaleBasedBuffer, ws.c_str(), 2048);
				_cout << tempLocalLocaleBasedBuffer << endl;
				cerr << tempLocalLocaleBasedBuffer << endl;
			}
#if 0
			if (!wofs.good())
			{
				int i = 0;
				int j = 0;
				const _char *abc = ws.c_str();
				for (; i < _strlen(abc); i++)
				{
					if (j > 10)
					{
						j = 0;
						//cout << endl;
					}
					if (static_cast<wint_t>(abc[i]) < 128)
					{
						cout << static_cast<char>(abc[i]);
					}
					else
					{
						//cout << "|" << static_cast<wint_t>(abc[i]) << "|";
						fprintf(stdout, "%d", (wint_t)abc[i]);
					}
					j++;
				}
				cout << endl;
				block();
			}
#endif
		}
	}
	cout << "The remaining length is " << wideCharStream.getRemainingLength() << endl;
#endif
	return 0;
}
