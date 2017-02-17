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
    public:
        IoTServer(std::string endpoint = Common::m_endpoint);
        virtual ~IoTServer();
    public:
        static int main(int argc, char *argv[]);
    protected:
        static void handle_signal(int signal);
        void init();
        void setup();
        OCStackResult createResource(std::string, std::string, OC::EntityHandler, OCResourceHandle &);
        OCEntityHandlerResult handleEntity(std::shared_ptr<OC::OCResourceRequest>);
        OCStackResult handlePost(std::shared_ptr<OC::OCResourceRequest> request);
        OCStackResult handleGet(std::shared_ptr<OC::OCResourceRequest> request);
        OCStackResult respond(std::shared_ptr<OC::OCResourceResponse> response);
        OC::OCRepresentation getResourceRepresentation();
        void postResourceRepresentation();
    protected:
        std::shared_ptr<OC::PlatformConfig> m_platformConfig;
        OC::OCRepresentation m_Representation;
        OCResourceHandle m_ResourceHandle;
        static bool m_over;
};

#endif /* SERVER_H_ */
