// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Ali Paikan and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef PRIORITYCARRIER_INC
#define PRIORITYCARRIER_INC

#include <math.h>
#include <yarp/os/impl/ModifyingCarrier.h>
#include <yarp/os/impl/Election.h>
#include <yarp/os/NullConnectionReader.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#define STIMUL_THRESHOLD        1.0
#define WITH_RIORITY_DEBUG

namespace yarp {
    namespace os {
        namespace impl {
            class PriorityGroup;
            class PriorityCarrier;
#ifdef WITH_RIORITY_DEBUG
            class PriorityDebugThread;
#endif //WITH_RIORITY_DEBUG
        }
    }
}



/**
 *
 * Manager for priority-aware inputs to a given port.
 *
 */
class yarp::os::impl::PriorityGroup : public PeerRecord {
public:
    virtual ~PriorityGroup() {}
    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader,
                                    PriorityCarrier *source);
    bool recalculate(double t); 

public:
    yarp::sig::Matrix InvA;         // the inverse of matrix (I-A) in the equation y(t) = [(I-A)^(-1) * B] .*x(t) 
    yarp::sig::Matrix B;            // matrix of biases B in the equation y(t) = [(I-A)^(-1) * B] .*x(t)
    yarp::sig::Matrix Y;            // matrix y(t) 
    yarp::sig::Matrix X;            // matrix x(t)
    //yarp::os::Semaphore semDebug;   // this semaphor is used only when debug mode is active
                                    // to control the access to matrices from debug thread
};



#ifdef WITH_RIORITY_DEBUG
class yarp::os::impl::PriorityDebugThread : public yarp::os::RateThread {
public:
    PriorityDebugThread(PriorityCarrier* carrier);
    virtual ~PriorityDebugThread();
    void run();
    bool threadInit();
    void threadRelease();

public:
    int count;
    PriorityCarrier* pcarrier;
    String debugPortName;
    BufferedPort<yarp::sig::Vector> debugPort;
};
#endif //WITH_RIORITY_DEBUG


/**
 *
 * Allow priority-based message selection.  Under development.
 * Affected by carrier modifiers.  Examples:
 *   tcp+recv.priority+level.15
 *
 */
class yarp::os::impl::PriorityCarrier : public yarp::os::impl::ModifyingCarrier {

#ifdef WITH_RIORITY_DEBUG
    friend class PriorityDebugThread;
#endif //WITH_RIORITY_DEBUG

public:
    PriorityCarrier()
#ifdef WITH_RIORITY_DEBUG
    : debugger(this)
#endif //WITH_RIORITY_DEBUG
    {
        group = 0/*NULL*/;
        timeConstant = timeArrival = 0;
        timeResting = 0;
        stimulation = 0;
        isVirtual = false;
        isActive = false;
        baias = 0;
        yi = 0;     // used in debug
    }

    virtual ~PriorityCarrier() {
        if (portName!="") {
            // let peer carriers know I'm gone.
            getPeers().remove(portName,this);
        }
    }

    virtual Carrier *create() {
        return new PriorityCarrier();
    }

    virtual String getName() {
        return "priority";
    }

    virtual String toString() {
        return "priority_carrier";
    }

    bool isResting(double priority){
        return ((timeResting > 0) &&
                ((priority < 0) || (priority >= STIMUL_THRESHOLD)));
    }


    void stimulate(double t) {

        temporalStimulation = getActualStimulation(t);
        // if it is in active sate, stimulates.
        if(!isResting(temporalStimulation))
        {
            temporalStimulation += stimulation;
            // respecting priority ceiling
            if(temporalStimulation >= STIMUL_THRESHOLD)
            {
                temporalStimulation = STIMUL_THRESHOLD;
                isActive = true;
            }
            // updating arrival time
            timeArrival = t;
        }
    }

    double getActualStimulation(double t);

    double getActualInput(double t);

    virtual bool configure(yarp::os::impl::Protocol& proto);

    virtual bool acceptIncomingData(yarp::os::ConnectionReader& reader);

    virtual void setCarrierParams(const yarp::os::Property& params) {
        yarp::os::Property property = params;
        timeConstant = property.check("tc", Value(timeConstant)).asDouble();
        timeResting = property.check("tr", Value(timeResting)).asDouble();
        stimulation = property.check("st", Value(stimulation)).asDouble();
        baias = property.check("bs", Value(baias)).asDouble();
        isVirtual = property.check("virtual", Value(isVirtual)).asBool();
        if(property.check("ex"))
            excitation = property.findGroup("ex");
    }

    virtual void getCarrierParams(yarp::os::Property& params) {
        params.put("tc", timeConstant);
        params.put("tr", timeResting);
        params.put("st", stimulation);
        params.put("bs", baias);
        params.put("virtual", (int)isVirtual);
        params.put("ex", excitation.toString());
    }



public:
    double timeConstant;            // priority reset time (tc > 0)
    double timeResting;             // priority resting time (tr > 0)
    double stimulation;             // stimulation value of the message (s > 0)
    double temporalStimulation;     // current priority of the message S(t)
    double timeArrival;             // arrival time of the message
    bool isVirtual;                 // a virtual link does not carry any data
    bool isActive;                  // true if port is in active state X(t)
    double baias;                   // baias value for excitation
    Bottle excitation;              // a list of exitatory signals as (name, value)
    String sourceName;

    double yi;                      // this is set in the recalculate() for the debug purpose

private:
    String portName;
    PriorityGroup *group;

    static ElectionOf<PriorityCarrier,PriorityGroup> *peers;

    static ElectionOf<PriorityCarrier,PriorityGroup>& getPeers();

#ifdef WITH_RIORITY_DEBUG
private:
    PriorityDebugThread debugger;
#endif //WITH_RIORITY_DEBUG
};



#endif


