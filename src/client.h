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

#ifndef CLIENT_H_
#define CLIENT_H_

#include <memory>
#include <iostream>

#include <iotivity/resource/OCApi.h>
#include <iotivity/resource/OCPlatform.h>
#include <iotivity/resource/OCResource.h>


class Resource
{
    public:
        Resource(std::shared_ptr<OC::OCResource> resource);
        virtual ~Resource();
    protected:
        std::shared_ptr<OC::OCResource> m_OCResource;
        OC::OCRepresentation m_Representation;
};


class IoTClient
{
    public:
        static int main(int argc, char *argv[]);
        static IoTClient *getInstance();
    public:
        std::shared_ptr<Resource> getResource();
        void start();
        void input();
        void print(std::shared_ptr<OC::OCResource> resource);
        static IoTClient *getInstance();
    private:
        IoTClient();
        virtual ~IoTClient();
        void init();
        void onFind(std::shared_ptr<OC::OCResource>);
    private:
        std::shared_ptr<Resource> m_resource;
        std::shared_ptr<OC::PlatformConfig> m_platformConfig;
        OC::FindCallback m_findCallback;
};

#endif /* CLIENT_H_ */
