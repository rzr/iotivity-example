### REQUIEREMENT ###

atmega256

works on 1.1.1

### TODO ###

* build server
make plaform=ardiuno upload
* test client
 make platform=linux run/client


* try to fix the serial , problem w/ core lib
* try to write a better documentation
* try to rebuild sources as explained  in : ### HOWTO ARDUINO ###
*  clean makefile(clean makefile or the file which generate by make)
* try client on linux : make -f linux.mk
* buiuld client on other ardiuno
* document it all
* cleanup code


### HOWTO ARDUINO ###

## 1/ build iotivity ##

clone iotivity  sources:

git clone http://github.com/iotivity/iotivity -b 1.1.1

then use the following command to generate the lib and tools we need:

 ./auto_build.sh  arduino 

or adapt with your model as explained 

resource TARGET_OS=arduino UPLOAD=false BOARD=mega TARGET_ARCH=avr TARGET_TRANSPORT=IP SHIELD=ETH

https://www.iotivity.org/documentation/arduino/getting-started

https://wiki.iotivity.org/build_iotivity_project_for_arduino


install cbor

TODO : explain result files where are they


## 2/ build example ## 

build for arduino using :

cd ~/Dropbox/home-rzr-shared-to-boleguochao-gmail.com/iotivity-example/


 make -f arduino.mk local/build iotivity_dir=${HOME}/iotivity

yes

## 3/ fix makefile to avoid local/link trick"

 make -f arduino.mk iotivity_dir=${HOME}/iotivity



## 4 / upload to device ##


 make -f arduino.mk iotivity_dir=${HOME}/iotivity upload

(if the previous command works, I think we just need to add upload in the end)
wont work 


### NOTES / SETUP ###


make -f arduino.mk

from your make file, you will generate a server.o in the server folder, I just need you to generate a server.hex or server.ino in the same folder, then I can upload it to the arduino, because that is the arduino file format, after I get the .hex or .ino file, I need to use avrdude tool to upload it to the arduino.

## 3/ Link library ##

add liboc*.a something like that to arduino.mk
see LIBS or LDFLAGS




### build ##



This is the documentation of the arduino-mk.

sudo apt-get install arduino arduino-core arduino-mk

fitst to install the necessary package to continue.



2. Write the example code, here is hello_world.c, but the truth is that it is the blinking example from arduino IDE, I just copy it and make it into C file.


first I copy the blinking example from the Arduino IDE, then add the main function to call all the setup and loop function, after the test, the arduino code doesn't work, then I add the #include "Arduino.h", then compile is not a problem and it works.




3. Write the Makefile. I write the make file following the link: https://github.com/sudar/Arduino-Makefile/blob/master/examples/MakefileExample/Makefile-example.mk


the website have a very detail explaination about what command is for what purpose, I just follow it to change the parameter.


4. After the test of make file, everything works, then just type make for compiling the file, and make upload for compiling and uploading.


after debuging of the c file and the make file, we can use make to compile and make upload to compile and upload the program to the arduino.



### NOTES ###

https://balau82.wordpress.com/2011/03/29/programming-arduino-uno-in-pure-c/


try :


make CC="avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p"
