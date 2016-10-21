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
#include "platform.h"
#include <iostream>
using namespace std;


void Platform::setValue(bool value)
{
    LOG();
    cout << value << endl;
    digitalPinWrite(Platform::m_pin, value);
}


void Platform::setup(int argc, char *argv[])
{
    LOG();
    if (argc > 1 && argv[1])
    {
        m_pin = atoi(argv[1]);
    }
    pinMode(m_pin, Platform::OUTPUT);
}


bool Platform::pinMode(int pin, bool direction)
{
    LOG();
    cout << direction << endl;
    m_direction = direction;
    if (m_mraa)
    {
        mraa_gpio_close(m_mraa);
    }
    m_pin = pin;
    m_mraa = mraa_gpio_init(m_pin);
    if (m_mraa)
    {
        mraa_gpio_dir(m_mraa, (m_direction ? MRAA_GPIO_IN : MRAA_GPIO_OUT));
        return true;
    }
    return false;
}


bool Platform::digitalPinWrite(int pin, bool value)
{
    LOG();
    if (m_mraa == NULL)
    {
        if (!pinMode(pin, Platform::OUTPUT) )
        {
            return false;
        }
    }
    if (m_mraa)
    {
        mraa_gpio_write(m_mraa, (int) value);
        return true;
    }
    return false;
}

Platform::~Platform()
{
    if (m_mraa)
        mraa_gpio_close(m_mraa);
}


void Platform::log(char const *const message)
{
}
