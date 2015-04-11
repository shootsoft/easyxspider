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

#ifndef _HYPAR_LOCALDEFS_H_
#define _HYPAR_LOCALDEFS_H_

#include <string>

#ifdef HYPAR_WIDE_CHAR
# include <wchar.h>
# include <wctype.h>
# define L(x) L##x
# define _strcmp wcscmp
# define _strncmp wcsncmp
# define _strcasecmp wcscasecmp
# define _strncasecmp wcsncasecmp
# define _strrchr wcsrchr
# define _strstr wcswcs
# define _strdup wcsdup
# define _strlen wcslen
# define _tolower towlower
# define _isspace iswspace
# define _char wchar_t
# define _cout wcout
# define _cerr wcerr
typedef std::wstring _string;
#else
# define L(x) x
# define _strcmp strcmp
# define _strncmp strncmp
# define _strcasecmp strcasecmp
# define _strncasecmp strncasecmp
# define _strrchr strrchr
# define _strstr strstr
# define _strdup strdup
# define _strlen strlen
# define _tolower tolower
# define _isspace isspace
# define _char char
# define _cout cout
# define _cerr cerr
# define wcstombs strncpy
# define mbstowcs strncpy
typedef std::string _string;
#endif

#define LANGLE L('<')
#define GANGLE L('>')
#define BANG L('!')
#define MINUX L('-')
#define SLASH L('/')
#define NULLCHAR L('\0')

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

#endif // _HYPAR_LOCALDEFS_H_
