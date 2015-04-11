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
#include <hypar/TextUtils.hpp>
#include <hypar/HyXML.hpp>

#define DEBUG 0
#include <hypar/Debug.hpp>

#include <iostream>

BEGIN_C_DECLS
#include <assert.h>
END_C_DECLS

using namespace std;

#define SCREENEND 108
#define SCREENENDSEEKSTART 80


HyXML::DOMNode::DOMNode(
		HyXML::DOMNode::NodeType nodeType,
		const _char *pStr
		) :
	m_pPropertyMap(0),
	m_pParent(0),
	m_pPrev(0),
	m_pNext(0),
	m_pChild(0),
	m_pLast(0),
	m_type(nodeType),
	m_pName(0),
	m_pContent(0),
	m_bSelfClosing(true),
	m_iLevel(0),
	m_iHorLevel(0)
{
	switch (m_type)
	{
		case ELEMENT:
			m_pName = pStr;
			break;
		case TEXT:
		case COMMENT:
			m_pContent = pStr;
			break;
		default:
			break;
	}
}

HyXML::DOMNode::~DOMNode()
{
	DOMNode *pTmpNode = m_pChild;
	
	while (m_pChild)
	{
		pTmpNode = m_pChild;
		m_pChild = m_pChild->m_pNext;
		delete pTmpNode;
	}

	if (m_pPropertyMap)
	{
		delete m_pPropertyMap;
		m_pPropertyMap = 0;
	}
}

HyXML::DOMNode *
HyXML::DOMNode::clone(bool bCopyProperties)
{
	DOMNode *pRetNode = new DOMNode(m_type, m_pName);
	assert(pRetNode);
	if (bCopyProperties && m_pPropertyMap)
	{
		PropertyMap::iterator pmx = m_pPropertyMap->begin();
		while (pmx != m_pPropertyMap->end())
		{
			pRetNode->addProperty((*pmx).first, (*pmx).second);
			pmx++;
		}
	}
	return pRetNode;
}

HyXML::DOMNode *
HyXML::DOMNode::clone(NodeType nodeType, const _char *pName,
		bool bCopyProperties)
{
	DOMNode *pRetNode = new DOMNode(nodeType, pName);
	assert(pRetNode);
	if (bCopyProperties && m_pPropertyMap)
	{
		PropertyMap::iterator pmx = m_pPropertyMap->begin();
		while (pmx != m_pPropertyMap->end())
		{
			pRetNode->addProperty((*pmx).first, (*pmx).second);
			pmx++;
		}
	}
	return pRetNode;
}

int
HyXML::DOMNode::detach()
{
	if (!m_pParent && !m_pPrev && !m_pNext)
		return -1;
	if (m_pPrev)
	{
		m_pPrev->m_pNext = m_pNext;
	}
	else
	{
		if (m_pParent)
		{
			m_pParent->m_pChild = m_pNext;
		}
	}
	if (m_pNext)
	{
		m_pNext->m_pPrev = m_pPrev;
	}
	else
	{
		if (m_pParent)
		{
			m_pParent->m_pLast = 0;
		}
	}
	m_pParent = 0;
	m_pPrev = 0;
	m_pNext = 0;
	return 0;
}

int
HyXML::DOMNode::attachAsChild(DOMNode *pNode)
{
	if (!pNode)
		return -1;
	if (!m_pChild)
	{
		m_pChild = m_pLast = pNode;
		pNode->m_pParent = this;
		pNode->m_pPrev = pNode->m_pNext = 0;
		pNode->initLevel(m_iLevel + 1, true);
		return 1;
	}
	else
	{
		int retval = m_pLast->attachAsNext(pNode);
		assert(m_pLast == pNode);
		return retval;
	}
}

int
HyXML::DOMNode::insertAsParent(DOMNode *pNode)
{
	int retval = -1;
	if (!pNode)
	{
		return -1;
	}
	if (m_pPrev)
	{
		m_pPrev->m_pNext = pNode;
	}
	if (m_pNext)
	{
		m_pNext->m_pPrev = pNode;
	}
	if (m_pParent)
	{
		if (this == m_pParent->m_pChild)
		{
			m_pParent->m_pChild = pNode;
		}
		if (this == m_pParent->m_pLast)
		{
			m_pParent->m_pLast = pNode;
		}
	}
	m_pPrev = m_pNext = m_pParent = 0;
	retval = pNode->attachAsChild(this);
	return retval;
}

int
HyXML::DOMNode::insertAsChild(DOMNode *pNewChild)
{
	DOMNode *pChild = m_pChild;
	while (pChild)
	{
		pChild->m_pParent = pNewChild;
		pChild = pChild->m_pNext;
	}
	pNewChild->m_pChild = m_pChild;
	pNewChild->m_pLast = m_pLast;
	m_pChild = pNewChild;
	m_pLast = pNewChild;
	m_pChild->initLevel(m_iLevel + 1, true);
	return 1;
}

