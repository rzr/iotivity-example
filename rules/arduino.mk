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

arduino/default: arduino/all
	@echo "# $@: $^"

arduino_mk_url?=https://github.com/sudar/Arduino-Makefile
platform=arduino
export platform
vpath += src

config_pkgconfig=0
#include rules/iotivity.mk

#system_ARDUINO_DIR?=/usr/share/arduino
#ARDMK_DIR?=${system_ARDUINO_DIR}

ARDMK_DIR?=${src_dir}/arduino-mk
arduino_mk?=${ARDMK_DIR}/Arduino.mk
#./tmp/iotivity/version/1.2.1/platform/arduino/src/iotivity/extlibs/arduino/arduino-1.5.8/hardware/tools/avr/etc/avrdude.conf
ARDUINO_DIR?=${iotivity_dir}/extlibs/arduino/arduino-1.5.8
ARDUINO_HOME?=${ARDUINO_DIR}
AVR_TOOLS_DIR?=${ARDUINO_DIR}/hardware/tools/avr
VARIANT?=mega
BOARD_TAG=mega2560
BOARD_SUB=
MCU?=at${BOARD_TAG}
F_CPU?=16000000L
#BOARD_SUB=at${BOARD_TAG}

MONITOR_PORT?=/dev/ttyUSB* 
#MONITOR_PORT?=/dev/ttyACM*
ARCHITECTURE=avr
AVRDUDE_ARD_BAUDRATE?=115200
MONITOR_BAUDRATE?=${AVRDUDE_ARD_BAUDRATE}

#AVRDUDE?=${ARDUINO_DIR}/hardware/tools/avr/bin/avrdude
#AVRDUDE_CONF?=${ARDUINO_DIR}/hardware/tools/avr/etc/avrdude.conf
AVRDUDE_ARD_PROGRAMMER?=wiring

# hack to use arduino c++ libraries
LOCAL_CPP_SRCS += src/server.c.tmp.cpp
LOCAL_CPP_SRCS += src/server/platform/${platform}/platform.c.tmp.cpp
CPPFLAGS+=-Isrc

#{ configuration
eth_enabled?=1
CPPFLAGS += -DARDUINOSERIAL=1
#CPPFLAGS += -DCONFIG_ARDUINOSERIAL=1
ARDUINO_LIBS += Wire 
ARDUINO_LIBS += SoftwareSerial
#ARDUINO_LIBS += HardwareSerial
#}
arch=${ARCHITECTURE}
arch=avr
export arch
#TODO
iotivity_out=${iotivity_dir}/out/${platform}/${arch}/${iotivity_mode}
export iotivity_out

#{ iotivity
CXXFLAGS+=-std=gnu++11


iotivity_libs_eth?=${iotivity_out}/libEthernet.a
#eol

ifeq (${eth_enabled}, 1)
ARDUINO_LIBS += Ethernet SPI
CPPFLAGS += -DARDUINOETH=1
iotivity_libs += ${iotivity_libs_eth}
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
#CORE_LIB=${core_lib}
#LIBS+=${core_lib}
iotivity_libs += ${core_lib}
LIBS+=${iotivity_libs}

scons_flags+=TARGET_OS=${platform}
#
scons_flags+=UPLOAD=false 
scons_flags+=BOARD=mega
scons_flags+=TARGET_ARCH=avr
scons_flags+=SHIELD=ETH

arduino/all: rule/iotivity/out ${TARGET_ELF}
	@echo "# $@: $^"

arduino/upload: ${AVRDUDE_CONF} ${AVRDUDE} ${MONITOR_PORT} upload

arduino/demo: arduino/all arduino/upload arduino/run
	@echo "# $@: $^"

arduino/run:
	sleep 10
	${MAKE} platform=default
	${MAKE} run platform=default
	@echo "# $@: $^"

%.c.tmp.cpp: %.c
	@echo '#include "${<F}"' >$@

rule/arduino/mk: ${arduino_mk}

rule/arduino/dir: rule/print/ARDUINO_DIR ${ARDUINO_DIR}

${ARDUINO_DIR}: ${iotivity_out}
#	ls $@ || ${MAKE} rule/iotivity/out
	ls $@ $^
	@echo "# $@: $^"

rule/iotivity/out: rule/print/iotivity_out ${iotivity_out}
	@echo "# $@: $^"

${iotivity_out}: rule/iotivity/build
	ls $@
#	ls $@ || make platform=arduino rule/iotivity/build

${iotivity_out}: rule/iotivity/build
#	ls $@ || ${MAKE} rule/iotivity/build

.PHONY: rule/default/arduino_mk

${arduino_mk}: ${ARDMK_DIR}
	-sed -e 's|::|:|g' -i "$@"

${ARDMK_DIR}: ${iotivity_out}
	${git_clone} ${arduino_mk_url} "$@"
	ls "$@"

todo/${arduino_mk}: rule/default/arduino_mk
	ls $@ || ${MAKE} platform=arduino rule/default/arduino_mk
	ls $@

rule/arduino_mk:
	ls /etc/debian_version \
&& ${MAKE} rule/debian/${@F} \
|| ${MAKE} rule/default/${@F}


rule/default/arduino_mk:
	ls ${ARDUINO_DIR}
	@mkdir -p ${@D}
	@rmdir ${@D}
	@-ls "${system_ARDUINO_DIR}" \
&& sudo apt-get remove arduino-mk arduino arduino-core
	[ -e "${arduino_mk}" ] \
|| git clone --depth 1 ${arduino_mk_url} "${ARDMK_DIR}" \
&& sed -e 's|::|:|g' -i "${arduino_mk}"
	ls -l "${arduino_mk}"

rule/debian/arduino_mk: /etc/debian_version rule/arduino/dir
	sudo apt-get install arduino-mk
	-sudo sed -e 's|::|:|g' -i "${arduino_mk}"
	[ -x "${ARDMK_DIR}" ] || ln -fs ${system_ARDUINO_DIR}  ${ARDMK_DIR}

rule/arduino/longhelp: \
 rule/print/ARDUINO_VERSION \
 rule/print/ARDUINO_DIR \
 rule/print/ARDUINO_LIBS \
 rule/print/CC \
 rule/print/all \
 rule/print/arduino_mk \
 rule/print/TARGET_ELF \
 #eol


#{
# http://stackoverflow.com/questions/8188849/avr-linker-error-relocation-truncated-to-fit/38755757#38755757
include ${arduino_mk}

LDFLAGS += -lc -lm

$(TARGET_ELF): $(LOCAL_OBJS) $(CORE_LIB) $(OTHER_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(LOCAL_OBJS) $(CORE_LIB) $(OTHER_OBJS)
#}

${TARGET_ELF}: ${LOCAL_OBJS} ${LIBS}
	${CC} ${LDFLAGS} -o $@ ${^}
	ls -l ${@} $^

/dev/ttyACM0: /dev/ttyUSB0
	@echo sudo ln -fs ${<F} $@

rule/arduino/setup: /etc/debian_release
	groups | grep dialout || sudo addgroup $USER dialout
	@echo sudo su -l ${USER} && cd ${CURDIR}

rule/arduino/target_efl:  rule/print/TARGET_ELF ${TARGET_ELF}
