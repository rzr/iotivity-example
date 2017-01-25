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
#include <stdlib.h>
#include <string.h>
#include <octypes.h>
#include <ocstack.h>
#include <ocpayload.h>

#ifdef __linux__
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H
#endif
#define HAVE_SIGNAL_H
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/ioctl.h>
#include <termios.h>

#include "common.h"


OCStackResult client_setup();
OCStackResult post();


unsigned int gDiscovered = 0;
static OCDevAddr gDestination;
int gObversable= 1;

#if 0
OCRepPayload *createPayload()
{
    OCRepPayload *payload = OCRepPayloadCreate();

    LOGf("%p", payload);
    if (!payload)
    {
        exit(1);
    }

    LOGf("%d (changing)", gGeolocation.value);
    OCRepPayloadSetPropBool(payload, "value", gGeolocation.value);

    LOGf("%d", gGeolocation.value);
    return payload;
}
#endif


OCStackApplicationResult handleResponse(void *ctx,
                                        OCDoHandle handle,
                                        OCClientResponse *clientResponse)
{
    LOGf("%d {", gGeolocation.value);
    OCStackApplicationResult result = OC_STACK_DELETE_TRANSACTION;

    if (!clientResponse)
    {
        LOGf("%p (error)", clientResponse);
        return result;
    }
    OCRepPayload *payload = (OCRepPayload *)(clientResponse->payload);
    if (!payload)
    {
        LOGf("%p (error)", payload);
        return result;
    }

    if (!OCRepPayloadGetPropInt(payload, "value", &gGeolocation.value))
    {
        LOGf("%d (error)", gGeolocation.value);
    }

    printf("%d\n", gGeolocation.value);

    LOGf("%d }", gGeolocation.value);
    return OC_STACK_DELETE_TRANSACTION;
}


#if 0

OCStackApplicationResult onGet(void *ctx,
                               OCDoHandle handle,
                               OCClientResponse *clientResponse)
{
    LOGf("%d {", gGeolocation.value);
    OCStackApplicationResult result = OC_STACK_KEEP_TRANSACTION;

    LOGf("%p", clientResponse);
    if (!true)
        result = handleResponse(ctx, handle, clientResponse);
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
    }
    LOGf("%d }", gGeolocation.value);
    return OC_STACK_DELETE_TRANSACTION;
}


OCStackResult get()
{
    LOGf("%d {", gGeolocation.value);
    OCStackResult result = OC_STACK_OK;
    OCMethod method = OC_REST_GET;
    OCRepPayload *payload = NULL;
    OCCallbackData callback = {NULL, NULL, NULL};
    callback.cb = onGet;

    result = OCDoResource(&gGeolocation.handle, method, gUri, &gDestination,
                          (OCPayload *) payload,
                          gConnectivityType, gQos, &callback, NULL, 0);

    if (result != OC_STACK_OK)
    {
        LOGf("%d", result);
    }
    LOGf("%d }", gGeolocation.value);
    return result;
}


OCStackApplicationResult onPost(void *ctx,
                                OCDoHandle handle,
                                OCClientResponse *clientResponse)
{
    LOGf("%d {", gGeolocation.value);
    OCStackApplicationResult result = OC_STACK_KEEP_TRANSACTION;

    LOGf("%p", clientResponse);
    result = handleResponse(ctx, handle, clientResponse);

    LOGf("%d", result);
    LOGf("%d }", gGeolocation.value);
    return OC_STACK_DELETE_TRANSACTION;
}


OCStackResult post()
{
    LOGf("%d {", gGeolocation.value);
    OCStackResult result = OC_STACK_OK;
    OCMethod method = OC_REST_POST;
    OCRepPayload *payload = NULL;
    OCCallbackData callback = {NULL, NULL, NULL};
    callback.cb = onPost;

    LOGf("%d", gGeolocation.value);
    gGeolocation.value = !gGeolocation.value;
    payload = createPayload();

    result = OCDoResource(&gGeolocation.handle, method, gUri, &gDestination,
                          (OCPayload *) payload,
                          gConnectivityType, gQos, &callback, NULL, 0);

    if (result != OC_STACK_OK)
    {
        LOGf("%d", result);
    }
    LOGf("%d }", gGeolocation.value);
    return result;
}
#endif

OCStackApplicationResult onObserve(void* ctx, 
                                       OCDoHandle handle,
                                       OCClientResponse * clientResponse)
{
    LOGf("%d {", gGeolocation.value);
    OCStackApplicationResult result = OC_STACK_KEEP_TRANSACTION;

    LOGf("%p", clientResponse);
    result = handleResponse(ctx, handle, clientResponse);

    LOGf("%d", result);
    LOGf("%d }", gGeolocation.value);
    return OC_STACK_KEEP_TRANSACTION;
}

