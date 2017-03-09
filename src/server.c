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
int platform_getValue();

OCStackResult createGeolocationResource();


OCRepPayload *updatePayload(OCRepPayload* payload)
{
    LOGf("%p", payload);
    if (!payload)
    {
        exit(1);
    }
    OCRepPayloadSetPropInt(payload, gResource.name, gResource.value);

    LOGf("%ld", gResource.value);
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

    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        LOGf("%d", entityHandlerRequest->method);
        OCRepPayload* input = NULL;
        switch (entityHandlerRequest->method)
        {
        case OC_REST_GET:
            payload = OCRepPayloadCreate();
            OCRepPayloadSetUri(payload, gUri);
            payload = updatePayload(payload);
            break;
        default:
            break;
        }
        if (payload == 0 ) 
            payload = OCRepPayloadCreate();
    
        //if (!payload)
        //{
        //  LOGf("%p (error)", payload);
        //  return OC_EH_ERROR;
        //}

        // updatePayload(payload);
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
    OCStackResult result = OCCreateResource(&(gResource.handle),
                                            gResourceTypeName,
                                            gResourceInterfaceName,
                                            gUri,
                                            onOCEntity,
                                            NULL,
                                            OC_DISCOVERABLE|OC_OBSERVABLE);
    LOGf("%d", result);
    return result;
}


OCStackResult server_loop()
{
    OCStackResult result = OC_STACK_ERROR;
    {
        gResource.value = platform_getValue();

        OCRepPayload* payload = NULL;
        payload = OCRepPayloadCreate();
        updatePayload(payload);
        
        OCNotifyAllObservers(gResource.handle, gQos);
        OCRepPayloadDestroy(payload);
    }

    result = OCProcess();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }
    sleep(gDelay);
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
