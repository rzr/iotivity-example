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
    m_PUTCallback = bind(&Resource::onPut, this, placeholders::_1, placeholders::_2, placeholders::_3);
}

Resource::~Resource()
{
}


void Resource::onGet(const HeaderOptions &headerOptions, const OCRepresentation &representation,
                     int errCode)
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    if (errCode == OC_STACK_OK)
    {
        int value;
        representation.getValue(Config::m_key, value);
        cout << endl << endl << "GPIO switch state is: " << value << endl;
    }
    else
    {
        cerr << endl << endl << "Error in GET response from Resource resource" << endl;
    }
    IoTClient::DisplayMenu();
}

void Resource::onPut(const HeaderOptions &headerOptions, const OCRepresentation &representation,
                     int errCode)
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    if (errCode == OC_STACK_OK)
    {
        int value;
        representation.getValue(Config::m_key, value);
        cout << endl << endl << "Set GPIO switch to: " << value << endl;
    }
    else
    {
        cerr << endl << endl << "Error in PUT response from GPIO resource" << endl;
    }
    IoTClient::DisplayMenu();
}

void Resource::get()
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    QueryParamsMap params;
    assert(m_resourceHandle); //TODO display alert if not ready
    m_resourceHandle->get(params, m_GETCallback);
}

void Resource::put(int Switch)
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    QueryParamsMap params;
    OCRepresentation rep;
    rep.setValue(Config::m_key, Switch);
    m_resourceHandle->put(rep, params, m_PUTCallback);
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
    OCConnectivityType connectivityType(CT_ADAPTER_IP);
    OCPlatform::findResource("", //
                             coap_multicast_discovery.c_str(),
                             connectivityType,
                             m_FindCallback,
                             OC::QualityOfService::HighQos);
}


shared_ptr<Resource> IoTClient::getPlatformResource()
{
    return m_platformResource;
}


void IoTClient::onFind(shared_ptr<OCResource> resource)
{
    cerr << __PRETTY_FUNCTION__ << std::endl;
    try
    {
        if (resource)
        {
            print(resource);
            string resourceUri = resource->uri();
            if (resourceUri == Config::m_endpoint)
            {
                m_platformResource = make_shared<Resource>(resource);
            }
        }
        IoTClient::DisplayMenu();
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

    cerr << "Resource uri: " << resourceUri << endl;
    cerr << "Resource host: " << hostAddress << endl;

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
}


void IoTClient::DisplayMenu()
{
    cout << "\nEnter:" << endl
         << "*) Display this menu" << endl
         << "0) Turn GPIO OFF" << endl
         << "1) Turn GPIO ON" << endl
         << "2) Toggle GPIO" << endl
         << "9) Quit" << endl;
}


int IoTClient::main(int argc, char *argv[])
{
    IoTClient::getInstance()->start();

    int choice;
    bool state = false;
    do
    {
        cin >> choice;
        switch (choice)
        {
            case 0:
                state = false;
                break;
            case 1:
                state = true;
                break;

            case 2:
                state = !state;
                break;

            case 9:
                return 0;

            default:
                IoTClient::DisplayMenu();
                break;
        }

        if (IoTClient::getInstance()->getPlatformResource())
            IoTClient::getInstance()->getPlatformResource()->put(state);
        else
            cout << "LED resource not yet discovered" << endl;

    }
    while (choice != 9);
    return 0;
}


#if 1
int main(int argc, char *argv[])
{
    return IoTClient::main(argc, argv);
}
#endif
