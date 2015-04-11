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

/* 
 *  _char  int            int           bool     bool       _char  _char
 *  name     contextSwitch  contextLevel  closure  occurOnce  parent   rparent
 */

const HyXML::TagEntry HyXML::TagTable::m_defaultHTMLTable[] =
{
	TagEntry(L("html"),   1,  1,      true,  true,    EMPTY,   EMPTY),
	TagEntry(L("head"),   1,  2,      true,  true,    L("html"),  EMPTY),
	TagEntry(L("body"),   1,  2,      true,  true,    L("html"), EMPTY),
	TagEntry(L("table"),  2,  11,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("col"),    0,  12,     false, false,   L("table"), EMPTY),
	TagEntry(L("tr"),     0,  5,      true,  false,   L("table"), EMPTY),
	TagEntry(L("th"),     0,  6,      true,  false,   L("tr"),   EMPTY),
	TagEntry(L("td"),     0,  6,      true,  false,   L("tr"),   EMPTY),
	TagEntry(L("title"),  0,  10,     true,  true,    L("head"), EMPTY),
	TagEntry(L("pre"),    0,  11,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("center"), 2,  10,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("noscript"), 2,  10,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("p"),      1,  12,     false,  false,  EMPTY,   L("body")),
	TagEntry(L("ol"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("ul"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("h1"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("h2"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("h3"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("h4"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("h5"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("h6"),     1,  13,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("li"),     1,  14,     true,  false,   L("ul,ol"),    EMPTY),
	TagEntry(L("form"),   2,  90,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("div"),    2,  90,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("style"),  2,  90,     true,  false,   EMPTY,   L("html")),
	TagEntry(L("script"), 2,  90,     true,  false,   EMPTY,   L("body")),
	TagEntry(L("select"), 0,  91,     true,  false,   EMPTY,   L("form")),
	TagEntry(L("option"), 1,  92,     false,  false,  L("select"),   EMPTY),
	TagEntry(L("input"),  0,  99,     false,  false,  EMPTY, L("form")),
	TagEntry(L("textarea"), 0, 99,     true,  false,  L("form"), EMPTY),
	TagEntry(L("map"), 0, 98,     true,  false,  EMPTY, L("body")),
	TagEntry(L("area"), 0, 99,     false,  false,  L("map"), EMPTY),
	TagEntry(L("span"),   2,  TEXT_ATTR, true,  false,   EMPTY,   L("body")),
	TagEntry(L("font"),   2,  TEXT_ATTR, true,  false, EMPTY,   L("body")),
	TagEntry(L("a"),      0,  TEXT_ATTR, true,  false, EMPTY,   L("body")),
	TagEntry(L("b"),      0,  TEXT_ATTR, true,  false, EMPTY,   L("body")),
	TagEntry(L("u"),      0,  TEXT_ATTR, true,  false, EMPTY,   L("body")),
	TagEntry(L("i"),      0,  TEXT_ATTR, true,  false, EMPTY,   L("body")),
	TagEntry(L("address"), 0, TEXT_ATTR, true,  false, EMPTY,   L("body")),
	TagEntry(L("img"),    0,  TEXT_ATTR, false,  false, EMPTY,   L("body")),
	TagEntry(L("base"),   0,  TEXT_ATTR, false,  true,  L("head"), EMPTY),
	TagEntry(L("br"),     0,  TEXT_ATTR, false,  false, EMPTY,   L("body")),
	TagEntry(L("nobr"),     0,  TEXT_ATTR, true,  false, EMPTY,   L("body")),
	TagEntry(L("hr"),     0,  TEXT_ATTR, false,  false, EMPTY,   L("body")),
	TagEntry(L("strong"), 0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("em"),     0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("dd"),     0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("dl"),     2,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("dt"),     0,  TEXT_ATTR, false,  false, EMPTY,   L("body")),
	TagEntry(L("code"),   0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("sup"),    0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("sub"),    0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("samp"),   0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("cite"),   0,  TEXT_ATTR, true,  false,  EMPTY,   L("body")),
	TagEntry(L("meta"),   0,  TEXT_ATTR, false,  true,  L("head"), EMPTY),
	TagEntry(L("link"),   0,  TEXT_ATTR, false,  false, EMPTY,   L("body")),
	TagEntry(EMPTY,       0,  TEXT_ATTR, false,  false, EMPTY,   EMPTY)
};
