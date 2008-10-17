// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_LOGGER_
#define _YARP2_LOGGER_

#include <yarp/os/impl/String.h>

#include <ace/Log_Msg.h>
#include <ace/Log_Record.h>
#include <ace/Log_Msg_Callback.h>
#include <ace/OS_NS_stdio.h>

namespace yarp {
    namespace os {
        namespace impl {
            class Logger;
        }
    }
}

/**
 * This is a wrapper for message logging.
 * This is currently a sad mixture of the java yarp logging mechanism
 * and ACE.
 *
 */
class yarp::os::impl::Logger : public ACE_Log_Msg_Callback {
public:
    enum Level {
        DEBUG = LM_DEBUG,
        MAJOR_DEBUG = LM_INFO,
        WARNING = LM_WARNING,
        INFO = LM_INFO,
        SEVERE = LM_ERROR,

        DEFAULT_WARN=INFO,
    };

    Logger(const char *prefix, Logger *parent = NULL) {
        this->prefix = prefix;
        this->parent = parent;
        verbose = 0;
        low = DEFAULT_WARN;
        if (this==&root) {
            ACE_Log_Msg *acer = ACE_Log_Msg::instance();
            acer->set_flags(8);
            acer->clr_flags(1);
            acer->msg_callback(this);
        }
    }

    Logger(const char *prefix, Logger& parent) {
        this->prefix = prefix;
        this->parent = &parent;
        verbose = 0;
        low = DEFAULT_WARN;
    }

    static Logger& get();

    virtual void log(ACE_Log_Record& log_record) {
        show(log_record.type(),log_record.msg_data());
    }

    void debug(const String& txt) {
        show(LM_DEBUG,txt);
    }

    void println(const String& txt) {
        debug(txt);
    }

    void info(const String& txt) {
        show(LM_INFO,txt);
    }

    void warning(const String& txt) {
        show(LM_WARNING,txt);
    }

    void error(const String& txt) {
        show(LM_ERROR,txt);
    }

    void fail(const String& txt) {
        show(LM_ERROR,txt);
        exit(1);
    }

    void assertion(bool cond) {
        if (!cond) {
            fail("assertion failure");
        }
    }

    void setVerbosity(int verbose = 0) {
        this->verbose = verbose;
    }

    void setPrefix(const char *prefix) {
        this->prefix = prefix;
    }

    void setPid();

    long int getPid() {
        return pid;
    }

private:
    void show(int level, const String& txt);
    void exit(int result);

    static Logger root;
    String prefix;
    Logger *parent;
    int verbose;
    int low;
    long int pid;
};

// compromise - use macros so that debugging can evaporate in optimized code.
// also, make a printf-style adaptor since c++ is a bit of a pain to 
// build strings in.
#define YARP_ERROR(log,x) log.error(x)
#define YARP_WARN(log,x)  log.warn(x)
#define YARP_INFO(log,x)  log.info(x)
#define YARP_DEBUG(log,x) log.debug(x)
#define YARP_FAIL(log,x)  log.fail(x)

#define YARP_ASSERT(x) if (!(x)) { ACE_OS::printf("Assertion failure %s:%d  !(%s)\n",__FILE__,__LINE__, #x ); ACE_OS::exit(1); }


#endif