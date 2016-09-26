// -*-c-*-
//******************************************************************
//
// Copyright 2016 Samsung Electronics France SAS All Rights Reserved.
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

#include <unistd.h>
#include <signal.h>


#ifndef CONFIG_GPIO
#define CONFIG_GPIO 22 //=pin13@artik10
#endif

#define OUTPUT (1)

static int gGpio = CONFIG_GPIO;

bool pinMode(int gpio, int dir)
{
    FILE * fd;
    char fName[128];

    //Exporting the pin to be used
    if(( fd = fopen("/sys/class/gpio/export", "w")) == NULL) {
        printf("error: unable to export pin for GPIO=%d\n",gpio);
        return false;
    }

    fprintf(fd, "%d\n", gpio);
    fclose(fd);	

    // Setting direction of the pin
    sprintf(fName, "/sys/class/gpio/gpio%d/direction", gpio);
    if((fd = fopen(fName, "w")) == NULL) {
        printf("error: can't open pin direction for GPIO=%d\n",gpio);
        return false;
    }

    fprintf(fd, (dir == OUTPUT) ? "out\n" : "in\n");
    fclose(fd);

    return true;
}


bool digitalPinWrite(int gpio, bool on)
{
    FILE * fd;
    char fName[128];
    LOGf("%d", gpio);

    pinMode(gGpio, OUTPUT);

    sprintf(fName, "/sys/class/gpio/gpio%d/value", gpio);
    if((fd = fopen(fName, "w")) == NULL) {
        printf("error: can't open inode for GPIO=%d\n",gpio);
        return false;
    }

    fprintf(fd, "%d\n" , (on) );
    fclose(fd);
    return true;
}


void platform_setValue(bool value)
{
    LOGf("%d", value);
#ifdef CONFIG_WANT_GPIO
    digitalPinWrite( gGpio, value);
#endif
}

void platform_setup()
{
#ifdef CONFIG_WANT_GPIO
    pinMode(gGpio, OUTPUT);
#endif
}

void platform_loop()
{
}

void handleSigInt(int signum)
{
    gOver = (signum == SIGINT);
}

int main()
{
    OCStackResult result;

    result = server_setup();

    if (result!=0)
    {
        return result;
    }

    signal(SIGINT, handleSigInt);

    while (!gOver)
    {
        result = server_loop();
    }

    stop();

    return (int) result ;
}
