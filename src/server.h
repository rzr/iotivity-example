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

#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <iostream>
#include <memory>

#include <iotivity/resource/OCPlatform.h>
#include <iotivity/resource/OCApi.h>

class IoTServer
{
        std::shared_ptr<OC::PlatformConfig> m_platformConfig;
        OC::OCRepresentation m_ledRepresentation;
        OCResourceHandle m_ledResource;

        void initializePlatform();
        void setupResources();
        void createResource(std::string, std::string, OC::EntityHandler, OCResourceHandle &);

        OC::OCRepresentation getLEDRepresentation();
        void putLEDRepresentation();
        OCEntityHandlerResult LEDEntityHandler(std::shared_ptr<OC::OCResourceRequest>);

    public:
        IoTServer(int pin = 0, std::string key = "switch");
        virtual ~IoTServer();

        static bool m_over;
};

#endif /* SERVER_H_ */
