/*
 * Copyright 2004 Ravindra Jaju (jaju [AT] it iitb ac in)
 *
 * This file is part of the HyParSuite
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

#ifdef _HYPAR_DEBUG_H_

#undef debug
#undef wdebug
#undef debug2
#undef wdebug2
#undef debug3
#undef wdebug3
#undef debug4
#undef wdebug4

#undef block

#endif

#ifndef _HYPAR_DEBUG_H_
#define _HYPAR_DEBUG_H_
#endif

/**
 * Convert to wchar_t * and return a pointer to the converted (static)
 * buffer location
 */
char *echowide(const _char *s);
#ifdef HYPAR_WIDE_CHAR
const char *echowide(const char *s);
#endif
long echowide(long l);

#if DEBUG

#include <iostream>
#include <cassert>

using namespace std;

#define debug(a) cerr << __FILE__ << ":" << __LINE__ << " : " \
        << a << endl;
#define wdebug(a) cerr << __FILE__ << ":" << __LINE__ << " : " \
        << echowide(a) << endl;
#define debug2(a,b) cerr << __FILE__ << ":" << __LINE__ << " : " \
        << a << " " << b << endl;
#define wdebug2(a,b) cerr << __FILE__ << ":" << __LINE__ << " : " \
        << echowide(a) << " " << echowide(b) << endl;
#define debug3(a,b,c) cerr << __FILE__ << ":" << __LINE__ << " : " \
        << a << " " << b << " " << c << endl;
#define wdebug3(a,b,c) cerr << __FILE__ << ":" << __LINE__ << " : " \
        << echowide(a) << " " << echowide(b) << " " << echowide(c) << endl;
#define debug4(a,b,c,d) cerr << __FILE__ << ":" << __LINE__ << " : " \
        << a << " " << b << " " << c << " " << d << endl;
#define wdebug4(a,b,c,d) cerr << __FILE__ << ":" << __LINE__ << " : " \
	<< echowide(a) << " " << echowide(b) << " " << echowide(c) << \
	" " << echowide(d) << endl;
#define debug_print(a, b) printf(a, b)
#define wdebug_print(a, b) wprintf(a, b)
#define block() \
{ \
	cerr << __FILE__ << ":" << __LINE__ \
	<< " - Enter any character to continue " \
	<< flush; \
		char i; \
		cin >> i; \
}

#define debug_assert(a) assert(a);

#else

#define debug(a)
#define wdebug(a)
#define debug2(a,b)
#define wdebug2(a,b)
#define debug3(a,b,c)
#define wdebug3(a,b,c)
#define debug4(a,b,c,d)
#define wdebug4(a,b,c,d)
#define debug_print(a)
#define debug_assert(a)

#define block()

#endif
