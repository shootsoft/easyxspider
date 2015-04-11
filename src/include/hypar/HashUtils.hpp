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

#ifndef _HYPAR_HASHUTILS_H_
#define _HYPAR_HASHUTILS_H_

#include <hypar/localdefs.h>

#include <string>
#if __GNUC__ < 3
#include <hash_map>
#include <hash_set>
#else
#include <ext/hash_map>
#include <ext/hash_set>
#endif

using namespace std;

#if __GNUC__ > 2
namespace __gnu_cxx {
#endif

#ifdef HYPAR_WIDE_CHAR
	/**
	 * Computes and returns the hash of a wide character
	 */
	inline size_t __stl_hash_string(const wchar_t *__s)
	{
		unsigned long __h = 0;
		for ( ; *__s; ++__s)
			__h = 5*__h + *__s;

		return size_t(__h);
	}
#endif

	template <class _Key> struct hashcase { };

	template <> struct hashcase<const _char *>
	{
		size_t operator()(const _char *__s) const // Why's the last const
			                                        // needed?
		{
			unsigned long __h = 0;
			for ( ; *__s; ++__s)
				__h = 5*__h + _tolower(*__s);
			return size_t(__h);
		}
	};

#if __GNUC__ > 2
} // __gnu_cxx
#endif

struct eqcase
{
	bool operator()(basic_string<_char>& str1, basic_string<_char>& str2)
	{
		return _strcasecmp(str1.c_str(), str2.c_str()) == 0;
	}
	bool operator()(const _char *str1, const _char *str2)
	{
		int iLen1, iLen2;
		while (_isspace(*str1))
		{
			str1++;
		}
		while (_isspace(*str2))
		{
			str2++;
		}
		iLen1 = _strlen(str1);
		iLen2 = _strlen(str2);
		return _strncasecmp(str1, str2, MIN(iLen1, iLen2)) == 0;
	}
	bool operator()(const _char *str1, const _char *str2) const
	{
		int iLen1, iLen2;
		while (_isspace(*str1))
		{
			str1++;
		}
		while (_isspace(*str2))
		{
			str2++;
		}
		iLen1 = _strlen(str1);
		iLen2 = _strlen(str2);
		return _strncasecmp(str1, str2, MIN(iLen1, iLen2)) == 0;
	}
};

#endif // _HYPAR_HASHUTILS_H_
