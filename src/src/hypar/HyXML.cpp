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
#include <hypar/WideCharEntityStream.hpp>

BEGIN_C_DECLS
#include <assert.h>
END_C_DECLS

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG 1
#include <hypar/Debug.hpp>

#define HYXML_ROOT_NODE L("HyXML_Root")

HyXML::HyXML(const TagEntry *pte)
	:
	m_tagTable(pte),
	m_bDocStarted(false),
	m_bIgnoreUnknownTag(false),
	m_pCurTagEntry(0),
	m_pCloneableNode(0),
	m_pRootNode(0),
	m_pCurrentNode(0),
	m_pCurrentParentNode(0),
	tagCb(0),
	textCb(0),
	commentCb(0)
{
	m_unknownTagEntry.m_pRparent = m_tagTable.getRootElement();
	m_unknownTagEntry.m_pFirstRparent = m_unknownTagEntry.m_pRparent;
	m_unknownTagEntry.m_bClosure = true;
	m_occurenceMapIter = m_occurenceMap.end();
}

void
HyXML::reset()
{
	m_pCloneableNode = 0;
	m_pRootNode = 0;
	m_pCurrentNode = 0;
	m_pCurrentParentNode = 0;
}

int HyXML::initDOM(Tag *tag)
{
	if (!m_pCloneableNode)
		return -1;
	if (tag)
	{
		m_pRootNode = m_pCloneableNode->
			clone(DOMNode::ELEMENT, tag->m_pName);
	}
	else
	{
		m_pRootNode = m_pCloneableNode->
			clone(DOMNode::ELEMENT, HYXML_ROOT_NODE);
	}
	m_pRootNode->m_pParent = m_pCloneableNode;
	if (!m_pCloneableNode->m_pChild)
	{
		m_pCloneableNode->m_pChild = m_pRootNode;
	}
	else
	{
		DOMNode *pTempNode = m_pCloneableNode->m_pLast;
		m_pRootNode->m_pPrev = pTempNode;
		pTempNode->m_pNext = m_pRootNode;
	}
	m_pCloneableNode->m_pLast = m_pRootNode;
	m_pCurrentParentNode = m_pRootNode;
	if (tag)
		m_pCurrentParentNode->copyAttributes(tag);
	m_pCurrentNode = 0;
	return 0;
}

int
HyXML::addNode(Tag *t, bool bSelfClosing)
{
	addNode(t->m_pName, bSelfClosing);
	m_pCurrentParentNode->copyAttributes(t);
	return 0;
}

int
HyXML::addNode(const _char *pElementName, bool bSelfClosing)
{
	DOMNode *pNode = m_pCloneableNode->clone(DOMNode::ELEMENT, pElementName);
	pNode->m_bSelfClosing = bSelfClosing;
	if (m_pCurrentNode)
	{
		m_pCurrentNode->m_pNext = pNode;
		pNode->m_pPrev = m_pCurrentNode;
		pNode->m_iHorLevel = m_pCurrentNode->m_iHorLevel + 1;
		m_pCurrentNode = 0;
	}
	else
	{
		m_pCurrentParentNode->m_pChild = pNode;
	}
	pNode->m_pParent = m_pCurrentParentNode;
	m_pCurrentParentNode->m_pLast = pNode;
	pNode->m_iLevel = m_pCurrentParentNode->m_iLevel + 1;
	m_pCurrentParentNode = pNode;
	return 0;
}

int
HyXML::addNodeSelfContained(DOMNode::NodeType nodeType, const _char *pContent)
{
	DOMNode *pNode = m_pCloneableNode->clone(nodeType, pContent);
	if (m_pCurrentNode)
	{
		m_pCurrentNode->m_pNext = pNode;
		pNode->m_pPrev = m_pCurrentNode;
	}
	else
	{
		m_pCurrentParentNode->m_pChild = pNode;
	}
	pNode->m_pParent = m_pCurrentParentNode;
	pNode->m_iLevel = m_pCurrentParentNode->m_iLevel + 1;
	m_pCurrentParentNode->m_pLast = pNode;
	m_pCurrentNode = pNode;
	return 0;
}

int
HyXML::addNodeSelfContained(Tag *t)
{
	addNodeSelfContained(DOMNode::ELEMENT, t->m_pName);
	m_pCurrentNode->copyAttributes(t);
	return 0;
}

int
HyXML::closeNode()
{
	if (m_pCurrentParentNode == m_pRootNode)
	{
		debug("Tried closing more nodes than you opened!");
		return -1;
	}
	m_pCurrentNode = m_pCurrentParentNode;
	m_pCurrentParentNode = m_pCurrentParentNode->m_pParent;
	return 0;
}

int
HyXML::closeNode(unsigned int iNumTimes)
{
	unsigned int i = 0;
	for (; i < iNumTimes; i++)
	{
		if (closeNode() < 0)
		{
			return -1;
		}
	}
	return i;
}

