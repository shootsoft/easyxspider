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

BEGIN_C_DECLS
#include <assert.h>
END_C_DECLS

const _char *HyXML::TagTable::EMPTY = 0;

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG 1
#include <hypar/Debug.hpp>

typedef pair<const _char*, const HyXML::TagEntry*> nameTagPair;
typedef hash_map<const _char*, const HyXML::TagEntry*,
		hashcase<const _char*>, eqcase>::iterator tableRepIterator;
	
HyXML::TagTable::TagTable()
	:
	m_iStatus(EINIT),
	m_pRootElementName(0)
{
	construct();
}

HyXML::TagTable::TagTable(const TagEntry *pte)
	:
	m_pRootElementName(0)
{
	construct(pte);
}

int
HyXML::TagTable::construct(const TagEntry *pte)
{
	int iEntryCount = 0;
	unsigned int iRootLevel = INT_MAX;

	while (pte[iEntryCount].m_pName != EMPTY)
	{
		nameTagPair p(pte[iEntryCount].m_pName, &pte[iEntryCount]);
		m_TERepository.insert(p);
		if (pte[iEntryCount].m_iContextLevel < iRootLevel)
		{
			iRootLevel = pte[iEntryCount].m_iContextLevel;
			m_pRootElementName = pte[iEntryCount].m_pName;
		}
		iEntryCount++;
	}
	return iEntryCount;
}

const HyXML::TagEntry *
HyXML::TagTable::search(const _char *pName)
{
	if (likely(pName))
	{
		tableRepIterator rx = m_TERepository.find(pName);
		if (rx != m_TERepository.end())
		{
			return rx->second;
		}
	}
	return 0;
}

const _char *
HyXML::TagTable::getRootElement()
{
	return m_pRootElementName;
}
