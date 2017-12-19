# IOTIVITY TUTORIAL #

URL: http://git.s-osg.org/iotivity-example


## ABOUT : ##

Minimal IoTivity examples, to get inspiration from.

This repo is a collection of standalone projects that are designed
to be used out of iotivity main project.
That said IoTivity project is shipping many sample apps, 
once you get familiar with basic projects provided in this repo,
you're welcome to check them to understand more advanced services.

Technically each "subproject" is stored in a git branch 
but for developer convenience this can be flattened into same directory 
(using git submodules), see index for walk-through.


## USAGE: ##


### SETUP ###

Preliminary you need to install IoTivity for your system.

* https://wiki.iotivity.org/os

For the record, some subprojects were tested with:

* IoTivity 1.3.1+ on Ubuntu:14.04
* Iotivity 1.3.0+ on Fedora:24
* Iotivity 1.2.1+ on Tizen:Common
* Iotivity 1.2.0+ on Yocto's poky
* IoTivity 1.2.0+ on GNU/Linux system (Debian).


### PREPARE ###

Then, you need to "unpack" all branches into current work directory:

    # First Install git make etc
    git clone http://git.s-osg.org/iotivity-example
    cd iotivity-example
    # Flatten all subprojects to "src" folders
    make
    # Explore all subprojects
    find src -iname "README*"


## INDEX ##

* branch=example/master:
  * *DISCOVERY* mechanism, can be used as skeleton base for C++ projects
* branch=example/packaging/master: (on example/master)
  * Integration files for various OS: Tizen, Yocto, Debian
* branch=geolocation/master: (on example/packaging/master)
  * Shows *NOTIFY* mechanism
* branch=switch/master: (on example/master)
  * Shows *PUT* mechanism 
* branch=clock/1.2-rel: (on example/packaging/master)
  * Shows *OBSERVE* mechanism on previous release
* branch=clock/master: (on clock/1.2-rel)
  * Shows *ACL* mechanism for observing unsecured resource
* branch=brightness/1.2-rel: (on example/packaging/master)
  * Implements OneIot resource
* branch=brightness/master: (on brightness/1.2-rel)
  * Shows *ACL* mechanism for observing secured resource


## RESOURCES: ##

* https://wiki.iotivity.org/examples
* https://wiki.iotivity.org/build
* https://wiki.iotivity.org/community
* https://blogs.s-osg.org/category/iotivity/
* https://github.com/tizenteam/iotivity-example
* https://wiki.tizen.org/wiki/User:Pcoval
