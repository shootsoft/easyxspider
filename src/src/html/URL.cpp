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
#include <hypar/URL.hpp>
#include <hypar/TextUtils.hpp>

#include <iostream>
#include <cassert>
#include <vector>

BEGIN_C_DECLS
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
END_C_DECLS

using namespace std;

unsigned int URLCollector::m_iMaxURLLen = 0;
list<const _char *> URLCollector::m_charURLList;
string URLCollector::m_baseHref;

URL::URL(const string &absolute)
{
	clear();
	parse(absolute, m_protocol, m_host, m_sPort, m_path, m_fragment,
			m_cgiUri, m_bIsCgi);
	if (m_protocol.empty() || m_host.empty() || m_sPort == 0 || m_path.empty())
	{
		debug("Something was empty");
		return;
	}
	rc = URL_OK;
	m_path = escape(m_path);
	simplifyPath();
}

void
URL::clear()
{
	m_protocol.erase();
	m_host.erase();
	m_sPort = 0;
	m_path.erase();
	m_fragment.erase();
	m_cgiUri.erase();
	m_bIsCgi = false;
	m_bIsCgiUriParsed = false;
	m_CGIAttrList.clear();
	rc = URL_BAD_SYNTAX;
}


URL::URL(const string &relative, const string &base)
{
	clear();
	if (URL::isAbsolute(relative))
	{
		parse(relative, m_protocol, m_host, m_sPort, m_path,
				m_fragment, m_cgiUri, m_bIsCgi );
	}
	else
	{
		URL ubase(base);
		if (ubase.rc != URL::URL_OK)
		{
			cerr << "Base URL " << base.c_str() << " erroneous!" << endl;
			return;
		}
		m_protocol = ubase.getProtocol();
		m_host = ubase.getHost();
		m_sPort = ubase.getPort();
		string _relative = escape(relative);
		if ('/' == _relative[0])
		{
			m_path = _relative;
		}
		else
		{
			string bpath = ubase.getPath();
			string::size_type lastSlashPos = bpath.find_last_of("/");
			assert(lastSlashPos != bpath.npos);
			bpath.erase(lastSlashPos);
			m_path = bpath + "/" + _relative;
		}
	}

	/* Remove fragment/cgiUri from the path */
	string::size_type locFrag, locCgi;
	locCgi = m_path.find_first_of("?");
	if (locCgi != m_path.npos)
	{
		m_cgiUri = m_path.substr(locCgi + 1);
		m_path = m_path.substr(0, locCgi);
	}
	locFrag = m_path.find_first_of("#");
	if (locFrag != m_path.npos)
	{
		m_fragment =  m_path.substr(locFrag);
		m_path = m_path.substr(0, locFrag);
	}
	rc = URL_OK;
	simplifyPath();
}

void
URL::parse(const string &_url, string &proto, string &host, short &port,
		string &path, string &fragment, string &cgiUri, bool &iscgi)
{
	string url = _url;
	string::size_type loc, loc1;
	static const char _proto[] = "://";
	if ((loc = url.find(_proto)) == url.npos)
	{
		url = "http://" + url;
	}
	loc = url.find(_proto);
	proto = url.substr(0, loc);
	url = url.substr(loc + strlen(_proto));
	if ((loc = url.find_first_of('/', 0)) == string::npos)
	{
		url += "/";
		loc = url.find_first_of( '/', 0 );
	}
	if (((loc1 = url.find_first_of(':', 0)) != url.npos) && (loc1 < loc))
	{
		host = url.substr(0, loc1);
		TextUtils::tolower(host);
		if (loc1 < loc - 1)
		{
			if (1 != sscanf(url.substr(loc1 + 1, (loc - loc1 - 1)).c_str(),
						"%hd", &port))
			{
				port = 0;
			}
		}
	}
	else
	{
		host = url.substr(0, loc);
		TextUtils::tolower(host);
		if (proto.size() > 0)
		{
			struct servent *serv = getservbyname(proto.c_str(), "tcp");
			if (serv)
			{
				port = ntohs(serv->s_port);
			}
		}
	}
	path = url.substr(loc); /* Skip to first slash */
	loc = path.find_first_of("?");
	if (loc != path.npos)
	{
		cgiUri = path.substr(loc + 1);
		path = path.substr(0, loc);
		iscgi = true;
	}
	loc1 = path.find_first_of("#");
	if (loc1 != path.npos)
	{
		fragment = path.substr(loc1);
		path = path.substr(0, loc1);
	}
}

