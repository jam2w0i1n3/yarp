// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/Logger.h>
#include <yarp/String.h>

#include <ace/OS.h>

using namespace yarp::os;
using namespace yarp;

#define RTARGET stderr

class ResourceFinderHelper {
private:
    yarp::os::Bottle apps;
    yarp::os::ConstString root;
    yarp::os::ConstString policyName;
    yarp::os::Property config;
    bool verbose;
public:
    ResourceFinderHelper() {
        verbose = false;
    }

    bool addAppName(const char *appName) {
        apps.addString(appName);
        return true;
    }
    
    bool clearAppNames() {
        apps.clear();
        return true;
    }
    
    bool configureFromPolicy(const char *policyName) {
        this->policyName = policyName;
        if (verbose) {
            fprintf(RTARGET,"||| policy set to %s\n", policyName);
        }
        String rootVar = policyName;
        const char *result = 
            ACE_OS::getenv(rootVar.c_str());
        if (result==NULL) {
            root = "";
            fprintf(RTARGET,"||| environment variable %s not set\n", 
                    rootVar.c_str());
            return false;
        }
        root = result;
        if (verbose) {
            fprintf(RTARGET,"||| %s: %s\n", 
                    rootVar.c_str(),root.c_str());
        }
        String rootConfig = String(root.c_str()) + "/" + policyName + ".ini";
        if (verbose) {
            fprintf(RTARGET,"||| loading policy from %s\n", 
                    rootConfig.c_str());
        }
        bool ok = config.fromConfigFile(rootConfig.c_str());
        if (!ok) {
            fprintf(RTARGET,"||| failed to load policy from %s\n", 
                    rootConfig.c_str());
            return false;
        }

        // currently only support "capability" style configuration
        if (config.check("style",Value("")).asString()!="capability") {
            fprintf(RTARGET,"||| policy \"style\" can currently only be \"capability\"\n");
            return false;
        }

        return true;
    }

    bool configureFromCommandLine(int argc, char *argv[]) {
        fprintf(RTARGET,"||| warning: not yet using command line overrides\n");
        return true;
    }


    yarp::os::ConstString check(const char *base1, 
                                const char *base2, 
                                const char *base3, 
                                const char *name) {
        String s = "";
        if (base1!=NULL) {
            s = base1;
            if (String(base1)!="") {
                s = s + "/";
            }
        }
        if (base2!=NULL) {
            s = s + base2;
            if (String(base2)!="") {
                s = s + "/";
            }
        }
        if (base3!=NULL) {
            s = s + base3;
            if (String(base3)!="") {
                s = s + "/";
            }
        }
        s = s + name;
        if (verbose) {
            fprintf(RTARGET,"||| checking %s\n", s.c_str());
        }
        if (exists(s.c_str())) {
            fprintf(RTARGET,"||| found %s\n", s.c_str());
            return s.c_str();
        }
        return "";
    }

    yarp::os::ConstString findFile(const char *name) {

        ConstString cap = 
            config.check("capability_directory",Value("app")).asString();
        ConstString defCap = 
            config.check("default_capability",Value("default")).asString();

        // check current directory
        ConstString str = check("","","",name);
        if (str!="") return str;

        // check ROOT/app/default/
        str = check(root.c_str(),cap,defCap,name);
        if (str!="") return str;

        // check app dirs
        for (int i=0; i<apps.size(); i++) {
            str = check(root.c_str(),cap,apps.get(i).asString().c_str(),
                        name);
            if (str!="") return str;
        }

        fprintf(RTARGET,"||| did not find %s\n", name);
        return "";
    }

    bool setVerbose(bool verbose) {
        this->verbose = verbose;
        return this->verbose;
    }

    bool exists(const char *fname) {
        ACE_stat s;
        int result = ACE_OS::stat(fname,&s);
        return (result==0);
	}
};

#define HELPER(x) (*((ResourceFinderHelper*)(x)))

ResourceFinder::ResourceFinder() {
    implementation = new ResourceFinderHelper;
    YARP_ASSERT(implementation!=NULL);
}

ResourceFinder::~ResourceFinder() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


bool ResourceFinder::configureFromPolicy(const char *policyName) {
    return HELPER(implementation).configureFromPolicy(policyName);
}

bool ResourceFinder::configureFromCommandLine(int argc, char *argv[]) {
    return HELPER(implementation).configureFromCommandLine(argc,argv);
}

bool ResourceFinder::addContext(const char *appName) {
    return HELPER(implementation).addAppName(appName);
}

bool ResourceFinder::clearContext() {
    return HELPER(implementation).clearAppNames();
}

yarp::os::ConstString ResourceFinder::findFile(const char *name) {
    return HELPER(implementation).findFile(name);
}


bool ResourceFinder::setVerbose(bool verbose) {
    return HELPER(implementation).setVerbose(verbose);
}