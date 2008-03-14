// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Alessandro Scalzo alessandro@liralab.it
 * Copyright (C) 2007 RobotCub Consortium
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __SHMEM_OUTPUT_STREAM__
#define __SHMEM_OUTPUT_STREAM__

#include <ace/config.h>
#include <ace/Process_Semaphore.h>
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/Log_Msg.h>
#if defined(ACE_LACKS_SYSV_SHMEM)
#include <ace/Shared_Memory_MM.h>
#else
#include <ace/Shared_Memory_SV.h>
#endif

#include <yarp/os/Semaphore.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/Logger.h>

#include <yarp/OutputStream.h>
#include <yarp/ShmemTypes.h>

namespace yarp 
{
	class ShmemOutputStream;
};

using namespace yarp;

class yarp::ShmemOutputStream : public OutputStream
{
public:
	ShmemOutputStream()
	{
		m_bOpen=false;

		m_pAccessMutex=m_pWaitDataMutex=0;
		m_pMap=0;
		m_pData=0;
		m_pHeader=0;
		m_ResizeNum=0;
		m_Port=0;
	}

	~ShmemOutputStream()
	{
		close();
	}

	bool isOk() { return m_pMap!=0; }
	bool open(int port,int size=SHMEM_DEFAULT_SIZE);
	void write(const Bytes& b);
	void close();
	
protected:
	bool Resize(int newsize);

	bool m_bOpen;

	int m_ResizeNum;
	int m_Port;
	ACE_Process_Semaphore *m_pAccessMutex,*m_pWaitDataMutex;
	ACE_Shared_Memory *m_pMap;
	char *m_pData;
	ShmemHeader_t *m_pHeader;
};

#endif