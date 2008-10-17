// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_CARRIERS_
#define _YARP2_CARRIERS_

#include <yarp/os/impl/String.h>
#include <yarp/os/Bytes.h>
#include <yarp/os/impl/Address.h>
#include <yarp/os/impl/Face.h>
#include <yarp/os/impl/OutputProtocol.h>
#include <yarp/os/impl/Carrier.h>

#include <ace/Vector_T.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Carriers;
        }
    }
}

/**
 * This class is the starting point for all communication.
 */
class yarp::os::impl::Carriers {
public:
    static Carrier *chooseCarrier(const String& name);
    static Carrier *chooseCarrier(const Bytes& bytes);

    static Face *listen(const Address& address); // throws IOException
    static OutputProtocol *connect(const Address& address); // throws IOException

    // msvc seems to want the destructor public, even for static private instance
    virtual ~Carriers();

private:
    ACE_Vector<Carrier *> delegates;

    Carriers();
  
    static Carriers instance;
    static Carriers& getInstance() {
        return instance;
    }

    Carrier *chooseCarrier(const String * name, const Bytes * bytes);  
};


#endif

