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

#include <csignal>
#include <functional>

#include "server.h"
#include "platform.h"

using namespace std;
using namespace OC;


bool IoTServer::m_over = false;

double IoTServer::m_co = 0;
double IoTServer::m_co2 = 0;
double IoTServer::m_offset = 0.001;

IoTServer::IoTServer(string endpoint)
{
    LOG();
    Common::m_endpoint = endpoint;
    init();
    setup();
}


IoTServer::~IoTServer()
{
    LOG();
}


void IoTServer::init()
{
    LOG();

    m_platformConfig = make_shared<PlatformConfig>
                       (ServiceType::InProc, // different service ?
                        ModeType::Server, // other is Client or Both
                        "0.0.0.0", // default ip
                        0, // default random port
                        OC::QualityOfService::LowQos// qos
                       );
    OCPlatform::Configure(*m_platformConfig);
}

void IoTServer::setup()
{
    LOG();
    OCStackResult result ;
    EntityHandler handler = bind(&IoTServer::handleEntity, this, placeholders::_1);

    result = createResource(Common::m_endpoint, Common::m_type, handler, m_ResourceHandle);
    if (OC_STACK_OK != result)
    {
        cerr << "error: Error on createResource" << endl;
        throw OC::InitializeException(__PRETTY_FUNCTION__, result);
    }
    result = createResource("/myCOResURI", "oic.r.airquality", handler, m_coResourceHandle);
    if (OC_STACK_OK != result)
    {
        cerr << "error: Error on createResource" << endl;
        throw OC::InitializeException(__PRETTY_FUNCTION__, result);
    }
    result = createResource("/myCO2ResURI", "oic.r.airquality", handler, m_co2ResourceHandle);
    if (OC_STACK_OK != result)
    {
        cerr << "error: Error on createResource" << endl;
        throw OC::InitializeException(__PRETTY_FUNCTION__, result);
    }
}

OCStackResult IoTServer::createResource(string uri, string type, EntityHandler handler,
                                        OCResourceHandle &handle)
{
    LOG();
    OCStackResult result;
    string resourceUri = uri;
    string resourceType = type;
    string resourceInterface = Common::m_interface;
    uint8_t resourceFlag = OC_DISCOVERABLE | OC_OBSERVABLE;
    try
    {
        result = OCPlatform::registerResource//
                 ( handle,
                   resourceUri, resourceType,
                   resourceInterface, //
                   handler, resourceFlag);

        if (result != OC_STACK_OK)
            cerr << "error: Could not create " << type << " resource" << endl;
        else
            cerr << "log: Successfully created " << type << " resource" << endl;
    }
    catch (OC::OCException &e)
    {
        cerr << "error: OCException " <<  e.reason().c_str() << " " << hex << e.code();
        cerr << "error: @" << __PRETTY_FUNCTION__ << endl;
        result = OC_STACK_ERROR;
    }

    return result;
}


OCStackResult IoTServer::respond(std::shared_ptr<OC::OCResourceResponse> response)
{
    OCStackResult result =  OC_STACK_ERROR;
    LOG();

    if (response)
    {
        response->setResponseResult(OC_EH_OK);
        response->setResourceRepresentation(m_Representation);
        result = OCPlatform::sendResponse(response);
    }
    return result;
}


OCEntityHandlerResult IoTServer::handleEntity(shared_ptr<OCResourceRequest> request)
{
    LOG();

    OCEntityHandlerResult result = OC_EH_ERROR;
    if (request)
    {
        string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();
        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            auto response = std::make_shared<OC::OCResourceResponse>();
            response->setRequestHandle(request->getRequestHandle());
            response->setResourceHandle(request->getResourceHandle());
            if (requestType == "GET")
            {
                cerr << "GET request for platform Resource" << endl;
                if (response)
                {
                    response->setResponseResult(OC_EH_OK);
                    response->setResourceRepresentation(m_Representation);
                    if (OCPlatform::sendResponse(response) == OC_STACK_OK)
                    {
                        result = OC_EH_OK;
                    }
                }
            }
            else
            {
                cerr << "error: unsupported " << requestType << endl;
                response->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(response);
            }
        }
    }
    return result;
}


void IoTServer::update()
{
    LOG();
    {
        m_co += m_offset;
        m_co2 += m_offset;

        m_coRepresentation.setValue("contaminantvalue", m_co);
        m_coRepresentation.setValue("contaminanttype", "CO");
        m_co2Representation.setValue("contaminantvalue", m_co2);
        m_co2Representation.setValue("contaminanttype", "CO2");

        cerr << "location: " << std::fixed << m_co << "," << std::fixed << m_co2 << endl;
    }
    OCStackResult result = OCPlatform::notifyAllObservers(m_ResourceHandle);
}


void IoTServer::handle_signal(int signal)
{
    LOG();
    IoTServer::m_over = true;
}


int IoTServer::main(int argc, char *argv[])
{
    LOG();
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = IoTServer::handle_signal;
    sigaction(SIGINT, &sa, nullptr);

    cerr << "log: Server: " << endl
         << "Press Ctrl-C to quit...." << endl
         << "Usage: server -v" << endl
         ;

    int subargc = argc;
    char **subargv = argv;
    for (int i = 1; i < argc; i++)
    {
        if (0 == strcmp("-v", argv[i]))
        {
            Common::m_logLevel++;
            argc--;
            subargv++;
        }
    }

    Platform::getInstance().setup(subargc, subargv);

    IoTServer server;
    try
    {
        int delay = Common::m_period;
        do
        {
            sleep(delay);
        }
        while (!IoTServer::m_over );
    }
    catch (...)
    {
        exit(1);
    }
    return 0;
}

#ifdef CONFIG_SERVER_MAIN
int main(int argc, char *argv[])
{
    return IoTServer::main(argc, argv);
}
#endif