void URL::simplifyPath()
{
	vector<string> dir;
	string tmpstr;
	const char * cpath;
	cpath = m_path.c_str();
	dir.clear();
	vector<string>::iterator diriter;
	int endDir = 0;
	const char * c, * d;
	c = cpath + 1;
	assert(m_path[0] == '/');
	while ((d = strchr(c, '/')) != NULL)
	{
		tmpstr = "";
		while (c != d)
		{
			tmpstr += *c;
			c++;
		}
		c++;
		dir.push_back(tmpstr);
	}
	if (strlen(c) > 0)
	{
		dir.push_back(string(c));
	}
	else
	{
		endDir = 1;
	}
	int i = -1;
	diriter = dir.begin();
	while (diriter != dir.end())
	{
		if (string("..") == *diriter)
		{
			if (i >= 0)
			{
				dir.erase(diriter);
				i--;
				diriter--;
			}
			dir.erase(diriter);
		}
		else if (string( ".") == *diriter)
		{
			dir.erase(diriter);
		}
		else
		{
			i++;
			diriter++;
		}
	}
	diriter = dir.begin();
	tmpstr = "";
	while (diriter != dir.end())
	{
		tmpstr += "/" + (*diriter);
		diriter++;
	}
	if (endDir == 1)
	{
		tmpstr += "/";
	}
	m_path = tmpstr;
}

bool URL::isAbsolute(const string &url)
{
	string::size_type loc;
	loc = url.find("://", 0);
	if (loc == string::npos)
	{
		return false;
	}
	if (getservbyname(url.substr(0, loc).c_str(), "tcp"))
	{
		return true;
	}
	return false;
}

string URL::getCanonical(bool dropFragment) const
{
	string ans;
	if (rc != URL_OK)
		return ans;
	ans.append(m_protocol);
	ans.append("://");
	ans.append(m_host);
	ans.append(":");

	char cport[sizeof(short) * 2 * CHAR_BIT];
	sprintf(cport, "%hd", m_sPort);
	ans.append(cport);
	ans.append(m_path);
	if (!dropFragment && m_fragment.size())
		ans.append(m_fragment);
	if (m_bIsCgi)
	{
		ans.append("?");
		ans.append(m_cgiUri);
	}
	return ans;
};

const char URL::hex[] = "0123456789ABCDEF";
const unsigned char URL::acceptMask[] =
{
	/* 0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xA 0xB 0xC 0xD 0xE 0xF */
	0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xF,0xE,0x0,0xF,0xF,0xC,
	/* 2x  !"#$%&'()*+,-./  */
	0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x8,0x0,0x0,0x0,0x0,0x0,
	/* 3x 0123456789:&lt;=>?  */
	0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,
	/* 4x @ABCDEFGHIJKLMNO  */
	0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x0,0x0,0x0,0x0,0xF,
	/* 5X PQRSTUVWXYZ[\]^_  */
	0x0,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,
	/* 6x `abcdefghijklmno  */
	0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x0,0x0,0x0,0x0,0x0
		/* 7X pqrstuvwxyz{\}~DEL */
};

bool URL::isAcceptable(unsigned char cx)
{
	if ('/' == cx)
		return true;
	return
		(cx >= 32) &&
		(cx < 128) &&
		((acceptMask[cx - 32]) & XALPHAS);
}

string URL::escape(const string& str)
{
	string result;
	if (!str.empty())
	{
		for (unsigned int i = 0; i < str.size(); i++)
		{
			unsigned char a = (unsigned char)(str[i]);
			if (!isAcceptable(a))
			{
				result+="%";
				result+=hex[a >> 4];
				result+=hex[a & 15];
			}
			else
			{
				result += str[i];
			}
		}
	}
	return result;
}

/*
string URL::unEscape(const string &str)
{
	string result;
	if (str.empty())
		return result;

	int i=0,j=0;
	while (str[i]!='\0')
	{
		if (str[i] == HEX_ESCAPE)
		{
			i++;
			int ch;
			if (str[i]!='\0') ch= HTAsciiHexToChar(str[i++]) * 16;
			if (str[i]!='\0') ch= (ch + HTAsciiHexToChar(str[i++]));
			result+=(char) ch;
		}
		else
			result+=str[i++];
	}
	return result;
}
*/

void URL::parseCgiUri()
{
	if (!m_bIsCgi)
	{
		return;
	}
	if (m_bIsCgiUriParsed)
	{
		return;
	}
	string tmpstr = m_cgiUri;
	string::size_type locamp, loceq;

	for ( ; ; )
	{
		loceq = tmpstr.find_first_of("=");
		locamp = tmpstr.find_first_of("&");
		if (loceq == string::npos)
		{
			break;
		}
		string name = tmpstr.substr(0, loceq);
		string value = tmpstr.substr(loceq + 1, locamp - loceq - 1);
		debug2("Name=", name);
		debug2("value=", value);
		NVPair p(name, value);
		m_CGIAttrList.push_back(p);
		if (locamp == string::npos)
		{
			break;
		}
		tmpstr = tmpstr.substr(locamp + 1);
	}
	return;
}

