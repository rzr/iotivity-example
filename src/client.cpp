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
#include <unistd.h>
#include <ctime>
#include "client.h"
#include "platform.h"

using namespace std;
using namespace OC;


Resource::Resource(shared_ptr<OCResource> resource)
{
    LOG();
    m_OCResource = resource;
}

Resource::~Resource()
{
}


void Resource::onGet(const HeaderOptions &headerOptions,
                     const OCRepresentation &representation, int eCode)
{
    LOG();
    if (eCode < OC_STACK_INVALID_URI)
    {
        bool value;
        representation.getValue(Common::m_propname, value);
        cout << value << endl;
    }
    else
    {
        cerr << "errror:: in GET response:" << eCode << endl;
    }
    IoTClient::menu();
}

void Resource::onPost(const HeaderOptions &headerOptions,
                      const OCRepresentation &representation, int eCode)
{
    LOG();
    if (eCode < OC_STACK_INVALID_URI)
    {
        bool value;
        representation.getValue(Common::m_propname, value);
        if (true) {
            std::cout<<value<<std::endl;
        } else {
            Platform::getInstance().setValue(value); //dont clash with server
        }
    } 
    else
    {
        cerr << "error: in POST response: " << eCode <<  endl;
    }
    IoTClient::menu();
}


void Resource::get()
{
    LOG();
    QueryParamsMap params;
    m_OCResource->get(params, m_GETCallback);
}

void Resource::post(bool value)
{
    LOG();
    QueryParamsMap params;
    OCRepresentation rep;
    rep.setValue(Common::m_propname, value);
    m_OCResource->post(rep, params, m_POSTCallback);
}


IoTClient::IoTClient()
{
    LOG();
    init();
}

IoTClient::~IoTClient()
{
    LOG();
}


IoTClient *IoTClient::mInstance = nullptr;

IoTClient *IoTClient::getInstance()
{
    if (!IoTClient::mInstance)
    {
        mInstance = new IoTClient;
    }
    return mInstance;
}

void IoTClient::init()
{
    LOG();

    m_platformConfig = make_shared<PlatformConfig>
                       (ServiceType::InProc, //
                        ModeType::Client, //
                        "0.0.0.0", //
                        0, //
                        OC::QualityOfService::LowQos //
                       );
    OCPlatform::Configure(*m_platformConfig);
    m_FindCallback = bind(&IoTClient::onFind, this, placeholders::_1);
}


void IoTClient::start()
{
    LOG();
    string coap_multicast_discovery = string(OC_RSRVD_WELL_KNOWN_URI);

    OCConnectivityType connectivityType(CT_ADAPTER_IP);
    OCPlatform::findResource("", //
                             coap_multicast_discovery.c_str(),
                             connectivityType,
                             m_FindCallback,
                             OC::QualityOfService::LowQos);
}


shared_ptr<Resource> IoTClient::getResource()
{
    return m_Resource;
}


void IoTClient::onFind(shared_ptr<OCResource> resource)
{
    LOG();
    try
    {
        if (resource)
        {
            print(resource);

            string resourceUri = resource->uri();
            if (Common::m_endpoint == resourceUri)
            {
                cerr << "resourceUri=" << resourceUri << endl;
                m_Resource = make_shared<Resource>(resource);

                input();
            }

        }
    }
    catch (OCException &ex)
    {
        cerr << "error: Caught exception in discoveredResource: " << ex.reason() << endl;
    }
}


void IoTClient::print(shared_ptr<OCResource> resource)
{
    cerr << "log: Resource: uri: " << resource->uri() << endl;
    cerr << "log: Resource: host: " << resource->host() << endl;

    for (auto &type : resource->getResourceTypes())
    {
        cerr << "log: Resource: type: " << type << endl << endl;
    }

    for (auto &interface : resource->getResourceInterfaces())
    {
        cerr << "log: Resource: interface: " << interface << endl;
    }
}


void IoTClient::input()
{
    cerr << endl << "menu: "
         << "  9) Quit"
         << "  *) Display this menu"
         << endl;
}


<<<<<<< HEAD
=======
bool IoTClient::toggle()
{
    Common::log(__PRETTY_FUNCTION__);

    bool value = !m_value;
    Common::log(value ? "1" : "0");
    if (m_Resource)
    {
        m_Resource->post(value);
    }
    else
    {
        cerr << "log: resource not yet discovered" << endl;
        Common::log("log: resource not yet discovered");
    }

    return m_value;
}


bool IoTClient::setValue(bool value)
{
    Common::log(__PRETTY_FUNCTION__);

    if (m_Resource)
    {
        m_Resource->post(value);
    }
    else
    {
        cerr << "log: resource not yet discovered" << endl;
        Common::log("log: resource not yet discovered");
    }

    return m_value;
}


>>>>>>> 6bddd0f... WIP: mod: src/client.cpp (master)
int IoTClient::main(int argc, char *argv[])
{
    IoTClient::getInstance()->start();

    for (int i = 1; i < argc; i++)
    {
        if (0 == strcmp("-v", argv[i]))
        {
            Common::m_logLevel++;
        }
    }

    int choice;
    do
    {
        cin >> choice;
        switch (choice)
        {
            case 9:
                return 0;
            default:
                IoTClient::input();
                break;
        }
    }
    while (choice != 9);
    return 0;
}


#ifdef CONFIG_CLIENT_MAIN
int main(int argc, char *argv[])
{
    return IoTClient::main(argc, argv);
}
#endif
