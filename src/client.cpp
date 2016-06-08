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

#include "config.h"
#include <unistd.h>
#include <ctime>
#include "client.h"

using namespace std;
using namespace OC;


Resource::Resource(shared_ptr<OCResource> resource)
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    m_resourceHandle = resource;
    m_GETCallback = bind(&Resource::onGet, this, placeholders::_1, placeholders::_2, placeholders::_3);
}

Resource::~Resource()
{
}

shared_ptr<Resource> IoTClient::getPlatformResource()
{
    return m_platformResource;
}

void Resource::onGet(const HeaderOptions &headerOptions, const OCRepresentation &representation,
                     int errCode)
{
    if (errCode == OC_STACK_OK)
    {
        IoTClient::handle(headerOptions, representation, errCode, 0);
    }
    else
    {
        cerr << endl << endl << "Error in GET response from Resource resource" << endl;
    }

}

void Resource::get()
{

    QueryParamsMap params;
    assert(m_resourceHandle); //TODO display alert if not ready
    m_resourceHandle->get(params, m_GETCallback);
}



IoTClient::IoTClient()
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    init();
}

IoTClient::~IoTClient()
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
}


IoTClient *IoTClient::mInstance = nullptr;

IoTClient *IoTClient::getInstance()
{
    if ( IoTClient::mInstance == 0 )
    {
        mInstance = new IoTClient;
    }
    return mInstance;
}

void IoTClient::init()
{
    cerr << __PRETTY_FUNCTION__ << std::endl;

    m_platformConfig = make_shared<PlatformConfig>
                       (ServiceType::InProc, //
                        ModeType::Client, //
                        "0.0.0.0", //
                        0, //
                        OC::QualityOfService::HighQos //
                       );
    OCPlatform::Configure(*m_platformConfig);
    m_FindCallback = bind(&IoTClient::onFind, this, placeholders::_1);
}

void IoTClient::start()
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    string coap_multicast_discovery = string(OC_RSRVD_WELL_KNOWN_URI);
    coap_multicast_discovery += "?if=";
    coap_multicast_discovery += Config::m_interface;
    OCConnectivityType connectivityType(CT_ADAPTER_IP);
    OCPlatform::findResource("", coap_multicast_discovery.c_str(),
                             connectivityType,
                             m_FindCallback,
                             OC::QualityOfService::HighQos);
}


void IoTClient::onFind(shared_ptr<OCResource> resource)
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    try
    {
        if (resource)
        {
            string resourceUri = resource->uri();
            string hostAddress = resource->host();

            cerr << "\nFound Resource" << endl << "Resource Types:" << endl;
            for (auto & resourceTypes : resource->getResourceTypes())
            {
                cerr << "\t" << resourceTypes << endl;
            }

            cerr << "Resource Interfaces: " << endl;
            for (auto & resourceInterfaces : resource->getResourceInterfaces())
            {
                cerr << "\t" << resourceInterfaces << endl;
            }
            cerr << "Resource uri: " << resourceUri << endl;
            cerr << "host: " << hostAddress << endl;

            if (resourceUri == Config::m_endpoint)
            {
                m_platformResource = make_shared<Resource>(resource);
            }
        }
    }
    catch (OCException &ex)
    {
        cerr << "Caught exception in discoveredResource: " << ex.reason() << endl;
    }
}




void IoTClient::print(shared_ptr<OCResource> resource)
{
    string resourceUri = resource->uri();
    string hostAddress = resource->host();
    cerr << "host: " << hostAddress << endl;


    cerr << "\nFound Resource" << endl << "Resource Types:" << endl;
    for (auto & resourceTypes : resource->getResourceTypes())
    {
        cerr << "\t" << resourceTypes << endl;
    }

    cerr << "Resource Interfaces: " << endl;
    for (auto & resourceInterfaces : resource->getResourceInterfaces())
    {
        cerr << "\t" << resourceInterfaces << endl;
    }
    cerr << "Resource uri: " << resourceUri << endl;

}


// TODO: overide with your business logic
void IoTClient::handle(const HeaderOptions headerOptions, const OCRepresentation &rep,
                       const int &eCode, const int &sequenceNumber)
{
    std::string line;
    rep.getValue( Config::m_key, line);

    std::cout << line << std::endl;
}



int IoTClient::main(int argc, char *argv[])
{
    IoTClient::getInstance()->start();
    int delay = 5;

    while (delay >= 0)
    {
        sleep(delay);
        IoTClient::getInstance()->getPlatformResource()->get();;

    }
    return 0;
}


#if 1
int main(int argc, char *argv[])
{
    return IoTClient::main(argc, argv);
}
#endif
