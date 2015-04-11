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

BEGIN_C_DECLS
#include <assert.h>
END_C_DECLS

#include <hypar/HashUtils.hpp>
#include <hypar/HyXML.hpp>

#define DEBUG 0
#include <hypar/Debug.hpp>

int
parseAndStoreElementNamesFromCommaDelimitedString(
		_char *pString, vector<const _char *> *pStorage
		)
{
	bool bFinished = false;
	_char *pStart;
	int iNumElements = 0;
	while (!bFinished)
	{
		while (*pString && (_isspace(*pString) || (L(',') == *pString)))
		{
			pString++;
		}
		pStart = pString;
		while (*pString && (L(',') != *pString) && !_isspace(*pString))
		{
			pString++;
		}
		if (L(',') == *pString)
		{
			*pString = L('\0');
			pString++;
		}
		else
		{
			*pString = L('\0');
			while (*pString && (L(',') != *pString))
			{
				pString++;
			}
		}
		pStorage->push_back(pStart);
		iNumElements++;
		if (!*pString)
		{
			bFinished = true;
		}
	}
	return iNumElements;
}

HyXML::TagEntry::TagEntry()
	:
	m_pName(0),
	m_iContextSwitch(1),
	m_iContextLevel(TEXT_ATTR),
	m_bClosure(true),
	m_bOccurOnce(false),
	m_pParent(0),
	m_pRparent(0),
	m_pFirstParent(0),
	m_pFirstRparent(0),
	m_pParentNew(0),
	m_pRparentNew(0),
	m_pParentList(0),
	m_pRparentList(0)
{
}

HyXML::TagEntry::TagEntry(
		const _char *pName, unsigned int iContextSwitch,
		unsigned int iContextLevel, bool bClosure,
		bool bOccurOnce, const _char *pParent, const _char *pRparent
		) :
	m_pName(pName),
	m_iContextSwitch(iContextSwitch),
	m_iContextLevel(iContextLevel),
	m_bClosure(bClosure),
	m_bOccurOnce(bOccurOnce),
	m_pParent(pParent),
	m_pRparent(pRparent),
	m_pFirstParent(0),
	m_pFirstRparent(0),
	m_pParentNew(0),
	m_pRparentNew(0),
	m_pParentList(0),
	m_pRparentList(0)
{
	_char *tmpStrPtr;

	if (m_pParent != TagTable::EMPTY)
	{
		tmpStrPtr = _strrchr(m_pParent, ',');
		if (!tmpStrPtr)
		{
			m_pFirstParent = m_pParent;
		}
		else
		{
			m_pParentNew = _strdup(m_pParent);
			assert(m_pParentNew);
			tmpStrPtr = m_pParentNew;
			while (*tmpStrPtr)
			{
				*tmpStrPtr = _tolower(*tmpStrPtr);
				tmpStrPtr++;
			}
			m_pParentList = new vector<const _char *>;
			parseAndStoreElementNamesFromCommaDelimitedString(m_pParentNew,
					m_pParentList);
			m_pFirstParent = m_pParentList->at(0);
		}
	}

	if (m_pRparent != TagTable::EMPTY)
	{
		tmpStrPtr = _strrchr(m_pRparent, ',');
		if (!tmpStrPtr)
		{
			m_pFirstRparent = m_pRparent;
		}
		else
		{
			m_pRparentNew = _strdup(m_pRparent);
			assert(m_pRparentNew);
			tmpStrPtr = m_pRparentNew;
			while (*tmpStrPtr)
			{
				*tmpStrPtr = _tolower(*tmpStrPtr);
				tmpStrPtr++;
			}
			m_pRparentList = new vector<const _char *>;
			parseAndStoreElementNamesFromCommaDelimitedString(m_pRparentNew,
					m_pRparentList);
			m_pFirstRparent = m_pRparentList->at(0);
		}
	}
}

HyXML::TagEntry::~TagEntry()
{
	if (m_pParentNew)
	{
		delete m_pParentNew;
		m_pParentNew = 0;
		delete m_pParentList;
		m_pParentList = 0;
	}
	if (m_pRparentNew)
	{
		delete m_pRparentNew;
		m_pRparentNew = 0;
		delete m_pRparentList;
		m_pRparentList = 0;
	}
}

inline bool
HyXML::TagEntry::findSubstr(
		const _char *needle,
		const _char *haystack
		) const
{
	if (_strstr(needle, haystack))
	{
		return true;
	}
	return false;
}

inline int
HyXML::TagEntry::findInList(
		const _char *needle,
		vector<const _char *> *haystack
		) const
{
	int sz = haystack->size();
	int i = 0;
	while (i < sz)
	{
		if (_strcasecmp(haystack->at(i), needle) == 0)
		{
			return i;
		}
		i++;
	}
	return -1;
}

const _char *
HyXML::TagEntry::getName()
{
	return m_pName;
}

const _char *
HyXML::TagEntry::getParent() const
{
	return m_pFirstParent;
}

const _char *
HyXML::TagEntry::getRparent() const
{
	return m_pFirstRparent;
}

const vector<const _char *> *
HyXML::TagEntry::getParentList() const
{
	return m_pParentList;
}

const vector<const _char *> *
HyXML::TagEntry::getRparentList() const
{
	return m_pRparentList;
}

bool
HyXML::TagEntry::isParent(const _char *pParent)
{
	if (m_pParentNew)
	{
		return findInList(pParent, m_pParentList) > -1 ?
			true : false;
	}
	return _strcasecmp(pParent, m_pParent) == 0 ? true : false;
}

bool
HyXML::TagEntry::isRparent(const _char *pRparent)
{
	if (m_pRparentNew)
	{
		return findInList(pRparent, m_pRparentList) > -1 ?
			true : false;
	}
	return _strcasecmp(pRparent, m_pRparent) == 0 ? true : false;
}
