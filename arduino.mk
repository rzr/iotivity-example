# //******************************************************************
# //
# // Copyright 2014 Intel Corporation.
# // Copyright 2016 Samsung Electronics France SAS All Rights Reserved.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

rule/arduino/default: help arduino/all
	@echo "# $@: $^"

arduino_mk_url?=https://github.com/sudar/Arduino-Makefile
platform=arduino
vpath += src

config_pkgconfig=0
include iotivity.mk

ARDUINO_DIR ?= /usr/share/arduino
AVR_TOOLS_DIR ?= ${ARDUINO_DIR}/hardware/tools/avr/
BOARD_TAG ?= mega2560
MONITOR_PORT ?= $(shell ls /dev/ttyUSB* | tail -n1)
MONITOR_PORT ?= $(shell ls /dev/ttyACM* | tail -n1)
ARCHITECTURE?=avr

MONITOR_BAUDRATE ?= 115200
AVRDUDE ?= ${ARDUINO_DIR}/hardware/tools/avrdude
AVRDUDE_CONF ?= ${ARDUINO_DIR}/hardware/tools/avrdude.conf

# hack to use arduino c++ libraries
LOCAL_CPP_SRCS += src/server.c.tmp.cpp
LOCAL_CPP_SRCS += src/server/platform/${platform}/platform.c.tmp.cpp
CPPFLAGS+=-Isrc

#{ configuration
eth_enabled?=1
serial_enabled?=1
#}
arch=${ARCHITECTURE}
arch=avr
iotivity_out=${iotivity_dir}/out/${platform}/${arch}/${iotivity_mode}

#{ iotivity
CXXFLAGS += -std=gnu++11
iotivity_libs_eth?=${iotivity_out}/libEthernet.a
#eol

ifeq (${eth_enabled}, 1)
ARDUINO_LIBS += Ethernet SPI
CPPFLAGS += -DARDUINOETH=1
iotivity_libs += ${iotivity_libs_eth}
endif

ARDUINO_LIBS += Wire 
ARDUINO_LIBS += SoftwareSerial
ifeq (${serial_enabled}, 1)
LIBS+=build-mega2560/HardwareSerial.o
CPPFLAGS += -DARDUINOSERIAL=1
CPPFLAGS += -DCONFIG_ARDUINOSERIAL=1
endif


# for iotivity-1.1.0
ifeq (${iotivity_version}, TODO_1.1.0)
iotivity_libs+=\
 ${iotivity_out}/libRBL_nRF8001.a \
 ${iotivity_out}/libBLE.a \
 #eol
endif

iotivity_libs+=\
 ${iotivity_out}/Time/libTime.a \
 ${iotivity_out}/libSPI.a \
 #eol


#}
USER_LIB_PATH+= ${iotivity_includedirs}


#OTHER_OBJS += ${iotivity_libs}

#TODO:
#core_lib=build-${BOARD_TAG}/libcore.a
core_lib=${iotivity_out}/arduino/libmega_at${BOARD_TAG}_core.a
CORE_LIB=${core_lib}


#LIBS+=${core_lib}
iotivity_libs += ${core_lib}
LIBS+=${iotivity_libs}

scons_flags+=TARGET_OS=${platform}
scons_flags+=LOGGING=1
#
scons_flags+=UPLOAD=false 
scons_flags+=BOARD=mega
scons_flags+=TARGET_ARCH=avr
scons_flags+=SHIELD=ETH

arduino/all: ${iotivity_dir} ${LIBS} ${TARGET_ELF}
	@echo "# $@: $^"

arduino/prep: rule/iotivity/build ${iotivity_libs}
	@echo "# $@: $^"

arduino/prepare:
	-killall xterm server client

arduino/demo: rule/arduino/default arduino/prepare upload arduino/run default/run
	@echo "# $@: $^"

arduino/run:
	@echo "# TODO: waiting iotivity resource registration ($@)"
	sleep 30
	@echo "# $@: $^"

${iotivity_libs}: ${iotivity_out}
	date

%.c.tmp.cpp: %.c arduino/prep
	@echo '#include "${<F}"' >$@

${iotivity_out}: ${iotivity_dir} ${iotivity_dir}/extlibs/tinycbor/tinycbor scons
	@echo "# $@: $^"
#	cd $< && ./auto_build.sh ${platform}

scons: ${iotivity_dir} tinycbor
	cd $< && scons resource ${scons_flags}

${ARDUINO_DIR}/Arduino.mk: 
	@echo "install: ${arduino_mk_url} "
	ls /etc/debian_version && sudo apt-get install arduino-mk

#{
# http://stackoverflow.com/questions/8188849/avr-linker-error-relocation-truncated-to-fit/38755757#38755757
include ${ARDUINO_DIR}/Arduino.mk
CORE_LIB=${core_lib}
LDFLAGS += -lc -lm


build-mega2560/libcore.a: ${core_lib}
	ln -fs ${core_lib} $@

#$(TARGET_ELF): $(LOCAL_OBJS) $(CORE_LIB) $(OTHER_OBJS)
#	$(CC) $(LDFLAGS) -o $@ $(LOCAL_OBJS) $(CORE_LIB) $(OTHER_OBJS)
#}

${TARGET_ELF}: ${LOCAL_OBJS} ${LIBS}
	${CC} ${LDFLAGS} -o $@ ${^}
	ls -l ${@}


arduino/help:
	@echo "# TARGET_ELF=${TARGET_ELF}"
	@echo "# CORE_LIB=${CORE_LIB}"
	@echo "# core_lib=${core_lib}"
	@echo "# LIBS=${LIBS}"


/dev/ttyACM0: /dev/ttyUSB0
	@echo sudo ln -fs ${<F} $@
