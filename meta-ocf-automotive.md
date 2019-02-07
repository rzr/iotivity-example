# How OCF is Creating the Connected Car #

## The Connected Car & Fragmentation ##

Traditional car manufacturers have begun including early iterations of touchscreen technology with access to media and apps that can also provide basic HVAC (Heating, Ventilation and A/C) controls for the vehicle. These features can often be accessed through mobile devices with tailor-made apps from each car maker. However, this has led to OEMs building their own ecosystem silos, similar to the trends observed in the smartphone industry. The lack of an open, standardized framework has resulted in a fragmented market, where experiences from one OEM won’t work with another in any streamlined way; consequently, developers aren’t thinking about how to provide a rich user experience that allows cars and drivers to work in unison; this is a huge missed opportunity.


## Samsung OSG, OCF, and IoTivity ##

The Open Connectivity Foundation (OCF) is creating a specification and sponsoring the IoTivity open source project to deliver an open and secure connectivity and interoperability framework; this enables devices from multiple vendors that belong to multiple vertical domains to run on a variety of software and hardware platforms. Automotive is one key vertical where OCF can have a significant impact. The Samsung Open Source Group (OSG) has been playing a significant role in both the Open Connectivity Foundation (OCF) and IoTivity.

[![ocf-automotive-ces2017](https://i1.ytimg.com/vi/3d0uZE6lHvo/hqdefault.jpg)](https://youtu.be/3d0uZE6lHvo# "[CES 2017] Samsung Contributes to Open IoT Showcase at CES 2017")


## Collaboration with Genivi & W3C ## 

The GENIVI Alliance, a community that’s developing open source software and standards for the connected car, and OCF are partnering to co-develop open standards for vehicle connectivity and vehicle data exchange, including a unified model for secure discovery and exchange of information between smart homes, connected cars, and other IoT devices. The joint effort will also address end-to-end security challenges to enable new opportunities across multiple verticals. Additionally, GENIVI and OCF will  closely collaborate with the W3C Automotive Working Group, which develops the Open Web Platform API specification to expose vehicle data to web application developers.

[OCF Automotive Project Video Roll](https://youtu.be/-iYD0cZA9to#)

## Proof of Concept ##

Through a broad collaboration spanning ten organizations, we were able to develop and showcase our demo at the OCF booth at CES 2017.  ETRI, Genivi Alliance, Honeywell, Intel, Jaguar Land Rover, OCF, Samsung, Tinnos, The Data Alliance, and W3C all collaborated on open specifications and open source software to realize numerous interesting connected car use cases and explore interoperability between smart home devices and the connected vehicle.


[Connected Car at CES 2017: GENIVI demo with Open Connectivity Foundation](https://youtu.be/YOzuwtA9OW4#)


## Source Code ##

If you are interested in recreating these demos, or would like to experiment with it, base of demo code have been shared on the Samsung OSG git repository. To get started with IoTivity on GENIVI, simply add the “meta-ocf-automotive” layer, which includes minimalistic examples that demonstrate IoTivity. You can use the following as a guideline, for Yocto based distros.

```sh
# Fist Setup yocto buildenv, then
git clone https://github.com/GENIVI/genivi-dev-platform 
cd genivi-dev-platform 
git clone http://git.s-osg.org/meta-ocf-automotive/ 
MACHINE=qemux86-64 # Or supported BSPs (raspberrypi, minnowboard, dragonboard...)
echo 'BBLAYERS += "${TOPDIR}/../meta-ocf-automotive"'  >> 'gdp-src-build/conf/templates/bblayers.inc' 
source ./init.sh $MACHINE bitbake genivi-dev-platform
```

Once booted on qemu or the Raspberry Pi 2/3, an IoTivity server can be started from “/opt/iotivity-example*” and example clients can be run on the same LAN, including localhost. These examples are part of our IoTivity example, and are ready to be forked to create your own IoT services. We hope it will inspire you and simplify integration; start learning! For support, check out the detailed instructions on IoTivity’s automotive wiki page. Since GENIVI’s development platform is now shipping iotivity-node, linking sensors to the cloud as shown at FOSDEM, can be done in a jiffy.


[https://youtu.be/3L6_DbMLJ1k#iotivity-artik-20170204rzr](https://youtu.be/3L6_DbMLJ1k#iotivity-artik-20170204rzr)

Working together across organizational boundaries has enabled us to realize these demonstrations with real devices that run open source software; hopefully this will help people understand the potential business opportunities of connected cars. Feel free to post a comment below if you have any questions or comments.


## Additional Resources ##

[https://www.slideshare.net/SamsungOSG/iotivity-for-automotive-iot-interoperability](https://www.slideshare.net/SamsungOSG/iotivity-for-automotive-iot-interoperability)

IoTivity for Automotive IoT Interoperability from Samsung Open Source Group

* OCF Genivi CES 2017 Connected Car Demo
* Samsung Contributes to Open IoT Showcase at CES 2017
* W3C OCF Interoperability Demo
* OCF automotive profile
* IoTivity automotive wiki page
* IoTivity on Tizen GearS2

GENIVI + OCF Cooperation from Samsung Open Source Group

[https://www.slideshare.net/SamsungOSG/genivi-ocf-cooperation](https://www.slideshare.net/SamsungOSG/genivi-ocf-cooperation)


IoTivity for Automotive: meta-ocf-automotive tutorial from Samsung Open Source Group 

[![meta-ocf-automotive-tutorial](https://image.slidesharecdn.com/meta-ocf-automotive-20170531rzr-170531151433/95/iotivity-for-automotive-metaocfautomotive-tutorial-1-638.jpg)](https://www.slideshare.net/SamsungOSG/iotivity-for-automotive-metaocfautomotive-tutorial)

[https://www.slideshare.net/SamsungOSG/iotivity-for-automotive-metaocfautomotive-tutorial](https://www.slideshare.net/SamsungOSG/iotivity-for-automotive-metaocfautomotive-tutorial)


* Authors: Sanjeev BA, Philippe Coval
* Source: http://s-opensource.org/
