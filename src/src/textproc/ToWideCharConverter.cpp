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

#include <iostream>
#include <hypar/ToWideCharConverter.hpp>

#define DEBUG 0
#include <hypar/Debug.hpp>

BEGIN_C_DECLS
#include <assert.h>
#include <strings.h>
#include <stdlib.h>
END_C_DECLS

const char* ToWideCharConverter::m_pToEncoding = "WCHAR_T";

ToWideCharConverter::ToWideCharConverter (
		const char *pFromEncoding
		)
:
m_pFromEncoding(pFromEncoding),
m_pOutbuf(0),
iconvWrapper(m_pFromEncoding, m_pToEncoding)
{
	setlocale(LC_ALL, m_pFromEncoding);
}

ToWideCharConverter::~ToWideCharConverter()
{
}

char *
ToWideCharConverter::convert(
		const char *pInbuf, size_t inbytesLeft
		)
{
	size_t outbufLen = sizeof(_char) * (inbytesLeft + 1);
	char *pTempOutBuf;
	m_pOutbuf = new char[outbufLen];
	assert(m_pOutbuf);
	bzero(m_pOutbuf, outbufLen);
	pTempOutBuf = m_pOutbuf;
	inbytesLeft = convert(&pInbuf, &inbytesLeft, &pTempOutBuf, &outbufLen);
	debug2("Bytes converted in an irreversible fashion", inbytesLeft);
	return m_pOutbuf; // To be deleted by caller!
}

size_t
ToWideCharConverter::convert(
		const char **pInbuf, size_t *pInbytesleft,
		char **pOutbuf, size_t *pOutbytesleft
		)
{
	return iconvWrapper.convert(
			pInbuf, pInbytesleft,
			pOutbuf, pOutbytesleft
			);
}

/**
 * The following uses the mbstowcs() call to convert to wide characters.
 */

/*
char *
ToWideCharConverter::convert2(
		const char *pInbuf, size_t inbytesLeft
		)
{
	size_t outbufLen = mbstowcs(NULL, pInbuf, 0) + 2; // This is num_of_wchars!
	char *pTempOutBuf;
	debug2("The length of the input string is", outbufLen - 2);
	m_pOutbuf = new char[outbufLen * sizeof(_char)];
	bzero(m_pOutbuf, outbufLen);
	pTempOutBuf = m_pOutbuf;
	inbytesLeft = mbstowcs((_char *) m_pOutbuf, pInbuf, outbufLen);
	debug2("Bytes converted ", inbytesLeft);
	return m_pOutbuf;
	return 0;
}
*/
