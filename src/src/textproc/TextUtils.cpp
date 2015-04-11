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
#include <hypar/TextUtils.hpp>
#define DEBUG 1
#include <hypar/Debug.hpp>

#include <algorithm>

using namespace std;

void
TextUtils::tolower(string &s)
{
	transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::tolower);
}

void
TextUtils::tolower(char *s)
{
	transform(s, s + strlen(s), s, (int(*)(int))std::tolower);
}

#ifdef HYPAR_WIDE_CHAR
void
TextUtils::towlower(_string &s)
{
	transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::towlower);
}

void
TextUtils::towlower(_char *s)
{
	transform(s, s + _strlen(s), s, (int(*)(int))std::towlower);
}
#endif

_string
TextUtils::echoSpaces(int iNumSpaces)
{
	_string spaces = L("");
	if (iNumSpaces <= 0)
		return spaces;
	spaces.assign(iNumSpaces, L(' '));
	return spaces;
}

_string
TextUtils::formatContent(const _char *pContent, int iNumIndents)
{
	_string formattedString = L("");
	int currentCursor = 0;
	_string spacePrefix = echoSpaces(iNumIndents);
	if (iNumIndents <= 0)
	{
		formattedString = pContent;
		return formattedString;
	}
	while (1)
	{
		if (!*pContent)
			break;
		for (currentCursor = 0; *pContent &&
				*(pContent + currentCursor) != L('\n'); currentCursor++);
		if (*(pContent + currentCursor) == L('\n'))
			currentCursor++;
		formattedString += spacePrefix;
		formattedString.append(pContent, currentCursor + 1);
		pContent += currentCursor + 1;
	}
	return formattedString;
}

_string
TextUtils::itows(int ii)
{
	_char wbuf[17];
	char buf[17];
	wbuf[16] = 0;
	buf[16] = 0;
	int i = ii;

	_char *retstr = wbuf + 15;
	char *p = buf + 15;

	*retstr = L('0');
	*p = '0';

	int residue = 0;
	bool cut = false;
	while (1)
	{
		if (cut)
			break;
		if (i < 10)
		{
			cut = true;
		}
		residue = i % 10;
		*retstr = L('0') + residue;
		*p = '0' + residue;
		i = i/10;
		retstr--; p--;
	}
	return _string(retstr + 1);
}

#ifdef HYPAR_WIDE_CHAR
string
TextUtils::wideToNarrow(const wstring &ws)
{
	return wideToNarrow(reinterpret_cast<const wchar_t *>(ws.c_str()));
}

string
TextUtils::wideToNarrow(const wchar_t *ws)
{
	string return_string;
	int bufsz = (wcslen(ws) + 1) * sizeof(wchar_t);
	char *pLocalLocaleBuffer = new char[bufsz];
	wcstombs(pLocalLocaleBuffer, ws, bufsz);
	return_string.assign(pLocalLocaleBuffer);
	delete pLocalLocaleBuffer;
	return return_string;
}
#endif

string
TextUtils::toLocalLocale(const _string &s)
{
#ifdef HYPAR_WIDE_CHAR
	return wideToNarrow(s.c_str());
#else
	return s;
#endif
}

string
TextUtils::toLocalLocale(const _char *s)
{
#ifdef HYPAR_WIDE_CHAR
	return wideToNarrow(s);
#else
	return string(s);
#endif
}
