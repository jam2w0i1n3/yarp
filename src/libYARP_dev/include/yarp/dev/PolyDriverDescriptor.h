// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __POLYDRIVERDESCRIPTOR__
#define __POLYDRIVERDESCRIPTOR__

/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/

#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace dev {
        class PolyDriverDescriptor;
    }
}

class yarp::dev::PolyDriverDescriptor
{
public:
    /* pointer to the polydriver */
    PolyDriver *poly;
    /* descriptor */
    yarp::os::ConstString key;
   
    PolyDriverDescriptor()
    {
        poly=0;
        key="";
    }

    PolyDriverDescriptor(PolyDriver *np, const char *k)
    {
           poly=np;
           key=yarp::os::ConstString(k);
    }
};

#endif