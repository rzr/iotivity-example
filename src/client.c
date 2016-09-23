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

#include <fcntl.h>

#include "common.h"

int gInit=0;
int gDiscovered=0;

OCStackResult client_setup();
OCStackResult post();

OCStackApplicationResult handleResponse(void *ctx,
                                        OCDoHandle handle,
                                        OCClientResponse *clientResponse)
{
    printf("%s: { %d\n", __FUNCTION__, gSwitch.value);
    OCStackApplicationResult result= OC_STACK_DELETE_TRANSACTION;

    if (!clientResponse)
    {
        return result;
    }
    OCRepPayload* payload = (OCRepPayload*)(clientResponse->payload);
    if (!payload)
    {
        return result;
    }

    if (!OCRepPayloadGetPropBool(payload, "value", &gSwitch.value))
    {
        printf("%s: error: %d\n", __FUNCTION__, __LINE__);
    }


    printf("%s: } %d\n", __FUNCTION__, gSwitch.value);

    return OC_STACK_DELETE_TRANSACTION;
}


OCStackApplicationResult handleGet(void *ctx,
                                   OCDoHandle handle,
                                   OCClientResponse *clientResponse)
{
    LOGf("{ %d", gSwitch.value);
    OCStackApplicationResult result = OC_STACK_KEEP_TRANSACTION;

    result = handleResponse(ctx, handle, clientResponse);
    LOGf("%p", clientResponse);
    LOGf("%d", result);

    result = post();

    LOGf("} %d", gSwitch.value);
    return OC_STACK_DELETE_TRANSACTION;
}


OCStackApplicationResult handlePost(void *ctx,
                                    OCDoHandle handle,
                                    OCClientResponse *clientResponse)
{
    LOGf("{ %d", gSwitch.value);
    OCStackApplicationResult result = OC_STACK_KEEP_TRANSACTION;

    result = handleResponse(ctx, handle, clientResponse);

    LOGf("%p", clientResponse);
    LOGf("%d", result);
    LOGf("} %d", gSwitch.value);
    return OC_STACK_DELETE_TRANSACTION;
}


OCRepPayload* createPayload()
{
    OCRepPayload* payload = OCRepPayloadCreate();

    printf("%s: %s=%p\n", __FUNCTION__, STR(payload), payload);
    if (!payload)
    {
        exit(1);
    }

    LOGf("changing: from %d",gSwitch.value);
    OCRepPayloadSetPropBool(payload, "value", !gSwitch.value); //TODO
    LOGf("%d",gSwitch.value);

    return payload;
}

// This is a function called back when a device is discovered
OCStackApplicationResult handleDiscover(void *ctx,
                                        OCDoHandle handle,
                                        OCClientResponse *clientResponse)
{
    OCStackResult result = OC_STACK_OK;

    printf("%s: %s=%p\n", __FUNCTION__, STR(clientResponse), clientResponse);

    if (!clientResponse)
    {
        return OC_STACK_DELETE_TRANSACTION;
    }

    printf("%s: %s=%p\n", __FUNCTION__, STR(clientResponse->devAddr.addr), clientResponse->devAddr.addr);

    LOGf("%d", clientResponse->sequenceNumber);

    gDestination = clientResponse->devAddr;
    gConnectivityType = clientResponse->connType;


    LOGf("%p", clientResponse->payload);
    OCDiscoveryPayload *payload = (OCDiscoveryPayload*) clientResponse->payload;
    LOGf("%p", payload);
    if (!payload)
    {
        return OC_STACK_DELETE_TRANSACTION;
    }

    OCResourcePayload *resource = (OCResourcePayload*) payload->resources;

    while (resource)
    {
        LOGf("%p", resource);
        if(resource->uri)
        {
            LOGf("%s", resource->uri); //segfault w/ 1.1.1
        }
        resource = resource->next;
    }

    gSwitch.handle = handle;

    gDiscovered++;

    return OC_STACK_KEEP_TRANSACTION;
}


