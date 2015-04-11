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
 * the COPYING file that comes with HyParSuite. If not, write to 
 * Affero, Inc., 510 Third Street, Suite 225, San Francisco, CA 94107, USA.
 */

#include <hypar/localdefs.h>
#include <hypar/IconvWrapper.hpp>

#define DEBUG 0
#include <hypar/Debug.hpp>

BEGIN_C_DECLS
#include <assert.h>
#include <string.h>
#include <errno.h>
END_C_DECLS

IconvWrapper::IconvWrapper(
        const char *pFromEncoding,
        const char *pToEncoding
        )
:
m_pFromEncoding(pFromEncoding),
m_pToEncoding(pToEncoding),
m_iIconvDescriptor((iconv_t)-1)
{
	int _errno;
    m_iIconvDescriptor =
        iconv_open(m_pToEncoding, m_pFromEncoding);
	_errno = errno;
    if (m_iIconvDescriptor != (iconv_t) -1)
    {
        m_iStatus = IconvWrapper::OK;
    }
    else
    {
		debug2("iconv_open failed!", strerror(_errno));
        m_iStatus = IconvWrapper::EICONVINVAL;
    }
}

IconvWrapper::~IconvWrapper()
{
	if (m_iIconvDescriptor < 0)
	{
		assert(iconv_close(m_iIconvDescriptor) == 0);
	}
}

size_t
IconvWrapper::convert(
        const char **pInbuf, size_t *pInbytesleft,
        char **pOutbuf, size_t *pOutbytesleft
        )
{
    size_t retval;
	int _errno;
    retval = iconv(m_iIconvDescriptor,
            (char **) pInbuf, pInbytesleft,
            pOutbuf, pOutbytesleft);
	_errno = errno;
	if (retval == (size_t) -1)
	{
		debug2("convert::iconv failed!", strerror(_errno));
		block();
	}
    return retval;
}
