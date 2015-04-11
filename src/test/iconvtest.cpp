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
 * the COPYING file that comes with Nalanda iVia.  If not, write to 
 * Affero, Inc., 510 Third Street, Suite 225, San Francisco, CA 94107, USA.
 */

#include <hypar/localdefs.h>
#define DEBUG 1
#include <hypar/Debug.hpp>
#include <hypar/ToWideCharConverter.hpp>
#include <hypar/MmapWrapper.hpp>

#include <iostream>

BEGIN_C_DECLS
#include <wchar.h>
END_C_DECLS

using namespace std;

void usage(int argc, char *argv[])
{
	cerr << "Usage: " << argv[0] << " <in-encoding> <filename>" << endl;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		usage(argc, argv);
		return -1;
	}

#ifdef HYPAR_WIDE_CHAR
	_char *pBuffer = 0;
	ToWideCharConverter toWideCharConverter(argv[1]);

	if (toWideCharConverter.getStatus() != IconvWrapper::OK)
	{
		cerr << "Error in creating a converter to wide_char from " <<
			argv[1] << endl;
		return -1;
	}
	
	MmapWrapper mmapWrapper(argv[2]);

	if (mmapWrapper.getStatus() != MmapWrapper::OK)
	{
		cerr << "Error in mmap'ing file " << argv[1] << endl;
		return -1;
	}

	pBuffer = reinterpret_cast<_char *> (
			toWideCharConverter.convert(
			mmapWrapper.getBuffer(), mmapWrapper.getBufferLength()
			)
			);

	if (!pBuffer)
	{
		cerr << "Could not convert to wide_char" << endl;
		return -1;
	}

	_cout << pBuffer << endl;
	block();

	int i = 0;
	int bufLen = _strlen(pBuffer);

	while (i < bufLen)
	{
		if (*(pBuffer + i) < 256)
			cout << (char) *(pBuffer + i);
		i++;
	}
	cout << endl;
	cout << "The length of the wide_char buffer is " << _strlen(pBuffer) << endl;
#endif
	return 0;
}
