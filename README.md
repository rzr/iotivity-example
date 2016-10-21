## About : ##

Minimal IoTivity example


## Usage: ##

It has been tested with IoTivity 1.1.0 and later (up to 1.2.0+RC4 at least),
on various OS (Tizen, Yocto, Debian).

```
while true ; do date ; sleep 1 ;  done | ./bin/server

log: Server:
Press Ctrl-C to quit....
log: Successfully created line resource
Fri Oct 21 16:51:37 CEST 2016
Fri Oct 21 16:51:38 CEST 2016
Fri Oct 21 16:51:39 CEST 2016
(...)


 ./client
(...)
log: Resource: type: line
(...)
resourceUri=/lineResURI
(...)
Fri Oct 21 16:51:38 CEST 2016
Fri Oct 21 16:51:39 CEST 2016

```


## Resources: ##

http://www.slideshare.net/SamsungOSG/iotivity-tutorial-prototyping-iot-devices-on-gnulinux

https://wiki.tizen.org/wiki/User:Pcoval

https://wiki.iotivity.org/community

