// -*-c-*-
//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ocstack.h>
#include <ocpayload.h>

#define STR(a) #a

#ifdef __linux__
#define LOGf(f,p)                                                       \
    printf("%s: %s: %s=" f "\n", __FILE__,  __FUNCTION__,STR(p), p)
#else
#define LOGf(f,p)                               \
    platform_log(__FUNCTION__)
#endif

static int gOver = 0;

static const char * gName = "oic.r.switch.binary";

static const char * gUri = "/switch";

static char const * gIface = //
    OC_RSRVD_INTERFACE_DEFAULT //"oic.if.baseline"
    ;

static OCDevAddr gDestination;

static OCConnectivityType gConnectivityType =//
    CT_ADAPTER_IP//    //CT_DEFAULT
    ;
static OCQualityOfService gQos = OC_HIGH_QOS;


typedef struct SWITCHRESOURCE
{
    OCResourceHandle handle;
    bool value;
} SwitchResource;

static SwitchResource gSwitch = {NULL, false};


static const int gDelay = 1;
