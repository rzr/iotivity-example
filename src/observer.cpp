// -*-c++-*-
//******************************************************************
//
// Copyright 2014 Intel Corporation.
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
#include <cstdio>

#include "observer.h"

using namespace std;
using namespace OC;


IoTObserver::IoTObserver()
{
    LOG();
    init();
}
IoTObserver::~IoTObserver()
{
    LOG();
}


IoTObserver *IoTObserver::getInstance()
{
    static IoTObserver *pInstance = nullptr;
    if (!pInstance)
    {
        pInstance = new IoTObserver;
    }
    return pInstance;
}


void IoTObserver::init()
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
    m_findCallback = bind(&IoTObserver::onFind, this, placeholders::_1);
}

void IoTObserver::start()
{
    LOG();
    string uri = string(OC_RSRVD_WELL_KNOWN_URI);
    OCConnectivityType connectivityType(CT_ADAPTER_IP);
    try
    {
        OCPlatform::findResource("",  //
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


void IoTObserver::onFind(shared_ptr<OCResource> resource)
{
    LOG();
    try
    {
        if (resource)
        {
            auto resourceUri = resource->uri();
            if (Common::m_endpoint == resourceUri)
            {
                cerr << "resourceUri=" << resourceUri << endl;
                QueryParamsMap test;
                resource->observe(OC::ObserveType::Observe, test, &IoTObserver::onObserve);

            }
        }
    }
    catch (OCException &ex)
    {
        cerr << "error: Caught exception in discoveredResource: " << ex.reason() << endl;
    }
}


void IoTObserver::onObserve(const HeaderOptions headerOptions,
                            const OCRepresentation &rep,
                            const int &eCode, const int &sequenceNumber)
{
    LOG();
    try
    {
        if (eCode == OC_STACK_OK && sequenceNumber != OC_OBSERVE_NO_OPTION)
        {
            if (sequenceNumber == OC_OBSERVE_REGISTER)
            {
                cerr << "log: Observe registration action is successful" << endl;
            }
            else if (sequenceNumber == OC_OBSERVE_DEREGISTER)
            {
                cerr << "log: Observe De-registration action is successful" << endl;
            }
            cerr << "log: observe: sequenceNumber=" << sequenceNumber << endl;
            IoTObserver::getInstance()->handle(headerOptions, rep, eCode, sequenceNumber);
        }
        else
        {
            if (sequenceNumber == OC_OBSERVE_NO_OPTION)
            {
                cerr << "warning: Observe registration or de-registration action is failed" << endl;
            }
            else
            {
                cerr << "error: onObserve Response error=" << eCode << endl;
                exit(-1);
            }
        }
    }
    catch (exception &e)
    {
        cerr << "warning: Exception: " << e.what() << " in onObserve" << endl;
    }
}


// TODO: override with your business logic
void IoTObserver::handle(const HeaderOptions headerOptions, const OCRepresentation &rep,
                         const int &eCode, const int &sequenceNumber)
{
    LOG();
    rep.getValue("brightness", Common::m_brightness);
    cout << Common::m_type << ": { "<< Common::m_brightness << " }" << endl;
}


int IoTObserver::main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (0 == strcmp("-v", argv[i]))
        {
            Common::m_logLevel++;
        }
    }
    IoTObserver::getInstance()->start();

    int choice = 0;
    do
    {
        cin >> choice;
        switch (choice)
        {
            case 9:
            default:
                return 0;
        }
    }
    while (choice != 9);
    return 0;
}


#ifdef CONFIG_OBSERVER_MAIN
int main(int argc, char *argv[])
{
    return IoTObserver::main(argc, argv);
}
#endif
