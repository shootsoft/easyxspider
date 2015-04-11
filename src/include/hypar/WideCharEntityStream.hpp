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

#ifndef _HYPAR_WIDECHARENTITYSTREAM_H_
#define _HYPAR_WIDECHARENTITYSTREAM_H_

#include <hypar/localdefs.h>

/**
 * Class for creating a stream of 'entities' from a buffer. The buffer will be
 * edited by this class! Entities are 'tag', 'text' and 'comment', as in the
 * context of XML
 */
class WideCharEntityStream
{
	public:
		typedef enum
		{
			EWCSTREAMINVALID = -100,
			EWCSTREAMEOF,
			OK = 0
		}
		Status;

		typedef enum
		{
			UNINIT = -2,
			END = -1,
			ELEMENT = 0,
			COMMENT,
			TEXT
		}
		EntityType;

	private:
		_char *m_pStart, *m_pEnd, *m_pCur;
		long m_lBufLength;
		Status m_iStatus;
		EntityType m_prevEntity;

	public:
		WideCharEntityStream();
		WideCharEntityStream(_char *pBuffer, long lBufLength);
		~WideCharEntityStream() {}

	public:
		void setSourceBuffer(_char *pBuffer, long lBufLength);
		void reset();

		unsigned long advance(unsigned long lLenToAdvance = 1);
		unsigned long rewind(unsigned long lLenToRewind = 1);

		_char peek();
		_char get();
		_char *getBuffer();
		_char *getBufferBase();
		_char *getTill(const _char stopChar, long &lOutLen, bool &bFound,
				bool bAdvance = false);

		_char *getUntilElement(const _char *pElementName, long &outLen,
				bool &bFound, bool bAdvance = false); /* Not Implemented */

		_char *getTillFirstOf(const _char stopChar_1,
				const _char stopChar_2,
				long &lOutLen, _char &charFound, bool bAdvance = false);

		bool set(const _char c);
		bool setNull();
		bool putBack(const _char c);

		long getRemainingLength();
		long getBufferLength();
		void setPrevEntity(EntityType et);

	public:
		EntityType getNextEntity(_char **pEntityStr,
				bool bSetNull = false);

};

#endif // _HYPAR_WIDECHARENTITYSTREAM_H_