OCStackResult get()
{
    LOGf("{ %d", gSwitch.value);
    OCStackResult result = OC_STACK_OK;
    OCMethod method = OC_REST_GET;
    OCCallbackData getCallback = { NULL, NULL, NULL };
    getCallback.cb = handleGet;
    OCRepPayload* payload = NULL;

    result = OCDoResource(&gSwitch.handle, method, gUri, &gDestination,
                          (OCPayload*) payload,
                          gConnectivityType, gQos, &getCallback, NULL,0);

    LOGf("} %d", gSwitch.value);
    return result;
}

OCStackResult post()
{
    LOGf("{ %d", gSwitch.value);
    OCStackResult result = OC_STACK_OK;
    OCMethod method = OC_REST_POST;
    OCRepPayload* payload;
    OCCallbackData postCallback = {NULL, NULL, NULL};
    postCallback.cb = handlePost;

    printf("%s: %d\n", __FUNCTION__, gSwitch.value);
    payload = createPayload();

    result = OCDoResource(&gSwitch.handle, method, gUri, &gDestination,
                          (OCPayload*) payload,
                          gConnectivityType, gQos, &postCallback, NULL,0);

    if (result != OC_STACK_OK)
    {
        LOGf("%d", method);
        LOGf("%d", result);
    }

    return result;
    LOGf("} %d", gSwitch.value);
}

OCStackResult client_loop()
{
    OCStackResult result;

    printf("iterate: %d\n", gSwitch.value);

    result = OCProcess();
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
        return result;
    }
    static int once = 1;
    if (gDiscovered && once-->0) {
        result = get();
    }

    {
        char buf;
        fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
        int numRead = read(0,&buf,1);
        if(numRead > 0){
            get();
        }
    }

    //if (kbhit()) get();

    sleep(gDelay);
    LOGf("%d", gOver);
    return result;
}


OCStackResult client_setup()
{
    int i=0;
    OCStackResult result;
    static int gInit=0;
    if (gInit++ == 0)
    {
        //result = OCInit(NULL, 0, OC_CLIENT);
        result = OCInit1(OC_CLIENT, OC_DEFAULT_FLAGS, OC_DEFAULT_FLAGS);

        if (result != OC_STACK_OK)
	{
            LOGf("%d (error)", result);
            return result;
	}
    }

    OCCallbackData cbData = {NULL, NULL, NULL };
    cbData.cb = handleDiscover;

    char queryUri[MAX_QUERY_LENGTH] = { 0 };
    snprintf(queryUri, sizeof (queryUri), "%s", OC_RSRVD_WELL_KNOWN_URI);
    //
    LOGf("%s", queryUri);

    //snprintf(queryUri, sizeof (queryUri), "%s", gUri);
    //LOGf("%s", queryUri);


    for (i=0; i<2; i++) {
        result = OCDoResource(NULL, // handle
                              OC_REST_DISCOVER, // method
                              queryUri, //requestUri: /oic/res
                              NULL, // destination
                              NULL,  // opayload
                              gConnectivityType, //
                              gQos, //
                              &cbData, //
                              NULL, // options
                              0 // numOptions
            );
        sleep(1);
    }
    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
    }

    LOGf("%d", result);
    return result;
}


void finish()
{
    OCStackResult result = OCStop();

    if (result != OC_STACK_OK)
    {
        LOGf("%d (error)", result);
    }
}

/* SIGINT handler: set gOver to 1 for graceful termination */
void handleSigInt(int signum)
{
    if (signum == SIGINT)
    {
        gOver = 1;
    }
}


int main(int argc, char* argv[])
{
    client_setup();

    sleep(gDelay);

    // Break from loop with Ctrl+C
    signal(SIGINT, handleSigInt);

    for(;!gOver;)
    {
        client_loop();
    }
    finish();

    return 0;
}
