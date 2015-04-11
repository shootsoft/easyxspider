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

#ifndef _HYPAR_ICONV_WRAPPER_H_
#define _HYPAR_ICONV_WRAPPER_H_

#include <hypar/localdefs.h>

BEGIN_C_DECLS
#include <iconv.h>
END_C_DECLS

/**
 * Wrapper class for iconv (3)
 */
class IconvWrapper
{
    public:
        typedef enum
        {
            EICONVINVAL = -1,
            OK
        }
        Status;
        
    private:
        char const *m_pFromEncoding;
        char const *m_pToEncoding;
        iconv_t m_iIconvDescriptor;
        IconvWrapper::Status m_iStatus;

    private:
        IconvWrapper();

    public:
		/**
		 * Constructor for IconvWrapper with from and to encoding-parameters
		 */
        IconvWrapper(
                const char *pFromEncoding,
                const char *pToEncoding
                );

		~IconvWrapper();

	public:
		IconvWrapper::Status getStatus()
		{
			return m_iStatus;
		}
        
		/**
		 * Convert from one encoding to another, with the same meaning for the
		 * parameters as in the iconv (3) call, but with the iconv_t parameter
		 * not needed.
		 */
        size_t convert(
                const char **pInbuf, size_t *pInbytesleft,
                char **pOutbuf, size_t *pOutbytesleft
                );
};
        
#endif // _HYPAR_ICONV_WRAPPER_H_
