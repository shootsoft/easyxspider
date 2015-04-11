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

#ifndef _HYPAR_HYXML_H_
#define _HYPAR_HYXML_H_

#include <hypar/localdefs.h>
#include <hypar/WideCharEntityStream.hpp>

#include <string>
#include <map>
#if __GNUC__ < 3
#include <hash_map>
#include <hash_set>
#else
#include <ext/hash_map>
#include <ext/hash_set>
#endif
#include <deque>
#include <list>

#include <hypar/HashUtils.hpp>

#if __GNUC__ > 2
using namespace __gnu_cxx;
#endif

typedef hash_map<const _char *, bool, hashcase<const _char *>,
		eqcase> OccurenceMap;
typedef hash_map<const _char *, const _char *,
	                    hashcase<const _char *>, eqcase> PropertyMap;
typedef list<pair<_char *, _char *> > TagAttrList;

//typedef map<const _char *, const _char *, eqcase> PropertyMap;

/**
 * class HyXML : The main class of HyParSuite
 *
 * This is the main class, and the 'main' function to use is 'parse'
 * Defines all the other necessary classes
 */
class HyXML
{

	public:
		/**
		 * enum EntityCbRetval - Return value of 'entity callback functions'
		 */
		typedef enum
		{
			IGNORE_SIMPLE = -100,
			IGNORE_CHILDREN,
			OK = 0
		} EntityCbRetval;

	protected:
		enum
		{
			TEXT_ATTR = 1000
		};

	public:
		/**
		 * TagEntry - All information about a valid 'Tag'
		 *
		 * For each valid tag, the name, information about where it can occur
		 * in the DOM structure, whether it can occur only once, its parent as
		 * well as 'remote' parents (either one) etc.
		 */
		class TagEntry
		{
			public:
				const _char *m_pName;
				unsigned int m_iContextSwitch;
				unsigned int m_iContextLevel;
				bool m_bClosure;
				bool m_bOccurOnce;

			public:
				const _char *m_pParent;
				const _char *m_pRparent;
				const _char *m_pFirstParent;
				const _char *m_pFirstRparent;

			private:
				_char *m_pParentNew;
				_char *m_pRparentNew;
				vector<const _char *> *m_pParentList, *m_pRparentList;

			private:
				inline bool findSubstr(const _char *needle,
						const _char *haystack) const;
				inline int findInList(const _char *needle,
						vector<const _char *> *haystack) const;

			public:
				TagEntry();
				TagEntry(const _char *pName, unsigned int iContextSwitch,
						unsigned int iContextLevel, bool bClosure,
						bool bOccurOnce, const _char *pParent,
						const _char *pRparent);
				~TagEntry();

			public:
				const _char *getName();
				bool isParent(const _char *pParent);
				bool isRparent(const _char *pRparent);
				const _char *getParent() const;
				const _char *getRparent() const;
				const vector<const _char *> *getParentList() const;
				const vector<const _char *> *getRparentList() const;
		};

	public:
		/**
		 * TagTable - The table which contains all the valid Tags and rules
		 *
		 * This contains all valid tags for a particular instance of HyXML. It
		 * is initialised using various TagEntry-objects, and combined
		 * together, these TagEntry-s define the rules which any document that
		 * HyXML parses is made to follow
		 */
		class TagTable
		{
			public:
				typedef enum
				{
					EINIT,
					OK
				}
				Status;

			public:
				static const _char *EMPTY;
				static const TagEntry m_defaultHTMLTable[];

			public:
				TagTable();
				/**
				 * Constructor which takes an array of TagEntry-s for
				 * constructing the internal rule-table
				 *
				 * By default, a pre-defined HTML rules-table is used if
				 * nothing is supplied.
				 */
				TagTable(const TagEntry *pte);
				/**
				 * If no table is supplied in the constructor, call this with
				 * another array of TagEntry objects. Default is
				 * m_defaultHTMLTable.
				 */
				int construct(const TagEntry *pte = m_defaultHTMLTable);
				const TagEntry *search(const _char *pName);
				const _char *getRootElement();

			protected:
				Status m_iStatus;
				const _char *m_pRootElementName;
				hash_map<const _char *, const TagEntry *,
					hashcase<const _char *>, eqcase> m_TERepository;
		};

	public:
		/**
		 * Class encapsulating all information about an individual Tag
		 * instance, including name, attributes etc.
		 */
		class Tag
		{
			public:
				const _char *m_pName;
				const _char *m_pStrEnd;
				_char *m_pTagAttrString;
				bool m_bIsEndTag;
				bool m_bIsSelfClosing;
				TagAttrList m_attrList;

			public:
				Tag();
				/**
				 * Constructor for a simple Tag element with no attributes
				 */
				Tag(const _char *pName);
				/**
				 * Constructor which takes a tag string
				 * 
				 * A complete string (without the &lt; and &gt;) which denotes
				 * a tag string. The bool attribute tells whether the
				 * attributes should be parsed or not. If the attributes are
				 * parsed, the values are stored in an internal list object.
				 */
				Tag(_char *pString, bool bParseAttributes);
				void reset();

				/**
				 * If the bool argument in the constructor is false, then this
				 * function, called anytime later, causes the attributes to be
				 * parsed
				 */
				int parseFurther();
				void toString(_string &targetString);

				int parseTag(_char **pTagString, _char const **pName,
						Tag *pTag, bool bParseAttributes);

			public:
				int
					parseAttributes(_char *&pTagAttrString, Tag *pTag);
				const _char *findProperty(const _char *pName);