int
HyXML::handleIgnoreChildren(Tag &tag, WideCharEntityStream &wideCharStream)
{
	long outLen = 0;
	bool bFound = false;
	_char *pEntity = 0;
	_char *pEnd = 0;
	_char peekedChar;
	int iMoreAdvances = 0; // takes care of futher advance() calls for
	                       // re-setting pEnd
goto_HICFurther:
	if (!pEntity)
	{
		pEntity = wideCharStream.getTill(LANGLE, outLen, bFound, false);
		pEnd = pEntity + outLen - 1;
	}
	else
	{
		wideCharStream.getTill(LANGLE, outLen, bFound, false);
		pEnd += outLen + iMoreAdvances;
	}
	if (bFound)
	{
		iMoreAdvances = 0;
		wideCharStream.advance(outLen);
		while ((peekedChar = wideCharStream.peek()) && _isspace(peekedChar))
		{
			wideCharStream.advance();
			iMoreAdvances++;
		}
		if ((peekedChar = wideCharStream.peek()) == SLASH)
		{
			wideCharStream.get();
			iMoreAdvances++;
			while (_isspace(wideCharStream.peek()))
			{
				wideCharStream.get();
				iMoreAdvances++;
			}
			if (_strncasecmp(tag.m_pName, wideCharStream.getBuffer(),
						_strlen(tag.m_pName)) == 0)
			{
				*pEnd = NULLCHAR;
				addNode(&tag);
				handleText(pEntity);
				closeNode();
				wideCharStream.getTill(GANGLE, outLen, bFound, true);
				wideCharStream.setPrevEntity(WideCharEntityStream::ELEMENT);
			}
			else
			{
				if (!peekedChar)
					return 0;
				goto goto_HICFurther;
			}
		}
		else
		{
			if (!peekedChar)
				return 0;
			goto goto_HICFurther;
		}
	}
	return 1;
}

/**
 * The 'main' routine. Note: pTextBuffer should be editable!
 */