bool URL::getCgiValue(const string &name, string &value)
{
	CGIAttrList::iterator cx = m_CGIAttrList.begin();
	while (cx != m_CGIAttrList.end())
	{
		if ((*cx).first == name)
		{
			value = (*cx).second;
			return true;
		}
		cx++;
	}
	return false;
}

string URL::getURI()
{
	string retval;
	retval = m_path;
	if ( m_bIsCgi ) {
		retval += "?";
		retval += m_cgiUri;
	}
	return retval;
}

URL::ReturnCode
URL::getParseStatus() const
{
	return rc;
}

string
URL::getProtocol() const
{
	return m_protocol;
}

string
URL::getHost() const
{
	return m_host;
}

short
URL::getPort() const
{
	return m_sPort;
}

string
URL::getPath() const
{
	return m_path;
}

string
URL::getFragment() const
{
	return m_fragment;
}

string
URL::getCgiUri() const
{
	return m_cgiUri;
}

bool
URL::isCGI() const
{
	return m_bIsCgi;
}

URLCollector::URLCollector(HyXML::DOMNode *pNode)
	:
m_pEncoding(0)
{
	m_baseHref = "";
	//collect(pNode);
}

URLCollector::URLCollector(_char *pBuffer)
	:
m_pEncoding(0)
{
	//m_baseHref = "";
	collect(pBuffer);
}

HyXML::EntityCbRetval
URLCollector::href_callback(HyXML::Tag *pTag)
{
	static const _char *pURLTagName = L("a");
	static const _char *pBase = L("base");
	if (!m_baseHref.size() && _strcasecmp(pBase, pTag->m_pName) == 0)
	{
		const _char *pVal = pTag->findProperty(L("href"));
		if (pVal)
		{
			int l = _strlen(pVal);
			if (l > 0)
			{
				char *s = new char[sizeof(_char) * (l + 1)];
				wcstombs(s, pVal, l + 1);
				m_baseHref = reinterpret_cast<char *>(s);
				delete s;
			}
		}
		return HyXML::IGNORE_SIMPLE;
	}
	if (_strcasecmp(pURLTagName, pTag->m_pName) == 0)
	{
		if (!pTag->m_bIsEndTag)
		{
			const _char *pVal = pTag->findProperty(L("href"));
			if (pVal)
			{
				URLCollector::m_charURLList.push_back(pVal);
				if (_strlen(pVal) > URLCollector::m_iMaxURLLen)
				{
					URLCollector::m_iMaxURLLen = _strlen(pVal);
				}
			}
		}
	}
	return HyXML::IGNORE_SIMPLE;
}

int
URLCollector::collect(_char *pBuffer, const char *pEncoding)
{
	setlocale(LC_ALL, pEncoding);
	URLCollector::m_iMaxURLLen = 0;
	HyXML h;
	h.tagCb = &URLCollector::href_callback;
	h.textCb = &URLCollector::text_callback;
	h.commentCb = &URLCollector::comm_callback;
	HyXML::DOMNode node(HyXML::DOMNode::ELEMENT, L("root"));
	node.m_iLevel = -1;
	URLCollector::m_charURLList.clear();
	h.parse(pBuffer, &node);
	list<const _char *>::iterator ulx = URLCollector::m_charURLList.begin();
	//cout<<"sbsbsb"<<endl;
#ifndef HYPAR_WIDE_CHAR
	const
#endif
		char *pLocalLocaleBuffer =
#ifdef HYPAR_WIDE_CHAR
		new char[URLCollector::m_iMaxURLLen*sizeof(_char) + 4];
#else
	0;
#endif
	while (ulx != URLCollector::m_charURLList.end())
	{
#ifdef HYPAR_WIDE_CHAR
		wcstombs(pLocalLocaleBuffer, *ulx, URLCollector::m_iMaxURLLen);
#else
		pLocalLocaleBuffer = *ulx;
#endif
		//cout<<"Debug:"<<m_baseHref<<endl;
		URL u(pLocalLocaleBuffer, m_baseHref);
		if (u.getParseStatus() == URL::URL_OK)
		{
			m_URLList.push_back(string(u.getCanonical()));
		}
		ulx++;
	}
#ifdef HYPAR_WIDE_CHAR
	delete pLocalLocaleBuffer;
#endif
	return 0;
}
