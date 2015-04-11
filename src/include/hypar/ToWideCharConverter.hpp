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

#ifndef _HYPAR_TOWIDECHARCONVERTER_H_
#define _HYPAR_TOWIDECHARCONVERTER_H_

#include <hypar/localdefs.h>

#include <hypar/IconvWrapper.hpp>

/**
 * Class to convert from any supported encoding to the wide-char form
 */
class ToWideCharConverter
{
	private:
		static const char* m_pToEncoding;
		char const *m_pFromEncoding;
		char *m_pOutbuf;
		IconvWrapper::Status m_iStatus;
		IconvWrapper iconvWrapper;

	private:
		ToWideCharConverter();

	public:
		ToWideCharConverter(const char *pFromEncoding);
		~ToWideCharConverter();

	public:
		IconvWrapper::Status getStatus()
		{
			return iconvWrapper.getStatus();
		}

		/**
		 * Call this when you want ToWideCharConverter to allocate space for
		 * you and copy the converted text to the allocated buffer. The caller
		 * has to delete the buffer
		 */
		char *convert(
				const char *pInbuf, size_t pInbytesleft
				);
		
		/**
		 * Call this function when space where the output is to be stored is
		 * already allocated by the user
		 */
		size_t convert(
				const char **pInbuf, size_t *pInbytesleft,
				char **pOutbuf, size_t *pOutbytesleft
				);

		char *convert2(
				const char *pInbuf, size_t pInbytesleft
				); // Don't use!
		
};
				
#endif // _HYPAR_TOWIDECHARCONVERTER_H_
