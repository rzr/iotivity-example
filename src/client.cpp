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
    m_GETCallback = bind(&Resource::onGet, this,
                         placeholders::_1, placeholders::_2, placeholders::_3);
}

Resource::~Resource()
{
}

// TODO: overide with your business logic
void IoTClient::handle(const HeaderOptions headerOptions, const OCRepresentation &rep,
                       const int &eCode, const int &sequenceNumber)
{
    LOG();
    double latitude = 0;
    double longitude = 0;

    if (rep.hasAttribute("latitude"))
    {
        latitude = rep.getValue<double>("latitude");
    }
    if (rep.hasAttribute("longitude"))
    {
        longitude = rep.getValue<double>("longitude");
    }
    Platform::setValue(latitude,longitude);
    cout << "location: " << latitude << ", " << longitude << std::endl;
}



void Resource::onGet(const HeaderOptions &headerOptions,
                     const OCRepresentation &representation, int eCode)
{
    LOG();
    if (eCode < OC_STACK_INVALID_URI)
    {
        IoTClient::handle(headerOptions, representation, eCode, 0);
    }
    else
    {
        Common::log( __PRETTY_FUNCTION__);
        Common::log("errror:: in GET response");
        cerr << "errror:: in GET response:" << eCode << endl;
    }
}


void Resource::get()
{
    LOG();
    QueryParamsMap params;
    m_OCResource->get(params, m_GETCallback);
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
            }

        }
    }
    catch (OCException &ex)
    {
        cerr << "error: Caught exception in discoveredResource: " << ex.reason() << endl;
        Common::log( "error: Caught exception in discoveredResource");
        Common::log(__PRETTY_FUNCTION__ );
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

int IoTClient::main(int argc, char *argv[])
{
    IoTClient::getInstance()->start();
    int delay = 5;

    while (delay >= 0)
    {
        sleep(delay);
        shared_ptr<Resource> resource = IoTClient::getInstance()->getResource();
        if (resource) resource->get();;

    }
    return 0;
}


#ifdef CONFIG_CLIENT_MAIN
int main(int argc, char *argv[])
{
    return IoTClient::main(argc, argv);
}
#endif