			private:
				inline int getQuotedName(_char *&pStr, _char **pName,
						bool bSetNull = false);
				inline int getQuotedValue(_char *&pStr,
						_char **pValue, bool bSetNull = false);
		};

	public:
		/**
		 * The DOM element
		 *
		 * The tree is made up of DOMNode nodes. This tree is generated by the
		 * HyXML::parse call. One can derive from this basic DOMNode class, and
		 * do all analysis stuff in the derived class. DOMNode provides only
		 * basic functionality.
		 */
		class DOMNode
		{
			public:
				typedef enum
				{
					END = -1,
					ELEMENT,
					TEXT,
					COMMENT
				}
				NodeType;

			private:
					PropertyMap *m_pPropertyMap;

			public:
				DOMNode *m_pParent, *m_pPrev, *m_pNext, *m_pChild, *m_pLast;
				NodeType m_type;
				const _char *m_pName;
				const _char *m_pContent;
				bool m_bSelfClosing;
				int m_iLevel;
				int m_iHorLevel;

			private:
				DOMNode() {};
				void reset();
				int initLevel(int iLevel = 0, bool bNext = true);

			public:
				DOMNode(HyXML::DOMNode::NodeType nodeType, const _char *pStr);
				~DOMNode();

				DOMNode *clone(bool bCopyProperties = false);
				DOMNode *clone(NodeType nodeType, const _char *pStr,
						bool bCopyProperties = false);

			public:
				int detach();
				int attachAsChild(DOMNode *pNode);
				int insertAsParent(DOMNode *pNode);
				int insertAsChild(DOMNode *pNode);
				int attachAsPrevious(DOMNode *pNode);
				int attachAsNext(DOMNode *pNode);

			public:
				int addProperty(pair<const _char *, const _char *> &p);
				int addProperty(const _char *pName, const _char *pValue);
				int copyAttributes(HyXML::Tag *tag);
				const _char *findProperty(const _char *pName);
				void toString(_string &targetString, bool bChildOnly = false);
		};

	private:
		class EntityStack
		{
			protected:
				deque<const _char *> m_repository;

			public:
				int size();
				void push(const _char *);
				int pop();
				void clear();
				const _char *top();
				int locateFromTop(const _char *needle);
				int locateAnyFromTop(const vector<const _char *> *needle,
						int &index);
				const _char *operator[] (int i)
				{
					return m_repository[i];
				}
		};

	public:
		TagTable m_tagTable;
		OccurenceMap m_occurenceMap;
		OccurenceMap::iterator m_occurenceMapIter;
		EntityStack m_entityStack;
		bool m_bDocStarted, m_bIgnoreUnknownTag;
		TagEntry m_unknownTagEntry;
		const TagEntry *m_pCurTagEntry;

	public:
		DOMNode *m_pCloneableNode, *m_pRootNode, *m_pCurrentNode,
				*m_pCurrentParentNode;

	public:
		HyXML(const TagEntry *pte = TagTable::m_defaultHTMLTable);

	public:
		/**
		 * The 'main' call.
		 *
		 * It does everything, and returns a DOM tree to the caller, with the
		 * supplied pCloneableNode as the root of this DOM tree
		 */
		DOMNode *parse(_char *pTextBuffer, DOMNode *pCloneableNode);

	private:
		void reset();

		inline int handleElement(Tag *t, bool bDocStarted);
		inline int handleIgnoreChildren(Tag &t, WideCharEntityStream &wcs);
		inline int handleText(_char *pText);
		inline int handleComment(_char *pComment);

		int correctStack(Tag *t, const TagEntry *pTagEntry,
				bool bCheckOnly = false);
		int clearCurrentContext(Tag *t, const TagEntry *pTagEntry,
				bool bCheckOnly = false);

		int initDOM(Tag *tag = 0);
		int addNode(const _char *pElementName, bool bClosure = false);
		int addNode(Tag *tag, bool bClosure = false);
		int addNodeSelfContained(DOMNode::NodeType nodeType,
				const _char *pContent);
		int addNodeSelfContained(Tag *t);
		int closeNode();
		int closeNode(unsigned int iNumTimes);

	public:
		/**
		 * A call-back function called when 'tags' are encountered
		 *
		 * Set this to some function you define, and everytime a tag is
		 * encountered in the text, this function is called with the parsed Tag
		 * element. Further processing path depends on the return value of this
		 * function. HyXML proceeds normally only if HyXML::OK is returned. If
		 * HyXML::IGNORE_SIMPLE is returned, this Tag is simply ignored, as if
		 * it never occured in the text. If HyXML::IGNORE_CHILDREN is returned,
		 * all text upto the closing tag of the current tag-entity is ignored.
		 * The IGNORE_CHILDREN way of ignoring is used to tell HyXML not to do
		 * anything with Javascript till &lt;/script&gt; is encountered, for
		 * example.
		 * <p>
		 * Similarly, IGNORE_SIMPLE can be used, say, when one wants to only
		 * gather the links in a document, and does not want the tree to be
		 * created.
		 */
		EntityCbRetval (*tagCb)(Tag *pTag);
		/**
		 * Call back for text entities. Text is ignored if return value is
		 * false
		 */
		bool (*textCb)(_char *pText);
		/**
		 * Call back for comments. Comment is ignored if return value is false
		 */
		bool (*commentCb)(_char *pComment);
};

#endif // _HYPAR_HYXML_H_
