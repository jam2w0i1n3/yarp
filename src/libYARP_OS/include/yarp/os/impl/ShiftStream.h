// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_SHIFTSTREAM_
#define _YARP2_SHIFTSTREAM_

#include <yarp/os/impl/TwoWayStream.h>
#include <yarp/os/impl/IOException.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ShiftStream;
        }
    }
}

/**
 * Container for a replacable I/O stream.
 * Classes implementing this interface can 
 * have their streams "borrowed" or replaced.
 */
class yarp::os::impl::ShiftStream : public TwoWayStream {
public:
    ShiftStream() {
        stream = NULL;
    }

    virtual ~ShiftStream() {
        close();
    }

    void check() {
        /*
        if (stream==NULL) {
            throw IOException("no stream present");
        }
        */
    }

    virtual InputStream& getInputStream() {
        check();
        if (stream==NULL) return nullStream;
        return stream->getInputStream();
    }

    virtual OutputStream& getOutputStream() {
        check();
        if (stream==NULL) return nullStream;
        return stream->getOutputStream();
    }

    virtual const Address& getLocalAddress() {
        check();
        return (stream==NULL)?nullStream.getLocalAddress():(stream->getLocalAddress());
    }

    virtual const Address& getRemoteAddress() {
        check();
        return (stream==NULL)?nullStream.getRemoteAddress():(stream->getRemoteAddress());
    }

    virtual void close() {
        if (stream!=NULL) {
            stream->close();
            delete stream;
            stream = NULL;
        }
    }

    virtual void takeStream(TwoWayStream *stream) {
        close();
        this->stream = stream;
    }

    virtual TwoWayStream *giveStream() {
        TwoWayStream *result = stream;
        stream = NULL;
        return result;
    }

    virtual TwoWayStream *getStream() {
        return stream;
    }

    virtual bool isEmpty() {
        return stream==NULL;
    }

    virtual bool isOk() {
        if (stream!=NULL) {
            return stream->isOk();
        }
        return false;
    }

    virtual void reset() {
        if (stream!=NULL) {
            stream->reset();
        }
    }

    virtual void beginPacket() {
        if (stream!=NULL) {
            stream->beginPacket();
        }
    }

    virtual void endPacket() { 
        if (stream!=NULL) {
            stream->endPacket();
        }
    }

private:
    TwoWayStream *stream;
    NullStream nullStream;
};

#endif