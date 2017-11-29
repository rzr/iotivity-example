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
    LOG();
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


IoTClient *IoTClient::getInstance()
{
    static IoTClient *pInstance = nullptr;
    if (!pInstance)
    {
        pInstance = new IoTClient;
    }
    return pInstance;
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
    m_findCallback = bind(&IoTClient::onFind, this, placeholders::_1);
}


void IoTClient::start()
{
    LOG();
    string uri = string(OC_RSRVD_WELL_KNOWN_URI);
    OCConnectivityType connectivityType(CT_ADAPTER_IP);
    try
    {
        OCPlatform::findResource("", //
                                 uri.c_str(), // coap_multicast_discovery
                                 connectivityType, // IP, BT, BLE etc
                                 m_findCallback, // callback object
                                 OC::QualityOfService::LowQos // or HIGH
                                );
    }
    catch (OCException &e)
    {
        cerr << "error: Exception: " << e.what();
        exit(1);
    }
}


void IoTClient::onFind(shared_ptr<OCResource> resource)
{
    LOG();
    try
    {
        if (resource)
        {
            print(resource);
            auto resourceUri = resource->uri();
            if (Common::m_endpoint == resourceUri)
            {
                cerr << "resourceUri=" << resourceUri << endl;
                m_resource = make_shared<Resource>(resource);
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
    LOG();
    cerr << "log: Resource: uri: " << resource->uri() << endl;
    cerr << "log: Resource: host: " << resource->host() << endl;

    for (auto &type : resource->getResourceTypes())
    {
        cerr << "log: Resource: type: " << type << endl;
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


shared_ptr<Resource> IoTClient::getResource()
{
    return m_resource;
}


int IoTClient::main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (0 == strcmp("-v", argv[i]))
        {
            Common::m_logLevel++;
        }
    }

    IoTClient::getInstance()->start();

    int choice = 0;
    do
    {
        cin >> choice;
        switch (choice)
        {
            case 9:
                return 0;
            default:
                IoTClient::getInstance()->input();
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
