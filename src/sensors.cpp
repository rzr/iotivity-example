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
#include "sensors.h"

mraa_gpio_context Sensors::gpio = NULL;

void Sensors::SetupPins(int pin)
{
#if 0
    gpio = mraa_gpio_init(pin);
    if (gpio != NULL) // Set direction to OUTPUT
        mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
#else
    digitalPinMode(pin,OUTPUT);
#endif
}


bool Sensors::digitalPinMode(int pin, int dir)
{
    FILE * fd;
    char fName[128];

    //Exporting the pin to be used
    if(( fd = fopen("/sys/class/gpio/export", "w")) == NULL) {
        printf("Error: unable to export pin\n");
        return false;
    }

    fprintf(fd, "%d\n", pin);
    fclose(fd);	// Setting direction of the pin
    sprintf(fName, "/sys/class/gpio/gpio%d/direction", pin);
    if((fd = fopen(fName, "w")) == NULL) {
        printf("Error: can't open pin direction\n");
        return false;
    }

    if(dir == OUTPUT) {
        fprintf(fd, "out\n");
    } else {
        fprintf(fd, "in\n");
    }

    fclose(fd);
    return true;
}


bool Sensors::digitalPinWrite(int pin, bool on){
    FILE * fd;
    char fName[128];

    //Exporting the pin to be used
    if(( fd = fopen("/sys/class/gpio/export", "w")) == NULL) {
        printf("Error: unable to export pin\n");
        return false;
    }

    fprintf(fd, "%d\n", pin);
    fclose(fd);	// Setting direction of the pin
    sprintf(fName, "/sys/class/gpio/gpio%d/value", pin);
    if((fd = fopen(fName, "w")) == NULL) {
        printf("Error: can't open pin direction\n");
        return false;
    }

    fprintf(fd, "%d\n" , (on) );


    fclose(fd);
    return true;
}


void Sensors::ClosePins()
{
#if 0
    mraa_gpio_close(gpio);
#endif
}

void Sensors::SetOnboardLed(int on)
{
#if 0
    if (gpio == NULL)
    {
        gpio = mraa_gpio_init(Config::m_gpio);
        if (gpio != NULL)  // Set direction to OUTPUT
            mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
        else
            exit(-1);
    }
    if (gpio != NULL) // Writes into GPIO
        mraa_gpio_write(gpio, on);
#else

    digitalPinWrite( Config::m_gpio ,on);
#endif
}
