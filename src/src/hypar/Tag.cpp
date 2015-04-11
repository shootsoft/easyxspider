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
#include <hypar/HyXML.hpp>

#define DEBUG 0
#include <hypar/Debug.hpp>

HyXML::Tag::Tag()
	:
	m_pName(0),
	m_pStrEnd(0),
	m_pTagAttrString(0),
	m_bIsEndTag(false),
	m_bIsSelfClosing(false)
{
}

HyXML::Tag::Tag(const _char *pName)
	:
	m_pName(pName),
	m_pTagAttrString(0),
	m_bIsEndTag(false),
	m_bIsSelfClosing(false)
{
	m_pStrEnd = pName + _strlen(pName);
}

HyXML::Tag::Tag(_char *pTagString, bool bParseAttributes)
	:
	m_pName(pTagString),
	m_pTagAttrString(0),
	m_bIsEndTag(false),
	m_bIsSelfClosing(false)
{
	m_pStrEnd = pTagString + _strlen(pTagString);
	parseTag(&pTagString, &m_pName, this, bParseAttributes);
}

void
HyXML::Tag::reset()
{
	m_pName = m_pTagAttrString = 0;
	m_bIsEndTag = false;
}

int
HyXML::Tag::parseFurther()
{
	if (m_pTagAttrString)
	{
		parseAttributes(m_pTagAttrString, this);
	}
	return m_attrList.size();
}

int
HyXML::Tag::parseTag(_char **pTagString, _char const **pName,
		Tag *pTag, bool bParseAttributes)
{
	_char *pTmpStr = *pTagString;
	bool bIsEndTag = false;
	while ((pTmpStr < pTag->m_pStrEnd) && _isspace(*pTmpStr))
	{
		pTmpStr++;
	}
	
	if (*pTmpStr == L('/'))
	{
		bIsEndTag = true;
		pTmpStr++;
	}

	while ((pTmpStr < pTag->m_pStrEnd) && _isspace(*pTmpStr))
	{
		pTmpStr++;
	}

	if (pName)
	{
		*pName = pTmpStr;
	}
	
	while ((pTmpStr < pTag->m_pStrEnd) && !_isspace(*pTmpStr))
	{
		*pTmpStr = _tolower(*pTmpStr);
		pTmpStr++;
	}

	if (pTmpStr < pTag->m_pStrEnd)
	{
		*pTmpStr = L('\0');
		pTmpStr++;
	}

	*pTagString = pTmpStr;

	if (pTag)
	{
		pTag->m_pName = *pName;
		pTag->m_bIsEndTag = bIsEndTag;

		while ((pTmpStr < pTag->m_pStrEnd) && _isspace(*pTmpStr))
		{
			pTmpStr++;
		}
		if ((pTmpStr < pTag->m_pStrEnd))
		{
			_char *pSelfClosing = pTmpStr;
			while ((pSelfClosing + 1) < pTag->m_pStrEnd)
			{
				pSelfClosing++;
			}
			while (_isspace(*pSelfClosing))
			{
				pSelfClosing--;
			}
			if (L('/') == *pSelfClosing)
			{
				pTag->m_bIsSelfClosing = true;
				*pSelfClosing = L('\0');
			}
			pTag->m_pTagAttrString = pTmpStr;
			if (bParseAttributes)
			{
				parseAttributes(pTmpStr, pTag);
			}
		}
	}
	return 0;
}

void
HyXML::Tag::toString(_string &targetString)
{
	targetString += L("<");
	if (m_bIsEndTag)
	{
		targetString += L("/");
	}
	if (m_pName)
	{
		targetString += m_pName;
	}

	if (m_attrList.size() > 0)
	{
		TagAttrList::iterator avx = m_attrList.begin();
		while (avx != m_attrList.end())
		{
			targetString += L(" ");
			targetString += (*avx).first;
			targetString += L("=\"");
			targetString += (*avx).second;
			targetString += L("\"");
			avx++;
		}
	}
	else if (m_pTagAttrString)
	{
		targetString += L(' ');
		targetString += m_pTagAttrString;
	}
	if (m_bIsSelfClosing && !m_bIsEndTag)
	{
		targetString += L("/>");
	}
	else
	{
		targetString += L('>');
	}
}

inline int
HyXML::Tag::getQuotedName(_char *&pStr, _char **pName, bool bSetNull)
{
	while ((pStr < m_pStrEnd) && _isspace(*pStr))
	{
		pStr++;
	}
	if (pStr >= m_pStrEnd)
	{
		return -1;
	}
	if (L('"') == *pStr || L('\'') == *pStr)
	{
		_char wideQuote = *pStr;
		pStr++;
		if (pStr >= m_pStrEnd)
			return -1;
		*pName = pStr;
		while ((pStr < m_pStrEnd) && *pStr != wideQuote)
		{
			pStr++;
		}
		if (pStr >= m_pStrEnd)
			return -1;
		if (*pStr != L('='))
		{
			return 1;
		}
		if (bSetNull)
		{
			*pStr = L('\0');
			pStr++;
		}
	}
	else
	{
		*pName = pStr;
		while ((pStr < m_pStrEnd) && (*pStr != L('=')) && !_isspace(*pStr))
		{
			pStr++;
		}
		if (pStr >= m_pStrEnd)
			return -1;
		while ((pStr < m_pStrEnd) && _isspace(*pStr))
		{
			pStr++;
		}
		if (pStr >= m_pStrEnd)
			return -1;
		if (*pStr != L('='))
		{
			pStr++;
			return 1;
		}
		if (bSetNull)
		{
			*pStr = L('\0');
			pStr++;
		}
	}
	while ((pStr < m_pStrEnd) && _isspace(*pStr))
	{
		pStr++;
	}
	return 0;
}

inline int
HyXML::Tag::getQuotedValue(_char *&pStr, _char **pValue, bool bSetNull)
{
	while ((pStr < m_pStrEnd) && _isspace(*pStr))
	{
		pStr++;
	}
	if (pStr >= m_pStrEnd)
	{
		return -1;
	}
	if (L('"') == *pStr || L('\'') == *pStr)
	{
		_char wideQuote = *pStr;
		pStr++;
		if (pStr >= m_pStrEnd)
			return -1;
		*pValue = pStr;
		while ((pStr < m_pStrEnd) && *pStr != wideQuote)
		{
			pStr++;
		}
		if (bSetNull)
		{
			*pStr = L('\0');
		}
		pStr++;
		return 0;
	}
	else
	{
		*pValue = pStr;
		while ((pStr < m_pStrEnd) && !_isspace(*pStr))
		{
			pStr++;
		}
		if (bSetNull)
		{
			*pStr = L('\0');
		}
		pStr++;
		return 0;
	}
}

int
HyXML::Tag::parseAttributes(_char *&pTagAttrString, Tag *pTag)
{
	int retval = -1;
	_char *pName, *pValue;
	int propCount = 0;
	while (1)
	{
		retval = getQuotedName(pTagAttrString, &pName, true);
		if (retval < 0)
		{
			break;
		}
		else if (1 == retval)
		{
			continue;
		}
		if (getQuotedValue(pTagAttrString, &pValue, true) < 0)
		{
			break;
		}
		pair<_char *, _char *> p(pName, pValue);
		pTag->m_attrList.push_back(p);
		propCount++;
	}
	return propCount;
}

const _char *
HyXML::Tag::findProperty(const _char *pName)
{
	TagAttrList::iterator hx = m_attrList.begin();
	while (hx != m_attrList.end())
	{
		if (_strcasecmp((*hx).first, pName) == 0)
		{
			return (*hx).second;
		}
		hx++;
	}
	return 0;
}
