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
    /** key (used both sides) **/
    static std::string  m_propname;
    /** network interface**/
    static std::string m_link;
    /** polling period**/
    static const int m_period = 5;

    static void log(char const * const message);
};

extern void printlog(char const * const message);

#define STR_(x) #x
#define STR(x) STR_(x)

#include <iostream>

class Logger
{
    const char* mMessage;
public:

    Logger(const char * message);
    ~Logger();
};

#define LOG()                                   \
    Logger localLoggerObject ## __LINE__ (__PRETTY_FUNCTION__)


//TODO: << " @"<< __FILE__<<":"<<__LINE__<<":"<<
#endif /* COMMON_H_ */
