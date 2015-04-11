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
#include <hypar/MmapWrapper.hpp>
#include <hypar/ToWideCharConverter.hpp>
#include <hypar/WideCharEntityStream.hpp>
#include <hypar/HyXML.hpp>
#include <hypar/URL.hpp>
#include <hypar/HashUtils.hpp>
#include <hypar/TextUtils.hpp>


#define DEBUG 1
#include <hypar/Debug.hpp>

#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <list>

BEGIN_C_DECLS
#include <assert.h>
#include <stdio.h>
#include <locale.h>
END_C_DECLS

using namespace std;
#if __GNUC__ > 2
using namespace __gnu_cxx;
#endif
list<_char *> URLList;
unsigned int iMaxURLLen = 0;

int main(int argc, char *argv[])
{
	string url1="http://www.shootsoft.net";
	string url2="http://www.shootsoft.net/";
	string url3="http://www.shootsoft.net/test/";
	string url4="http://www.shootsoft.net/test1/test2/1.html";

	string relative1 = "/index.html";
	string relative2 = "./index.html";
	string relative3 = "../../index.html";

	URL* u1=new URL(relative1, url1);
	URL* u2=new URL(relative2, url1);
	URL* u3=new URL(relative1, url2);
	URL* u4=new URL(relative2, url2);
	URL* u5=new URL(relative3, url2);
	URL* u6=new URL(relative2, url3);
	URL* u7=new URL(relative3, url3);
	URL* u8=new URL(relative1, url3);

	cout<<u1->getCanonical()<<endl;
	cout<<u2->getCanonical()<<endl;
	cout<<u3->getCanonical()<<endl;
	cout<<u4->getCanonical()<<endl;
	cout<<u5->getCanonical()<<endl;
	cout<<u6->getCanonical()<<endl;
	cout<<u7->getCanonical()<<endl;
	cout<<u8->getCanonical()<<endl;
	return 0;
}
