// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_NETTYPE_
#define _YARP2_NETTYPE_

#include <yarp/os/impl/String.h>
#include <yarp/Bytes.h>
#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/IOException.h>
#include <yarp/os/impl/Logger.h>

#include <ace/OS_NS_stdlib.h>

#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>

namespace yarp {
    namespace os {
        namespace impl {
            class NetType;
        }
    }
}

/**
 * Various utilities related to types and formats.
 */
class yarp::os::impl::NetType {
public:

    static int netInt(const Bytes& code) {
        YARP_ASSERT(code.length()==sizeof(NetType::NetInt32));
        //// this does not work on Solaris with gcc 3.2
        //NetType::NetInt32& i = *((NetType::NetInt32*)(code.get()));
        //return i;
        NetType::NetInt32 tmp;
        ACE_OS::memcpy((char*)(&tmp),code.get(),code.length());
        return tmp;
    }

    static bool netInt(int data, const Bytes& code) {
        NetType::NetInt32 i = data;
        Bytes b((char*)(&i),sizeof(i));
        if (code.length()!=sizeof(i)) {
            YARP_ERROR(Logger::get(),"not enough room for integer");
            return false;
        }
        ACE_OS::memcpy(code.get(),b.get(),code.length());
        return true;
    }

    static String readLine(InputStream& is, int terminal = '\n',
                           bool *success = NULL);

    static int readFull(InputStream& is, const Bytes& b);

    static int readDiscard(InputStream& is, int len);

    static String toString(int x);

    static String toString(long x);

    static String toString(unsigned int x);

    static int toInt(String x);

    static unsigned long int getCrc(char *buf, int len);

    typedef yarp::os::NetInt32 NetInt32;
    typedef yarp::os::NetFloat64 NetFloat64;
};

#endif