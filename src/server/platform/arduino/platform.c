// -*-c-*-
//******************************************************************
//
// Copyright 2016 Samsung Electronics France SAS All Rights Reserved.
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

#include <Arduino.h>

#ifdef ARDUINOWIFI
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#elif defined ARDUINOETH
#include <EthernetServer.h>
#ifdef __cplusplus
#include <Ethernet.h>
#endif
#include <Dns.h>
#include <EthernetClient.h>
#include <util.h>
#include <EthernetUdp.h>
#include <Dhcp.h>
#endif

#include "common.h"

OCStackResult server_setup();
OCStackResult server_loop();
OCStackResult server_finish();

#ifndef CONFIG_GPIO_PIN
#define CONFIG_GPIO_PIN 13
#endif

// TODO : put on a single config "000000000" string
// https://www.arduino.cc/en/Reference/EthernetBegin
#ifndef CONFIG_MAC_1
#define CONFIG_MAC_1 0xDE
#endif
#ifndef CONFIG_MAC_2
#define CONFIG_MAC_2 0xAD
#endif
#ifndef CONFIG_MAC_3
#define CONFIG_MAC_3 0xBE
#endif
#ifndef CONFIG_MAC_4
#define CONFIG_MAC_4 0xEF
#endif
#ifndef CONFIG_MAC_5
#define CONFIG_MAC_5 0xFE
#endif
#ifndef CONFIG_MAC_6
#define CONFIG_MAC_6 0xED
#endif

static int gGpio = CONFIG_GPIO_PIN;


unsigned int sleep(unsigned int secs)
{
    delay(secs*1000);
}


#include <SFE_BMP180.h>
int platform_getValue()
{
    double value=0;
    char status=0;
    static SFE_BMP180 sensor;
    static int once=1;
    if (once) { once--;
        status = sensor.startTemperature();
    }
    status = sensor.getTemperature(value);
    return (int) value;
}

void platform_setValue(bool value)
{
    digitalWrite(gGpio, (value) ? HIGH : LOW);
}


void blink(int n)
{
    int step = 250;
    delay(4*step);
    for (int i=0;i<n;i++) {
        platform_setValue(false);
        delay(step);
        platform_setValue(true);
        delay(step);
    }
    platform_setValue(false);
    delay(4*step);
}


//TODO: http://playground.arduino.cc/Main/Printf
void platform_log(const char* text)
{
#ifdef ARDUINOSERIAL
    if (!true)
    {
        Serial.print(text);
    }
#else
    if (false) //TODO: poor man version
    {
        blink(strlen(text));
    }
#endif
}


int setup_network()
{
#ifdef ARDUINOETH
    //TODO: update with actual MAC address
    uint8_t ETHERNET_MAC[] =
        { CONFIG_MAC_1, CONFIG_MAC_2, CONFIG_MAC_3,
          CONFIG_MAC_4, CONFIG_MAC_5, CONFIG_MAC_6};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);

    if (error == 0)
    {
        LOGf("%s",__FUNCTION__);
        if (false) blink(9);
        //return -1; //TODO check
    }

    if (false) {
        IPAddress ip = Ethernet.localIP();
        for(int i=0;i<4;i++) {
            blink(ip[i]);
        }
    }
    //TODO: OIC_LOG_V(INFO, TAG, "IP Address:  %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
#else
    blink(20);
#endif

    return 0;
}


void platform_test()
{
    if (false) blink(2);
}

void platform_setup()
{
    static int init=1; // make sure it will be init once
    if (0 >= init--) return;

    pinMode(gGpio, OUTPUT);
    LOGf("%d",gGpio);

    if (!false) blink(1);

#ifdef CONFIG_ARDUINOSERIAL
    Serial.begin(115200);
    LOGf("%d",gGpio);
#endif
    if (!false) blink(2);
    if (true)
    {
        setup_network();
    }
    if (!false) blink(3);
    platform_log("1");

    delay(4*gDelay);
}

void setup()
{
    platform_setup();
    server_setup();
    if (false) blink(4);
}


void loop()
{
    if (false) blink(1); // heartbeat to debug
    delay(4*gDelay);
    server_loop();
}
