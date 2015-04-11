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

#include <iostream>

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
	MmapWrapper mmapWrapper(argv[1]);

	cout << "---------------------------------------" << endl;
	for (int i = 0; i < mmapWrapper.getBufferLength(); i++)
	{
		cout << *(mmapWrapper.getBuffer() + i) << flush;
	}
	cout << "---------------------------------------" << endl;

	return 1;
}
