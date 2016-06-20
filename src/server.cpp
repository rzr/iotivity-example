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
#include "config.h"

#include <csignal>
#include <functional>

#include "server.h"
#include "sensors.h"

using namespace Sensors;
using namespace std;
using namespace OC;

bool IoTServer::m_over = false;

IoTServer::IoTServer(int pin, string key)
{
    cerr << __PRETTY_FUNCTION__ << endl;
    init();
    setup();
    m_Representation.setValue(key, 0);
    SetupPins(pin);
}


IoTServer::~IoTServer()
{
    cerr << __PRETTY_FUNCTION__ << endl;
    ClosePins();
}


void IoTServer::init()
{
    cerr << __PRETTY_FUNCTION__ << endl;

    m_platformConfig = make_shared<PlatformConfig>
                       (ServiceType::InProc, // different services ?
                        ModeType::Server, // other is client
                        "0.0.0.0", // default ip
                        0, // default port ?
                        OC::QualityOfService::HighQos// qos
                       );

    OCPlatform::Configure(*m_platformConfig);
}


void IoTServer::setup()
{
    cerr << __PRETTY_FUNCTION__ << endl;
    OCStackResult result ;
    EntityHandler handler = bind(&IoTServer::handleEntity, this, placeholders::_1);

    result = createResource(Config::m_endpoint, Config::m_type, handler, m_Resource);
    if (OC_STACK_OK != result)
    {
        cerr << "Error on createResource" << endl;
        throw OC::InitializeException(__PRETTY_FUNCTION__, result);
    }

    result = OCPlatform::bindTypeToResource(m_Resource, Config::m_type);
    if (OC_STACK_OK != result)
    {
        cerr << "Binding TypeName to Resource was unsuccessful" << endl;
        throw OC::InitializeException(__PRETTY_FUNCTION__, result);
    }

    result = OCPlatform::bindInterfaceToResource(m_Resource, Config::m_link);
    if (OC_STACK_OK != result)
    {
        cerr << "Binding TypeName to Resource was unsuccessful" << endl;
        throw OC::InitializeException(__PRETTY_FUNCTION__, result);
    }

}


OCStackResult IoTServer::createResource(string uri, string type, EntityHandler handler,
                                        OCResourceHandle &handle)
{
    cerr << __PRETTY_FUNCTION__ << endl;
    OCStackResult result;
    string resourceUri = uri;
    string resourceType = type;
    string resourceInterface = Config::m_interface;
    uint8_t resourceFlag = OC_DISCOVERABLE | OC_OBSERVABLE;
    try
    {
        result = OCPlatform::registerResource//
                 ( handle,
                   resourceUri, resourceType,
                   resourceInterface, //
                   handler, resourceFlag);

        if (result != OC_STACK_OK)
            cerr << "Could not create " << type << " resource" << endl;
        else
            cerr << "Successfully created " << type << " resource" << endl;
    }

    catch (OC::OCException &e)
    {
        cerr << "OCException " <<  e.reason().c_str() << " " << hex << e.code();
        cerr << "@" << __PRETTY_FUNCTION__ << endl;
        result = OC_STACK_ERROR;
    }

    return result;
}


void IoTServer::putResourceRepresentation()
{
    cerr << __PRETTY_FUNCTION__ << endl;
    int state = 0;
    m_Representation.getValue(Config::m_key, state);
    SetOnboardLed(state);
    OCStackResult result = OCPlatform::notifyAllObservers(m_Resource);
    if (state == 0)
        cout << "Turned off GPIO" << endl;
    else if (state == 1)
        cout << "Turned on GPIO" << endl;
    else
        cerr << "Invalid request value" << endl;
}


OCRepresentation IoTServer::getResourceRepresentation()
{
    cerr << __PRETTY_FUNCTION__ << endl;
    return m_Representation;
}


OCEntityHandlerResult IoTServer::handleEntity(shared_ptr<OCResourceRequest> Request)
{
    cerr << __PRETTY_FUNCTION__ << endl;

    OCEntityHandlerResult result = OC_EH_ERROR;
    if (Request)
    {
        string requestType = Request->getRequestType();
        int requestFlag = Request->getRequestHandlerFlag();
        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            auto Response = std::make_shared<OC::OCResourceResponse>();
            Response->setRequestHandle(Request->getRequestHandle());
            Response->setResourceHandle(Request->getResourceHandle());
            if (requestType == "PUT")
            {
                cerr << "PUT request for platform Resource" << endl;
                OCRepresentation requestRep = Request->getResourceRepresentation();
                if (requestRep.hasAttribute(Config::m_key))
                {
                    try
                    {
                        requestRep.getValue<int>(Config::m_key);
                    }
                    catch (...)
                    {
                        Response->setResponseResult(OC_EH_ERROR);
                        OCPlatform::sendResponse(Response);
                        cerr << "Client sent invalid resource value type" << endl;
                        return result;
                    }
                }
                else
                {
                    Response->setResponseResult(OC_EH_ERROR);
                    OCPlatform::sendResponse(Response);
                    cerr << "Client sent invalid resource key" << endl;
                    return result;
                }
                m_Representation = requestRep;
                putResourceRepresentation();
                if (Response)
                {
                    Response->setErrorCode(200);
                    Response->setResourceRepresentation(getResourceRepresentation());
                    Response->setResponseResult(OC_EH_OK);
                    if (OCPlatform::sendResponse(Response) == OC_STACK_OK)
                    {
                        result = OC_EH_OK;
                    }
                }
            }
            else if (requestType == "GET")
            {
                cerr << "GET request for platform Resource" << endl;
                if (Response)
                {
                    Response->setErrorCode(200);
                    Response->setResponseResult(OC_EH_OK);
                    Response->setResourceRepresentation(getResourceRepresentation());
                    if (OCPlatform::sendResponse(Response) == OC_STACK_OK)
                    {
                        result = OC_EH_OK;
                    }
                }
            }
            else
            {
                Response->setResponseResult(OC_EH_ERROR);
                OCPlatform::sendResponse(Response);
                cerr << "Unsupported request type" << endl;
            }
        }
    }
    return result;
}


void IoTServer::handle_signal(int signal)
{
    cerr << __PRETTY_FUNCTION__ << endl;
    IoTServer::m_over = true;
}


int IoTServer::main(int argc, char *argv[])
{
    cerr << __PRETTY_FUNCTION__ << endl;
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = IoTServer::handle_signal;
    sigaction(SIGINT, &sa, nullptr);

    cerr << "Server: " << endl
         << "Press Ctrl-C to quit...." << endl;

    if (argc > 1 && argv[1])
    {
        Config::m_gpio  = atoi(argv[1]);
    }

    IoTServer server(Config::m_gpio, Config::m_key);
    try
    {

        do
        {
            usleep(2000000);
        }
        while (!IoTServer::m_over );
    }
    catch (...)
    {
        exit(1);
    }
    return 0;
}

#if 1
int main(int argc, char *argv[])
{
    return IoTServer::main(argc, argv);
}
#endif
