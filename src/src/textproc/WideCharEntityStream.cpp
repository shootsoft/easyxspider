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
#define DEBUG 0
#include <hypar/Debug.hpp>

#include <hypar/WideCharEntityStream.hpp>
#include <iostream>

BEGIN_C_DECLS
#include <assert.h>
END_C_DECLS

using namespace std;

WideCharEntityStream::WideCharEntityStream()
:
m_pStart(0),
m_pEnd(0),
m_pCur(0),
m_lBufLength(0),
m_iStatus(WideCharEntityStream::EWCSTREAMINVALID),
m_prevEntity(WideCharEntityStream::UNINIT)
{
}

WideCharEntityStream::WideCharEntityStream(
		_char *pBuffer, long lBufLength
		)
{
	setSourceBuffer(pBuffer, lBufLength);
}

void
WideCharEntityStream::setSourceBuffer(
		_char *pBuffer, long lBufLength
		)
{
	assert(pBuffer != 0);
	assert(lBufLength != 0);
	m_pStart = m_pCur = pBuffer;
	m_lBufLength = lBufLength;
	m_pEnd = m_pStart + m_lBufLength; // 1 past the end!!
	if (*m_pEnd != L('\0'))
	{
		cerr << "Please use only NULL terminated buffers" << endl;
		assert(0);
	}
	m_iStatus = OK;
	m_prevEntity = UNINIT;
}

void
WideCharEntityStream::reset()
{
	assert(m_iStatus != EWCSTREAMINVALID);
	m_pCur = m_pStart;
	m_iStatus = WideCharEntityStream::OK;
}

unsigned long
WideCharEntityStream::advance(unsigned long lLenToAdvance)
{
	unsigned long retval = 0;
	if (likely(m_pCur + lLenToAdvance < m_pEnd))
	{
		m_pCur += lLenToAdvance;
		return lLenToAdvance;
	}
	else
	{
		retval = m_pEnd - m_pCur;
		m_pCur = m_pEnd;
		m_iStatus = WideCharEntityStream::EWCSTREAMEOF;
		return retval;
	}
}

unsigned long
WideCharEntityStream::rewind(unsigned long lLenToRewind)
{
	assert(m_iStatus != WideCharEntityStream::EWCSTREAMINVALID);
	unsigned long retval = 0;
	if (m_pCur - lLenToRewind > m_pStart)
	{
		m_pCur -= lLenToRewind;
		return lLenToRewind;
	}
	else
	{
		retval = m_pCur - m_pStart;
		m_pCur = m_pStart;
		return retval;
	}
}

_char
WideCharEntityStream::peek()
{
	assert(m_iStatus != WideCharEntityStream::EWCSTREAMINVALID);
	if (m_pCur < m_pEnd)
	{
		return *m_pCur;
	}
	else
	{
		return L('\0');
	}
}

_char
WideCharEntityStream::get()
{
	assert(m_iStatus != WideCharEntityStream::EWCSTREAMINVALID);
	if (m_pCur < m_pEnd)
	{
		m_pCur++;
		return *(m_pCur - 1);
	}
	else
	{
		m_iStatus = WideCharEntityStream::EWCSTREAMEOF;
		return L('\0');
	}
}

_char *
WideCharEntityStream::getBuffer()
{
	if (m_iStatus != WideCharEntityStream::EWCSTREAMEOF)
	{
		return m_pCur;
	}
	return 0;
}

_char *
WideCharEntityStream::getBufferBase()
{
	return m_pStart;
}

_char *
WideCharEntityStream::getTill(
		const _char stopChar, long &outLen, bool &bFound, bool bAdvance
		)
{
	if (m_iStatus == WideCharEntityStream::EWCSTREAMEOF)
	{
		return 0;
	}

	_char *retLocation = m_pCur;
	long lengthLeft = getRemainingLength();

	for (
			outLen = 0;
			outLen < lengthLeft && (*(m_pCur + outLen) != stopChar);
			outLen++
		);


	if (outLen == lengthLeft)
	{
		bFound = false;
	}
	else
	{
		bFound = true;
		outLen++;
	}

	if (bAdvance)
	{
		m_pCur += outLen;
		if (outLen == lengthLeft)
		{
			m_iStatus = WideCharEntityStream::EWCSTREAMEOF;
		}
	}

	return retLocation;
}

_char *
WideCharEntityStream::getTillFirstOf(
		const _char stopChar_1,
		const _char stopChar_2,
		long &lOutLen,
		_char &wcharFound,
		bool bAdvance
		)
{
	_char *pCur1, *pCur2;
	long lLenTillStopChar1, lLenTillStopChar2;
	bool bFound1, bFound2;
	pCur1 = getTill(stopChar_1, lLenTillStopChar1, bFound1, false);
	pCur2 = getTill(stopChar_2, lLenTillStopChar2, bFound2, false);

	if (bFound1 || bFound2)
	{
		lOutLen = (lLenTillStopChar1 < lLenTillStopChar2) ?
			lLenTillStopChar1 : lLenTillStopChar2;
		wcharFound = *(pCur1 + lOutLen);
	}
	else
	{
		lOutLen = lLenTillStopChar1;
		wcharFound = L('\0');
	}

	if (bAdvance)
	{
		advance(lOutLen);
	}

	return pCur1;
}

