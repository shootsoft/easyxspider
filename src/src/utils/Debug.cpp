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
#include <hypar/Debug.hpp>

#include <iostream>

#define BUFSZ 16*1024
char strBuf[BUFSZ];

char *echowide(const _char *s)
{
	*strBuf = 0;
	int len = _strlen(s) < BUFSZ ? _strlen(s) + 1 : BUFSZ;
	bzero(strBuf, BUFSZ);
	wcstombs(strBuf, s, len);
	return strBuf;
}

#ifdef HYPAR_WIDE_CHAR
const char *echowide(const char *s)
{
	return s;
}
#endif
