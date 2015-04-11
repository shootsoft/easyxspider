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

#ifndef _HYPAR_STRUTILS_H_
#define _HYPAR_STRUTILS_H_

#include <hypar/localdefs.h>
#include <string>

using namespace std;

/**
 * General helper functions for working with strings (C and C++), of 'both'
 * kinds - 'char' and 'wchar_t'
 */
class TextUtils
{
	public:
		static void tolower(string &s);
		static void tolower(char *s);
#ifdef HYPAR_WIDE_CHAR
		static void towlower(_string &s);
		static void towlower(_char *s);
		static string wideToNarrow(const wstring &ws);
		static string wideToNarrow(const wchar_t *ws);
#endif

	public:
		static _string echoSpaces(int iNumSpaces);
		static _string formatContent(const _char *pContent, int iNumIndents);
		static _string itows(int ii);
		static string toLocalLocale(const _string &s);
		static string toLocalLocale(const _char *s);

};

#endif