// This is a function called back when a device is discovered
OCStackApplicationResult onDiscover(void *ctx,
                                    OCDoHandle handle,
                                    OCClientResponse *clientResponse)
{
    OCStackResult result = OC_STACK_OK;

    LOGf("%p", ctx);
    LOGf("%p", clientResponse);

    if (!clientResponse)
    {
        return OC_STACK_DELETE_TRANSACTION;
    }

    LOGf("%p", clientResponse->devAddr.addr);
    LOGf("%d", clientResponse->sequenceNumber);
    LOGf("%p", clientResponse->payload);
    OCDiscoveryPayload *payload = (OCDiscoveryPayload *) clientResponse->payload;
    LOGf("%p", payload);
    gDiscovered++;
    if (!payload)
    {
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCResourcePayload *resource = (OCResourcePayload *) payload->resources;

    while (resource)
    {
        LOGf("%p", resource);
        if (resource->uri)
        {
            LOGf("%s", resource->uri);
            if (0 == strcmp(gUri, resource->uri))
            {
                gDestination = clientResponse->devAddr;
                LOGf("%s", gDestination.addr);
                gConnectivityType = clientResponse->connType;
                gGeolocation.handle = handle;
                if (gObversable)
                {
                    OCCallbackData callback = {NULL, NULL, NULL};
                    callback.cb = onObserve;
                    OCStackResult ret;
                    ret = OCDoResource(&gGeolocation.handle, OC_REST_OBSERVE,
                                       gUri, &gDestination, NULL,
                                       gConnectivityType, gQos, &callback, NULL, 0);
                }
            }
        }
        resource = resource->next;
    }

    return OC_STACK_KEEP_TRANSACTION;
}



OCStackResult client_loop()
{
    OCStackResult result;
    LOGf("%d (iterate)", gGeolocation.value);

    result = OCProcess();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }
#if 0 
    static int once = 1;
    if (gDiscovered && once-- > 0)
    {
        sleep(4 * gDelay);
        result = post();
        LOGf("%d (post)", result);
    }
    int c = 0;
    if (gDiscovered && ((c = kbhit()) > 0))
    {
        LOGf("%d (post on kbhit)", c);
        result = post();
    }
#endif
    sleep(gDelay);
    LOGf("%d", gOver);
    return result;
}


OCStackResult client_setup()
{
    int i = 0;
    OCStackResult result;
    static int gInit = 0;
    if (gInit++ == 0)
    {
        result = OCInit1(OC_CLIENT, // or OC_CLIENT_SERVER,
                         OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS);

        if (result != OC_STACK_OK)
        {
            LOGf("%d (error)", result);
            return result;
        }
    }

    OCCallbackData cbData = {NULL, NULL, NULL};
    cbData.cb = onDiscover;
    LOGf("%p", cbData.cb);
    char queryUri[MAX_QUERY_LENGTH] = { 0 };
    snprintf(queryUri, sizeof (queryUri), "%s", OC_RSRVD_WELL_KNOWN_URI);
    LOGf("%s", queryUri);

    for (i = 0; !gDiscovered && i < 2; i++)
    {
        LOGf("%d", gDiscovered);

        result = OCDoResource(NULL, // handle
                              OC_REST_DISCOVER, // method
                              queryUri, //requestUri: /oic/res
                              NULL, // destination
                              NULL,  // opayload
                              gConnectivityType, //
                              gQos, // OC_LOW_QOS
                              &cbData, //
                              NULL, // options
                              0 // numOptions
                             );

        if (result != OC_STACK_OK)
        {
            LOGf("%d (error)", result);
        }

        sleep(gDelay/2);
    }
    LOGf("%d", result);
    return result;
}


void client_finish()
{
    OCStackResult result = OCStop();

    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
    }
}


/* SIGINT handler: set gOver to 1 for graceful termination */
void onSignal(int signum)
{
    if (signum == SIGINT)
    {
        gOver = 1;
    }
}


int client_main(int argc, char *argv[])
{
    if (argc>1 && (0 == strcmp("-v", argv[1])))
    {
        gVerbose++;
    }
    LOGf("%d", gVerbose);
    LOGf("%s", "Break from loop with Ctrl+C");
    signal(SIGINT, onSignal);

    client_setup();

    for (; !gOver;)
    {
        client_loop();
    }
    client_finish();
    return 0;
}


int main(int argc, char *argv[])
{
    return client_main(argc, argv);
}
