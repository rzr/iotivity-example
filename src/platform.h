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

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <mraa.h>

class Platform
{
    public:

        /**
         * Minnowboard Max pin21 is GPIO_S5_0 Signal mapped on GPIO 82/338/.. (calamari's LED3 : Red)
         **/
        Platform(): m_pin {21}, m_mraa {NULL}, m_direction(OUTPUT) {}

        ~Platform();

        static Platform &getInstance() { static Platform instance; return instance; }

        void setup(int argc = 0, char *argv[] = 0);

        void setValue(bool value);

        static void log(char const *const message);

        static const bool INPUT = true;

        static const bool OUTPUT = false;

    private:

        unsigned int m_pin;

        bool m_direction;

        /** @param: mode : true for INPUT (RO), false for OUTPUT (WO) **/
        bool pinMode(int gpio, bool direction);

        bool digitalPinWrite(int gpio, bool value);

        mraa_gpio_context m_mraa;
};
#endif // PLATFORM_H_
