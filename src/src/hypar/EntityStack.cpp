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

#define DEBUG 0
#include <hypar/Debug.hpp>

typedef deque<const _char *>::iterator repIter;

int
HyXML::EntityStack::size()
{
	return m_repository.size();
}

void
HyXML::EntityStack::push(const _char *s)
{
	assert(s);
	m_repository.push_back(s);
}

int
HyXML::EntityStack::pop()
{
	if (m_repository.size() > 0)
	{
		m_repository.pop_back();
		return 1;
	}
	return 0;
}

void
HyXML::EntityStack::clear()
{
	while(pop());
}

const _char *
HyXML::EntityStack::top()
{
	return m_repository.back();
}

int
HyXML::EntityStack::locateFromTop(const _char *s)
{
	assert(s != 0);
	int sz = size();
	if (sz <= 0)
	{
		return -1;
	}
	for (int i = sz - 1; i >= 0; i--)
	{
		if (_strcasecmp(s, m_repository[i]) == 0)
		{
			wdebug3(L("Found at"), s, sz - i - 1);
			return sz - i - 1;
		}
	}
	return -1;
}

int
HyXML::EntityStack::locateAnyFromTop(const vector<const _char *> *needle,
		int &index)
{
	int sz = needle->size();
	int iPosFromTop = 0;
	for (index = 0; index < sz; index++)
	{
		iPosFromTop = locateFromTop(needle->at(index));
		if (iPosFromTop >= 0)
		{
			return iPosFromTop;
		}
	}
	return -1;
}
