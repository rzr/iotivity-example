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


static FILE *override_fopen(const char *path, const char *mode)
{
    LOG();
    static const char *CRED_FILE_NAME = "oic_svr_db_anon-clear.dat";
    char const *const filename
        = (0 == strcmp(path, OC_SECURITY_DB_DAT_FILE_NAME)) ? CRED_FILE_NAME : path;
    return fopen(filename, mode);
}


void IoTServer::init()
{
    LOG();

    static OCPersistentStorage ps {override_fopen, fread, fwrite, fclose, unlink };
    m_platformConfig = make_shared<PlatformConfig>
                       (ServiceType::InProc, // different service ?
                        ModeType::Server, // other is Client or Both
                        "0.0.0.0", // default ip
                        0, // default random port
                        OC::QualityOfService::LowQos, // qos
                        &ps // Security credentials
                       );
    OCPlatform::Configure(*m_platformConfig);
}

void IoTServer::setup()
{
    LOG();
    OCStackResult result ;
    EntityHandler handler = bind(&IoTServer::handleEntity, this, placeholders::_1);

    result = createResource(Common::m_endpoint, Common::m_type, handler, m_resourceHandle);
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
    try
    {
        result = OCPlatform::registerResource // resource to be discovered
                 (handle,
                  uri, // Resource URI, CoAP endpoint
                  type, // Resource type (can use oneiota or not)
                  Common::m_interface, //resourceInterface grant CRUD opts
                  handler, // for responding to client requests
                  Common::m_policy // observable or secured ?
                 );
        if (result != OC_STACK_OK)
        {
            cerr << "error: Could not create endpoint:"
                 << uri << " as " << type << endl;
        }
        else
        {
            cerr << "log: Successfully created endpoint: " 
                 << uri << " as " << type << endl;
        }
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
        response->setResourceRepresentation(m_representation);
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
            if (respond(response) == OC_STACK_OK)
            {
                result = OC_EH_OK;
            }
        }
    }
    return result;
}


void IoTServer::update()
{
    LOG();

    static double offset=1;
    Common::m_brightness += offset;
    if (100 < Common::m_brightness)
    {
        Common::m_brightness = 100;
        offset=-offset;
    } else if (0 > Common::m_brightness)
    {
        Common::m_brightness = 0;
        offset=-offset;
    }

    m_representation.setValue("brightness", Common::m_brightness);
    cout << Common::m_type << ": { " << Common::m_brightness << " }" << endl;
    OCStackResult result = OCPlatform::notifyAllObservers(m_resourceHandle);
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

    int delay = Common::m_period;
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
        else
        {
            delay = atoi(argv[i]);
        }
    }

    Platform::getInstance().setup(subargc, subargv);

    IoTServer server;
    try
    {
        do
        {
            server.update();
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
