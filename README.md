## IOTIVITY EXAMPLE ##

URL: http://git.s-osg.org/iotivity-example


## ABOUT: ##

Minimal IoTivity example.

```
while true ; do date ; sleep 1 ;  done | ./bin/server

log: Server:
Press Ctrl-C to quit....
log: Successfully created line resource
Fri Oct 21 16:51:37 CEST 2016
Fri Oct 21 16:51:38 CEST 2016
Fri Oct 21 16:51:39 CEST 2016
(...)


./bin/client
(...)
log: Resource: type: line
(...)
resourceUri=/lineResURI
(...)
Fri Oct 21 16:51:38 CEST 2016
Fri Oct 21 16:51:39 CEST 2016

```


## USAGE: ##

It has been tested with:

* IoTivity 1.2.0+ on various OS (Tizen, Yocto, Debian).


## RESOURCES: ##

* https://wiki.iotivity.org/examples
* https://wiki.iotivity.org/build
* https://wiki.iotivity.org/community
* https://blogs.s-osg.org/category/iotivity/
* https://github.com/tizenteam/iotivity-example
* https://wiki.tizen.org/wiki/User:Pcoval

