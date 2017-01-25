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

#include "common.h"

OCStackResult server_setup();
OCStackResult server_loop();
OCStackResult server_finish();

void platform_log(char const *);
void platform_setup();
void platform_loop();
void platform_setValue(int value);
int platform_getValue();

OCStackResult createGeolocationResource();


OCStackResult setValue(int value)
{
    OCStackResult result;
    LOGf("%d", value);
    gGeolocation.value = value;
    platform_setValue( (bool) (gGeolocation.value%2));
    result = OCNotifyAllObservers(gGeolocation.handle, gQos);
    return result;
}


OCRepPayload *updatePayload(OCRepPayload* payload)
{

    LOGf("%p", payload);
    if (!payload)
    {
        exit(1);
    }
    //OCRepPayloadAddResourceType(payload, gIface);
    //OCRepPayloadAddInterface(payload, DEFAULT_INTERFACE);

    LOGf("%ld (payload)", gGeolocation.value);
    OCRepPayloadSetPropInt(payload, "value", gGeolocation.value);
    OCRepPayloadSetPropDouble(payload, "lat", gGeolocation.lat);
    OCRepPayloadSetPropDouble(payload, "lon", gGeolocation.lon);
    OCRepPayloadSetPropString(payload, "line", "TODO");
    OCRepPayloadSetPropInt(payload, "illuminance", gGeolocation.illuminance);

    return payload;
}


OCEntityHandlerResult onOCEntity(OCEntityHandlerFlag flag,
                                 OCEntityHandlerRequest *entityHandlerRequest,
                                 void *callbackParam)
{
    OCEntityHandlerResult result = OC_EH_OK;
    OCStackResult res = OC_STACK_OK;
    OCRepPayload *payload = NULL;
    OCEntityHandlerResponse response = {0};
    memset(&response,0,sizeof response);

    LOGf("%p", entityHandlerRequest);
    LOGf("%ld (current)", gGeolocation.value);

    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        LOGf("%d", entityHandlerRequest->method);
        OCRepPayload* input = NULL;
        switch (entityHandlerRequest->method)
        {
        case OC_REST_GET:
            OCRepPayloadSetUri(payload, gUri);
            payload = OCRepPayloadCreate();
            payload = updatePayload(payload);
            break;
        default:
            break;
        }
        if (payload == 0 ) 
            payload = OCRepPayloadCreate();
    
        if (!payload)
        {
            LOGf("%p (error)", payload);
            return OC_EH_ERROR;
        }
        updatePayload(payload);
        response.payload = (OCPayload *) payload;

        response.ehResult = result;
        response.numSendVendorSpecificHeaderOptions = 0;
        memset(response.sendVendorSpecificHeaderOptions, 0,
               sizeof response.sendVendorSpecificHeaderOptions);

        memset(response.resourceUri, 0, sizeof response.resourceUri);
        response.persistentBufferFlag = 0;
        response.requestHandle = entityHandlerRequest->requestHandle;
        response.resourceHandle = entityHandlerRequest->resource;
        LOGf("%p (note request infos are copied)", response.resourceHandle);

        res = OCDoResponse(&response);
        if (res != OC_STACK_OK)
        {
            LOGf("%d (error)", res);
            result = OC_EH_ERROR;
        }
        OCRepPayloadDestroy(payload);
    }
    return result;
}


OCStackResult createGeolocationResource()
{
    OCStackResult result = OCCreateResource(&(gGeolocation.handle),
                                            gName,
                                            gIface,
                                            gUri,
                                            onOCEntity,
                                            NULL,
                                            OC_DISCOVERABLE|OC_OBSERVABLE);
    LOGf("%s", gIface );
    LOGf("%d", result);
    return result;
}
#include <Arduino.h>


OCStackResult server_loop()
{
    LOGf("%ld (iterate)", gGeolocation.value);
    OCStackResult result = OC_STACK_ERROR;

    if ( gGeolocation.value % 10 == 0)
    {
        static double m_lat = 48.1033; //TODO check double on atmega
        static double m_lon = -1.6725;
        static double m_offset = 0.001;
        static double m_latmax = 49;
        static double m_latmin = 48;

        m_lat += m_offset;
        m_lon += m_offset;

        if (m_lat > m_latmax)
        {
            if (m_offset > 0) { m_offset = - m_offset; }
        }
        else if (m_lat < m_latmin)
        {
            if ( m_offset < 0 ) m_offset = - m_offset;
        }
        gGeolocation.lat = m_lat;
        //    gGeolocation.lon = m_lon;
        gGeolocation.lat++;
        gGeolocation.value++;
        gGeolocation.illuminance = platform_getValue();

        OCRepPayload* payload = NULL;
        payload = OCRepPayloadCreate();
        updatePayload(payload);

        result = OCNotifyAllObservers(gGeolocation.handle, gQos);
        //postResourceRepresentation();
        OCRepPayloadDestroy(payload);

    }

    result = OCProcess();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }
//#ifdef ARDUINO
//    delay(100);
//#else
    sleep(gDelay);
//#endif
    //static int count=0; if (++count > 20) { gOver = true;}

    return result;
}


OCStackResult server_setup()
{
    OCStackResult result;
    result = OCInit(NULL, 0, OC_SERVER);
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }

    result = createGeolocationResource();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }

    LOGf("%d", result);
    return result;
}


OCStackResult server_finish()
{
    OCStackResult result = OCStop();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
    }
    return result;
}


int server_main(int argc, char* argv[])
{
    OCStackResult result;    
    if (argc>1 && (0 == strcmp("-v", argv[1])))
    {
        gVerbose++;
    }
    result = server_setup();

    if (result != 0)
    {
        return result;
    }

    while (!gOver)
    {
        result = server_loop();
    }

    server_finish();

    return (int) result ;
}