HyXML::DOMNode *
HyXML::parse(_char *pTextBuffer, DOMNode *pCloneableNode)
{
	/* Define/declare needed variables */
	_char *pEntity;
	bool bDocStarted = false;

	/* Reset all variables */
	reset();
	m_entityStack.clear();
	m_occurenceMap.clear();
	WideCharEntityStream wideCharStream(pTextBuffer, _strlen(pTextBuffer));
	WideCharEntityStream::EntityType entityType;
	m_pCloneableNode = pCloneableNode;

	/* Now, parse the input and create the tree */
	initDOM();

	while ((entityType = wideCharStream.getNextEntity(&pEntity, true))
			!= WideCharEntityStream::END)
	{
		switch (entityType)
		{
			case WideCharEntityStream::ELEMENT:
				{
					EntityCbRetval retval = HyXML::OK;
					Tag tag(pEntity, true);
					if (tagCb)
					{
						retval = tagCb(&tag);
						switch (retval)
						{
							case HyXML::IGNORE_SIMPLE:
								continue;
							case HyXML::IGNORE_CHILDREN:
								handleIgnoreChildren(tag, wideCharStream);
								break;
							case HyXML::OK:
								if (handleElement(&tag, bDocStarted) == 0)
								{
									bDocStarted = true;
								}
							default:
								break;
						}
					}
				}
				break;
			case WideCharEntityStream::TEXT:
				{
					if (textCb && !textCb(pEntity))
					{
						break;
					}
					if (unlikely(!bDocStarted))
					{
						break;
					}
					handleText(pEntity);
				}
				break;
			case WideCharEntityStream::COMMENT:
				{
					if (commentCb && !commentCb(pEntity))
					{
						break;
					}
					handleComment(pEntity);
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

inline int
HyXML::handleElement(Tag *tag, bool bDocStarted)
{
	m_pCurTagEntry = m_tagTable.search(tag->m_pName);
	assert(tag);
	if (!m_pCurTagEntry)
	{
		if (m_bIgnoreUnknownTag)
		{
			return -1;
		}
		m_unknownTagEntry.m_pName = tag->m_pName;
		m_pCurTagEntry = &m_unknownTagEntry;
	}
	if (unlikely(!bDocStarted))
	{
		if (tag->m_bIsEndTag)
		{
			return -1;
		}
		if (_strcasecmp(tag->m_pName, m_tagTable.getRootElement()) != 0)
		{
			Tag t(m_tagTable.getRootElement());
			addNode(&t, m_pCurTagEntry->m_bClosure);
			m_occurenceMap[m_tagTable.getRootElement()] = true;
			m_entityStack.push(m_tagTable.getRootElement());
		}
		else
		{
			addNode(tag);
			m_occurenceMap[m_tagTable.getRootElement()] = true;
			m_entityStack.push(tag->m_pName);
			return 0;
		}
	}
	/* Opening tag or closing tag? */
	if (tag->m_bIsEndTag)
	{
		/* Is it supposed to end? */
		if (!m_pCurTagEntry->m_bClosure)
		{
			return -1;
		}
		int i = m_entityStack.locateFromTop(tag->m_pName);
		/* Pop everything */
		while (i >= 0)
		{
#ifdef DEBUG
			if (i != 0)
				m_pCurrentParentNode->addProperty(L("forceclosed"), L("10"));
#endif
			closeNode();
			m_entityStack.pop();
			i--;
		}
	}
	else
	{
		if (!m_pCurTagEntry->m_bClosure)
		{
			addNodeSelfContained(tag);
			tag->m_bIsEndTag = true;
		}
		else
		{
			int i;
			i = m_entityStack.locateFromTop(tag->m_pName);
			m_occurenceMapIter = m_occurenceMap.find(tag->m_pName);
			if (m_pCurTagEntry->m_bOccurOnce &&
					(m_occurenceMapIter != m_occurenceMap.end()) &&
					(true == m_occurenceMapIter->second))
			{
				return -1;
			}
			clearCurrentContext(tag, m_pCurTagEntry, false);
			correctStack(tag, m_pCurTagEntry, false);
			addNode(tag);
#ifdef DEBUG
			if (m_pCurTagEntry == &m_unknownTagEntry)
			{
				m_pCurrentParentNode->addProperty(L("unknowntag"), L("1"));
			}
#endif
			m_occurenceMap[tag->m_pName] = true;
			m_entityStack.push(tag->m_pName);
		}
	}
	return 0;
}

inline int
HyXML::handleText(_char *pText)
{
	addNodeSelfContained(DOMNode::TEXT, pText);
	return 0;
}

inline int
HyXML::handleComment(_char *pComment)
{
	addNodeSelfContained(DOMNode::COMMENT, pComment);
	return 0;
}

int
HyXML::correctStack(Tag *t, const TagEntry *pTagEntry, bool bCheckOnly)
{
	int iCorrections = 0;
	const _char *pAncestor = 0;
	const vector<const _char *> *pAncestorList = 0;
	int iDummy = -1, iLocationFromTop = -1;
	if ((pAncestor = pTagEntry->getRparent()) != 0)
	{
		pAncestorList = pTagEntry->getRparentList();
	}
	else
	{
		if (!pTagEntry->m_pParent)
		{
			/* This could be the root element! */
			return iCorrections;
		}
		pAncestor = pTagEntry->getParent();
		pAncestorList = pTagEntry->getParentList();
	}
	if (pAncestorList)
	{
		iLocationFromTop = m_entityStack.
			locateAnyFromTop(pAncestorList, iDummy);
	}
	else
	{
		iLocationFromTop = m_entityStack.
			locateFromTop(pAncestor);
	}
	if (iLocationFromTop < 0)
	{
		if (bCheckOnly)
			return 1;
		Tag tag(pAncestor);
		const TagEntry *pt = m_tagTable.search(tag.m_pName);
		if (pt)
		{
			iCorrections +=
				correctStack(&tag, pt, bCheckOnly);
		}
		addNode(&tag);
		m_entityStack.push(tag.m_pName);
		iCorrections++;
	}
	return iCorrections;
}

int
HyXML::clearCurrentContext(Tag *t, const TagEntry *pTagEntry, bool bCheckOnly)
{
	int retval = 0;
	const TagEntry *pte;
	if ((pTagEntry->m_iContextSwitch == 1) && (!pTagEntry->m_bOccurOnce) &&
			(m_entityStack.locateFromTop(t->m_pName) > 0))
	{
		while (m_entityStack.size() > 0)
		{
			pte = m_tagTable.search(m_entityStack.top());
			if (pte)
			{
				if (pte->m_iContextLevel < TEXT_ATTR)
				{
					break;
				}
			}
			if (bCheckOnly)
			{
				return 1;
			}
#ifdef DEBUG
			m_pCurrentParentNode->addProperty(L("forceclosed"), L("1"));
#endif
			closeNode(); m_entityStack.pop(); retval++;
		}
	}
	pte = m_tagTable.search(m_entityStack.top());
	assert(!_strcasecmp(m_entityStack.top(), m_pCurrentParentNode->m_pName));
	while (pte && (m_entityStack.size() > 0) &&
			(pte->m_iContextLevel >= pTagEntry->m_iContextLevel) &&
			(pte->m_iContextSwitch != 2))
	{
		if ((pte->m_iContextLevel == pTagEntry->m_iContextLevel) &&
				(pte != pTagEntry))
		{
			break;
		}
		if (bCheckOnly)
		{
			return 1;
		}
#ifdef DEBUG
		m_pCurrentParentNode->addProperty(L("forceclosed"), L("2"));
#endif
		closeNode(); m_entityStack.pop(); retval++;
		if (pte == pTagEntry)
		{
			break;
		}
		pte = m_tagTable.search(m_entityStack.top());
	}
	return retval;
}
