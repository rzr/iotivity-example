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
OCStackResult stop();

void platform_log(char const *);
void platform_setup();
void platform_loop();

#ifdef __linux__
#include "server/platform/linux/platform.c"
#elif defined ARDUINO
#include "server/platform/arduino/platform.c"
#else
#include "platform/default/platform.c"
#warning "TODO: platform may not supported"
#endif

OCStackResult createSwitchResource();

void setValue(bool value)
{
    LOGf("%d", value);
    gSwitch.value = value;
    platform_setValue(gSwitch.value);
}

OCEntityHandlerResult handleOCEntity(OCEntityHandlerFlag flag,
                                     OCEntityHandlerRequest *entityHandlerRequest,
                                     void *callbackParam)
{
    OCEntityHandlerResult result = OC_EH_OK;
    OCEntityHandlerResponse response = {0};

    LOGf("%p", entityHandlerRequest);
    LOGf("%d (current)", gSwitch.value);

#if 1
    OCRepPayload *payload = (OCRepPayload *) OCRepPayloadCreate();
    if (!payload)
    {
        LOGf("%p (error)", payload);
        return OC_EH_ERROR;
    }
    
    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        LOGf("%d", entityHandlerRequest->method);
        OCRepPayload* input = NULL;
        switch (entityHandlerRequest->method) {
        case OC_REST_POST: //4
        case OC_REST_PUT:
            
            input = (OCRepPayload*) entityHandlerRequest->payload;
            OCRepPayloadGetPropBool(input, "value", &gSwitch.value);
#if 0 // defined ARDUINO            
            if ( !false ) {
                LOGf("TODO: check %s (workaround)",__FILE__);
                gSwitch.value = !gSwitch.value;
            }
#endif
            LOGf("%d (update)", gSwitch.value);
            setValue(gSwitch.value);

            break;
        case OC_REST_GET: //1
            OCRepPayloadSetUri(payload, gUri);
            OCRepPayloadSetPropBool(payload, "value", gSwitch.value);
            //OCRepPayloadAddResourceType(payload, gIface);
            //OCRepPayloadAddInterface(payload, DEFAULT_INTERFACE);

            break;
        default:
            break;
        }

        //

        response.ehResult = result;

        response.numSendVendorSpecificHeaderOptions = 0;
        memset(response.sendVendorSpecificHeaderOptions, 0,
               sizeof response.sendVendorSpecificHeaderOptions);

        memset(response.resourceUri, 0, sizeof response.resourceUri);

        response.persistentBufferFlag = 0;
        response.payload = (OCPayload *) payload;
        response.requestHandle = entityHandlerRequest->requestHandle;
        response.resourceHandle = entityHandlerRequest->resource;

        LOGf("%p (note request infos are copied)", response.resourceHandle);
        OCStackResult res = OCDoResponse(&response);
        if (res != OC_STACK_OK)
        {
            LOGf("%d (error)", res);
            result = OC_EH_ERROR;
        }
        OCRepPayloadDestroy(payload);
    }
#endif
    return result;
}


OCStackResult createSwitchResource()
{
    OCStackResult result = OCCreateResource(&(gSwitch.handle),
                                            gName,
                                            gIface,
                                            gUri,
                                            handleOCEntity,
                                            NULL,
                                            OC_DISCOVERABLE|OC_OBSERVABLE);
    LOGf("%s", gIface ); //
    LOGf("%d", result);
    return result;
}


OCStackResult server_loop()
{
    printf("iterate: %d\n", gSwitch.value);
    OCStackResult result = OCProcess();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }

    sleep(2*gDelay);
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

    result = createSwitchResource();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }

    LOGf("%d", result);
    return result;
}


OCStackResult stop()
{
    OCStackResult result = OCStop();
    if ( result != OC_STACK_OK)  {
        LOGf("%d (error)", result);
    }
    return result;
}