bool
WideCharEntityStream::set(const _char c)
{
	assert(m_iStatus != WideCharEntityStream::EWCSTREAMINVALID);
	if (m_pCur < m_pEnd)
	{
		*m_pCur = c;
		return true;
	}
	return false;
}

bool
WideCharEntityStream::setNull()
{
	return set(L('\0'));
}

void
WideCharEntityStream::setPrevEntity(EntityType et)
{
	m_prevEntity = et;
}

bool
WideCharEntityStream::putBack(_char c)
{
	bool retval = set(c);
	if (retval)
	{
		return rewind(1) == 1 ? true : false;
	}
	return false;
}

long
WideCharEntityStream::getRemainingLength()
{
	assert(m_pCur <= m_pEnd);
	return (m_pEnd - m_pCur);
}

long
WideCharEntityStream::getBufferLength()
{
	return m_lBufLength;
}

WideCharEntityStream::EntityType
WideCharEntityStream::getNextEntity(_char **pEntityStr, bool bSetNull)
{
	/*
	 * Omit everything till the first '<' character
	 */
	bool bNextIsElement = false;
	if (m_prevEntity == UNINIT)
	{
		m_prevEntity = ELEMENT;
	}

	if (m_prevEntity != TEXT)
	{
		while (_isspace(*m_pCur) && advance()) {}
	}

	if (m_pCur == m_pEnd)
	{
		m_iStatus = EWCSTREAMEOF;
		m_prevEntity = END;
		return END;
	}

	if ((m_prevEntity == TEXT) || (*m_pCur == L('<')))
	{
		bNextIsElement = true;
		if (L('<') == *m_pCur)
		{
			m_pCur++;
		}
	}

	if (bNextIsElement)
	{
		_char *pFirstGangle = 0;
		bool bFirstGangleFound = false;
		_char quoteChar = 0;
		if (likely(*(m_pCur) != L('!')))
		{
			/* Tag-element */
			bool bInQuote = false;
			bool bNewLine = false;
			_char *pNewLine = 0;
			*pEntityStr = m_pCur;

			while ((m_pCur != m_pEnd) && (L('>') != *m_pCur))
			{
				if (((*m_pCur == L('"')) || (*m_pCur == L('\'')))
						&& (*(m_pCur - 1) != L('\\')))
				{
					bInQuote = !bInQuote;
					quoteChar = *m_pCur;
					while ((m_pCur != m_pEnd) && (*m_pCur != quoteChar))
					{
						if ((L('\\') != *(m_pCur - 1)) && (L('\n') == *m_pCur))
						{
							bNewLine = true;
							pNewLine = m_pCur;
						}
						else if (!bFirstGangleFound && (L('>') == *m_pCur))
						{
							bFirstGangleFound = true;
							pFirstGangle = m_pCur;
						}
						advance();
					}
					if (*m_pCur == quoteChar)
					{
						bInQuote = !bInQuote;
					}
				}
				m_pCur++;
			}
			if (bInQuote)
			{
				if (bFirstGangleFound)
				{
					m_pCur = pFirstGangle;
				}
				else
				{
					while ((m_pCur != m_pEnd) && (L('>') != *m_pCur))
					{
						m_pCur++;
					}
					if ((L('>') != *m_pCur) && bNewLine)
					{
						m_pCur = pNewLine;
					}
					else
					{
						m_prevEntity = END;
						return END;
					}
				}
			}
			if (bSetNull && (m_pCur != m_pEnd))
			{
				*(m_pCur) = L('\0');
			}
			advance();
			if (*pEntityStr == m_pCur - 1)
			{
				m_prevEntity = END;
				return END;
			}
			m_prevEntity = ELEMENT;
			return ELEMENT;
		}
		else
		{
			/* Comment */
			bool bIsPureComment = false;
			*pEntityStr = m_pCur;

			if (L('-') == *(m_pCur + 1))
			{
				bIsPureComment = true;
			}
goto_getOverComment:
			while (*m_pCur && *m_pCur != L('>'))
			{
				m_pCur++;
			}
			if (m_pCur == m_pEnd)
			{
				m_iStatus = EWCSTREAMEOF;
				m_prevEntity = END;
				return END;
			}
			if (bIsPureComment && (_strncmp(m_pCur - 1, L("->"), 2) != 0))
			{
				advance();
				goto goto_getOverComment;
			}
			if (bSetNull && (m_pCur != m_pEnd))
			{
				*m_pCur = L('\0');
			}
			advance();
			m_prevEntity = COMMENT;
			return COMMENT;
		}
	}
	else
	{
		/* Text */
		while (_isspace(*m_pCur))
		{
			m_pCur++;
		}
		if (m_pCur == m_pEnd)
		{
			m_iStatus = EWCSTREAMEOF;
			m_prevEntity = END;
			return END;
		}
		*pEntityStr = m_pCur;
		while ((*m_pCur != L('<')) && advance()) {}
		if (bSetNull)
		{
			if (m_pCur != m_pEnd)
				*(m_pCur) = L('\0');
		}
		advance();
		m_prevEntity = TEXT;
		return TEXT;
	}
	/* Not reached */
}
