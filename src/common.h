// -*-c++-*-
//******************************************************************
//
// Copyright 2014 Intel Corporation.
// Copyright 2015 Eurogiciel <philippe.coval@eurogiciel.fr>
// Copyright 2016 Samsung Electronics France SAS All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <iotivity/resource/OCApi.h>

#if !defined(PACKAGE)
#define PACKAGE "Example"
#endif

/** Pseudo singleton class to store common configuration variables **/
class Common
{
    public:
        /** public interface (used both sides) **/
        static std::string  m_interface;
        /** type of resource (used both sides) **/
        static std::string  m_type;
        /** url's path (used both sides) **/
        static std::string  m_endpoint;
        /** resource policy **/
    static const uint8_t m_policy = OC_DISCOVERABLE | OC_OBSERVABLE //unsecured here
        //| OC_SECURE // added to support coaps
        ;
        /** polling period**/
        static int m_period;
        /** log enabled if positive **/
        static int m_logLevel;
    public:
        /** Resource properties **/
        static double m_brightness;
    public:
        static void log(char const *const message);
};

#include <iostream>

class Logger
{
    public:
        Logger(const char *message);
        ~Logger();
    protected:
        const char *mMessage;
};

#define LOG()                                   \
    Logger localLoggerObject ## __LINE__ (__PRETTY_FUNCTION__)

// octypes.h
#ifndef OC_SECURITY_DB_DAT_FILE_NAME
#define OC_SECURITY_DB_DAT_FILE_NAME    "oic_svr_db.dat"
#endif

#endif /* COMMON_H_ */