int
HyXML::DOMNode::attachAsPrevious(DOMNode *pNode)
{
	if (!pNode)
		return -1;
	pNode->m_pNext = this;
	pNode->m_pPrev = m_pPrev;
	pNode->m_pParent = m_pParent;
	if (m_pPrev)
	{
		m_pPrev->m_pNext = pNode;
	}
	else
	{
		if (m_pParent)
		{
			m_pParent->m_pChild = pNode;
		}
	}
	m_pPrev = pNode;
	pNode->initLevel(m_iLevel, false);
	return 0;
}

int
HyXML::DOMNode::attachAsNext(DOMNode *pNode)
{
	if (!pNode)
		return -1;
	pNode->m_pNext = m_pNext;
	pNode->m_pPrev = this;
	pNode->m_pParent = m_pParent;
	if (m_pNext)
	{
		m_pNext->m_pPrev = pNode;
	}
	else
	{
		if (m_pParent)
		{
			m_pParent->m_pLast = pNode;
		}
	}
	m_pNext = pNode;
	pNode->initLevel(m_iLevel, false);
	return 0;
}

int
HyXML::DOMNode::copyAttributes(HyXML::Tag *tag)
{
	int retval = 0;
	wdebug2("Copying attributes for", tag->m_pName);
	if (!m_pPropertyMap)
	{
		m_pPropertyMap = new PropertyMap;
		m_pPropertyMap->clear();
		assert(m_pPropertyMap);
	}
	TagAttrList::const_iterator avx = tag->m_attrList.begin();
	while (avx != tag->m_attrList.end())
	{
		pair<_char *, _char *> p(avx->first, avx->second);
		(*m_pPropertyMap).insert(p);
		avx++;
		retval++;
	}
	return retval;
}

void
HyXML::DOMNode::reset()
{
	if (m_pPropertyMap)
	{
		m_pPropertyMap->clear();
	}
	m_pParent = 0;
	m_pPrev = 0;
	m_pNext = 0;
	m_pChild = 0;
	m_pLast = 0;
	m_type = END;
	m_pName = 0;
	m_pContent = 0;
	m_iLevel = 0;
	m_iHorLevel = 0;
}

int
HyXML::DOMNode::initLevel(int iLevel, bool bNext)
{
	m_iLevel = iLevel;
	if (m_pChild)
	{
		m_pChild->initLevel(m_iLevel + 1, true);
	}
	if (bNext && m_pNext)
	{
		m_pNext->initLevel(m_iLevel, true);
	}
	return 0;
}

int
HyXML::DOMNode::addProperty(const _char *pName, const _char *pValue)
{
	if (!m_pPropertyMap)
	{
		m_pPropertyMap = new PropertyMap;
		m_pPropertyMap->clear();
		if (!m_pPropertyMap)
			return -1;
	}
	(*m_pPropertyMap)[pName] = pValue;
	return 0;
}

int
HyXML::DOMNode::addProperty(pair<const _char *, const _char *> &p)
{
	if (!m_pPropertyMap)
	{
		m_pPropertyMap = new PropertyMap;
		m_pPropertyMap->clear();
		if (!m_pPropertyMap)
			return -1;
	}
	if ((m_pPropertyMap->insert(p)).second == true)
	{
		return 0;
	}
	return -1;
}

const _char *
HyXML::DOMNode::findProperty(const _char *pName)
{
	if (m_pPropertyMap)
	{
		PropertyMap::iterator px;
		px = m_pPropertyMap->find(pName);
		if (px != m_pPropertyMap->end())
		{
			return (*px).second;
		}
	}
	return 0;
}

void
HyXML::DOMNode::toString(_string &targetString, bool bChildOnly)
{
	switch (m_type)
	{
		case ELEMENT:
			targetString += TextUtils::echoSpaces(m_iLevel);
			targetString += L("<");
			targetString += m_pName;
			/* TODO - Add the attributes */
			if (m_pPropertyMap)
			{
				PropertyMap::iterator pmx = m_pPropertyMap->begin();
				while (pmx != m_pPropertyMap->end())
				{
					wdebug3(L("More properties for"), m_pName, ++i);
					wdebug2(pmx->first, pmx->second);
					targetString += L(" ");
					targetString += pmx->first;
					targetString += L("=\"");
					targetString += pmx->second;
					targetString += L("\"");
					pmx++;
				}
			}
#if DEBUG
			targetString += L(" level=")");
			targetString += itows(m_iLevel);
			targetString += L("\" horlevel=\"");
			targetString += itows(m_iHorLevel);
			targetString += L("\"");
#endif
			if (m_pChild)
			{
				targetString += L(">\n");
				m_pChild->toString(targetString);
			}
			else
			{
				if (m_bSelfClosing)
				{
					targetString += L("/>\n");
				}
				else
				{
					targetString += L("></");
					targetString += m_pName;
					targetString += L(">\n");
				}
				break;
			}
			targetString += TextUtils::echoSpaces(m_iLevel);
			targetString += L("</");
			targetString += m_pName;
			targetString += L(">\n");
			break;
		case TEXT:
			targetString += m_pContent;
			targetString += L("\n");
			break;
		case COMMENT:
			targetString += L("<");
			targetString += m_pContent;
			targetString += L(">\n");
			break;
		default:
			break;
	}
	if (!bChildOnly && m_pNext)
	{
		m_pNext->toString(targetString);
	}
}

