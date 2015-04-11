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

#include <hypar/MmapWrapper.hpp>
#define DEBUG 0
#include <hypar/Debug.hpp>

BEGIN_C_DECLS
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
END_C_DECLS

MmapWrapper::MmapWrapper(const char *pFileName)
:
m_iStatus(MmapWrapper::EMMAPINVAL),
m_pFileName(pFileName),
m_pMemBuffer(0),
m_lFileLen(0)
{
	if (init() == MmapWrapper::OK)
		m_iStatus = MmapWrapper::OK;
}

inline int MmapWrapper::init()
{
	int fd;
	struct stat fstatinfo;

	if ((fd = open(m_pFileName, O_RDONLY)) < 0)
	{
		debug2("Error in read-only open of file", m_pFileName);
		return MmapWrapper::EMMAPFILEREAD;
	}
	
	if (fstat(fd, &fstatinfo) < 0)
	{
		debug2("Error in stat'ting file", m_pFileName);
		return MmapWrapper::EMMAPINVAL;
	}

	m_lFileLen = (size_t) fstatinfo.st_size;

	m_pMemBuffer = (const char *)
		mmap(0, m_lFileLen, PROT_READ, MAP_PRIVATE, fd, 0);

	if (m_pMemBuffer)
	{
		return MmapWrapper::OK;
	}
	else
	{
		return MmapWrapper::EMMAPINVAL;
	}
}

MmapWrapper::~MmapWrapper()
{
	if (m_pMemBuffer)
	{
		munmap((void *)m_pMemBuffer, m_lFileLen);
	}
	m_pMemBuffer = 0;
}

MmapWrapper::Status
MmapWrapper::getStatus()
{
	return m_iStatus;
}

const char *
MmapWrapper::getBuffer()
{
	return m_pMemBuffer;
}

const char *
MmapWrapper::getFileName()
{
	return m_pFileName;
}

long
MmapWrapper::getBufferLength()
{
	return m_lFileLen;
}
