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
#include "client.h"

using namespace std;
using namespace OC;


shared_ptr<LED> IoTClient::getPlatformLED()
{
    return m_platformLED;
}

LED::LED(shared_ptr<OCResource> Resource)
{
    m_resourceHandle = Resource;
    m_GETCallback = bind(&LED::onGet, this, placeholders::_1, placeholders::_2, placeholders::_3);
    m_PUTCallback = bind(&LED::onPut, this, placeholders::_1, placeholders::_2, placeholders::_3);
}

LED::~LED()
{
}

void LED::get()
{
    QueryParamsMap params;
    m_resourceHandle->get(params, m_GETCallback);
}

void LED::put(int Switch)
{
    QueryParamsMap params;
    OCRepresentation rep;
    rep.setValue(Config::m_key, Switch);
    m_resourceHandle->put(rep, params, m_PUTCallback);
}

void LED::onGet(const HeaderOptions &headerOptions, const OCRepresentation &representation,
                int errCode)
{
    if (errCode == OC_STACK_OK)
    {
        int value;
        representation.getValue(Config::m_key, value);
        cout << endl << endl << "LED switch state is: " << value << endl;
    }
    else
    {
        cerr << endl << endl << "Error in GET response from LED resource" << endl;
    }
    IoTClient::DisplayMenu();
}

void LED::onPut(const HeaderOptions &headerOptions, const OCRepresentation &representation,
                int errCode)
{
    if (errCode == OC_STACK_OK)
    {
        int value;
        representation.getValue(Config::m_key, value);
        cout << endl << endl << "Set LED switch to: " << value << endl;
    }
    else
    {
        cerr << endl << endl << "Error in PUT response from LED resource" << endl;
    }
    IoTClient::DisplayMenu();
}


IoTClient::IoTClient()
{
    cout << "Running IoTClient constructor" << endl;
    initializePlatform();
}

IoTClient::~IoTClient()
{
    cout << "Running IoTClient destructor" << endl;
}

void IoTClient::initializePlatform()
{
    m_platformConfig = make_shared<PlatformConfig>(ServiceType::InProc, ModeType::Client, "0.0.0.0",
                       0, OC::QualityOfService::HighQos);
    OCPlatform::Configure(*m_platformConfig);
    m_resourceDiscoveryCallback = bind(&IoTClient::discoveredResource, this, placeholders::_1);
}

void IoTClient::findResource()
{
    string coap_multicast_discovery = string(OC_RSRVD_WELL_KNOWN_URI "?if=" );
    coap_multicast_discovery += Config::m_interface;
    OCConnectivityType connectivityType(CT_ADAPTER_IP);
    OCPlatform::findResource("", coap_multicast_discovery.c_str(),
                             connectivityType,
                             m_resourceDiscoveryCallback,
                             OC::QualityOfService::LowQos);
}

void IoTClient::discoveredResource(shared_ptr<OCResource> Resource)
{
    try
    {
        if (Resource)
        {
            string resourceUri = Resource->uri();
            string hostAddress = Resource->host();

            cout << "\nFound Resource" << endl << "Resource Types:" << endl;
            for (auto & resourceTypes : Resource->getResourceTypes())
            {
                cout << "\t" << resourceTypes << endl;
            }

            cout << "Resource Interfaces: " << endl;
            for (auto & resourceInterfaces : Resource->getResourceInterfaces())
            {
                cout << "\t" << resourceInterfaces << endl;
            }
            cout << "Resource uri: " << resourceUri << endl;
            cout << "host: " << hostAddress << endl;

            if (resourceUri == Config::m_endpoint)
            {
                m_platformLED = make_shared<LED>(Resource);
            }
        }
        IoTClient::DisplayMenu();
    }
    catch (OCException &ex)
    {
        cerr << "Caught exception in discoveredResource: " << ex.reason() << endl;
    }
}



void IoTClient::DisplayMenu()
{
    cout << "\nEnter:" << endl
         << "*) Display this menu" << endl
         << "0) Turn LED OFF" << endl
         << "1) Turn LED ON" << endl
         << "9) Quit" << endl;
}


int main(int argc, char *argv[])
{
    IoTClient client;
    cout << "Performing Discovery..." << endl;
    client.findResource();
    int choice;
    do
    {
        cin >> choice;
        switch (choice)
        {
            case 0:
                if (client.getPlatformLED())
                    client.getPlatformLED()->put(0);
                else
                    cout << "LED resource not yet discovered" << endl;
                break;
            case 1:
                if (client.getPlatformLED())
                    client.getPlatformLED()->put(1);
                else
                    cout << "LED resource not yet discovered" << endl;
                break;
            case 9:
	      return 0;
            default:
                IoTClient::DisplayMenu();
		break;
        }
    }
    while (choice != 9);
    return 0;
}
