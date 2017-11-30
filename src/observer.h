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

#ifndef OBSERVER_H_
#define OBSERVER_H_

#include <memory>
#include <iostream>

#include <iotivity/resource/OCApi.h>
#include <iotivity/resource/OCPlatform.h>
#include <iotivity/resource/OCResource.h>


class IoTObserver
{
    public:
        static int main(int argc, char *argv[]);

        static IoTObserver *getInstance();

        static void onObserve(const OC::HeaderOptions /*headerOptions*/,
                              const OC::OCRepresentation &rep,
                              const int &eCode, const int &sequenceNumber);
    public:
        void start();
        /// Override with your business logic related to resource type
        void handle(const OC::HeaderOptions /*headerOptions*/,
                    const OC::OCRepresentation &rep,
                    const int &eCode, const int &sequenceNumber);
    private:
        IoTObserver();
        virtual ~IoTObserver();
        void init();
        void onFind(std::shared_ptr<OC::OCResource>);
    private:
        std::shared_ptr<OC::PlatformConfig> m_platformConfig;
        OC::FindCallback m_findCallback;
        std::string m_dateTime;
        double m_countDown;
};

#endif /* OBSERVER_H_ */
