// -*-c++-*-
//******************************************************************
//
// Copyright 2014 Intel Corporation.
// Copyright 2015 Eurogiciel <philippe.coval@eurogiciel.fr>
// Copyright 2016 Samsung <philippe.coval@osg.samsung.com>
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

#include "common.h"
#include "platform.h"
#include <iostream>

#ifdef __TIZEN__
#include <dlog.h>
extern void printlog(char const *const message);
extern void handleValue(bool value);
#else
#define dlog_print(type,tag,message) \
    printf(message)
extern void printlog(char const *const message)
{ printf("%s", message); }
void handleValue(bool value) {}
#endif

using namespace std;


Platform::~Platform()
{
    LOG();
}


void Platform::setup(int argc, char *argv[])
{
    LOG();
    log(__PRETTY_FUNCTION__);
}


void Platform::log(char const *const message)
{
    LOG();
    cout<<"geolocation: "<<lat<<","<<lon<<endl;
    char const *const LOGTAG = "LOG";
#ifdef __TIZEN__
    dlog_print(DLOG_INFO, LOGTAG, message);
#endif
    printlog(message);
}
