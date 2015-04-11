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

#ifndef _URL_H_
#define _URL_H_

#include <hypar/localdefs.h>
#include <hypar/HyXML.hpp>
#include <string>
#include <list>

typedef pair<string, string> NVPair;
typedef list<NVPair> CGIAttrList;

/**
 * The URL parser class
 *
 * Use this to parse URLs and extract various components, including
 * protocol, port, host, path, cgi name-value pairs, views
 */

/**
 * Class for parsing and working with URLs
 */
class URL
{
	public:
		/**
		 * Indicates successes and failures */
		typedef enum
		{
			URL_BAD_SYNTAX = -100,
			URL_OK = 0
		}
		ReturnCode;

	protected:
		static const char hex[];
		static const unsigned char acceptMask[];
		static const unsigned int XALPHAS = 0x1;

		string m_protocol, m_host, m_path, m_fragment, m_cgiUri;
		short m_sPort;
		ReturnCode rc;
		bool m_bIsCgi, m_bIsCgiUriParsed;
		CGIAttrList m_CGIAttrList;

	protected:
		void simplifyPath();

	public:
		/**
		 * static call for parsing - fills up all values */
		static void parse(
				const string &url,
				string &proto,
				string &host,
				short  &port,
				string &path,
				string &fragment,
				string &cgiUri,
				bool   &iscgi);
		/**
		 * Tells whether the character is an acceptable character in URLs */
		static bool isAcceptable( unsigned char cx );
		/**
		 * Clear up all attributes */
		void clear();

	public:
		/**
		 * Constructor from absolute URLs */
		URL(const string & absolute);
		/**
		 * Constructor from relative URLs, given the base URL */
		URL(const string & relative, const string & base);
		/**
		 * Get the status after parsing */
		ReturnCode getParseStatus() const;
		/**
		 * Get the canonical form of the URL */
		string getCanonical( bool dropFragment = false ) const;
		/**
		 * Get the protocol in string format */
		string getProtocol() const;
		/**
		 * Get the target host in string format */
		string getHost() const;
		/**
		 * Get the port of the target host as a short int */
		short getPort() const;
		/**
		 * Get the URI path on the destination host */
		string getPath() const;
		/**
		 * Get the fragment indicator, like the part after #
		 * in http://..../a/b.html#part2 */
		string getFragment() const;
		/**
		 * Get the CGI component, after the '?' character */
		string getCgiUri() const;
		/**
		 * Get the resource indicator on the target host */
		string getURI();
		/**
		 * Parse the CGI component, and store the name-value pairs */
		void parseCgiUri();
		/**
		 * Given a CGI LHS variable, extract the value on the RHS */
		bool getCgiValue(const string &name, string &value);
		/**
		 * Is the URL a dynamic, CGI URL? */
		bool isCGI() const;

		/**
		 * Is the supplied URL absolute? */
		static bool isAbsolute( const string & aurl );
		/**
		 * Escape invalid characters, and convert them */
		static string escape( const string & unes );
		/**
		 * Unescape chars in the URL */
		static string unEscape( const string & esc );

};

class URLCollector
{
	public:
		list<string> m_URLList;
		static list<const _char *> m_charURLList;
		static unsigned int m_iMaxURLLen;
		static string m_baseHref;
		const char *m_pEncoding;

	public:
		URLCollector();
		URLCollector(HyXML::DOMNode *pNode);
		URLCollector(_char *pBuffer);

	public:
		int collect(HyXML::DOMNode *pNode);
		int collect(_char *pBuffer, const char *pEncoding = "ISO-8859-1");

	public:
		static HyXML::EntityCbRetval href_callback(HyXML::Tag *pTag);
		static inline bool text_callback(_char *p) { return false; }
		static inline bool comm_callback(_char *p) { return false; }
};

#endif /* _URL_H_ */
