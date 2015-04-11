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

#ifndef _HYPAR_MMAPWRAPPER_H_
#define _HYPAR_MMAPWRAPPER_H_

#include <hypar/localdefs.h>

/**
 * Wrapper call for the mmap (2) call. Easier reading from files!
 */
class MmapWrapper
{
	public:
		typedef enum
		{
			EMMAPINVAL = -100,
			EMMAPFILEREAD,
			OK = 0
		}
		Status;
		
	private:
		Status m_iStatus;
		char const *m_pFileName;
		char const *m_pMemBuffer;
		long m_lFileLen;

	private:
		MmapWrapper();
		inline int init();

	public:
		/**
		 * The constructor, with the file-name
		 */
		MmapWrapper(const char *pFileName);
		~MmapWrapper();

	public:
		Status getStatus();
		/*
		 * Get the pointer to the base of the buffer where the file's contents
		 * exist
		 */
		const char *getBuffer();
		const char *getFileName();
		/**
		 * Call to get the length of the file
		 */
		long getBufferLength();
};
#endif // _HYPAR_MMAPWRAPPER_H_
