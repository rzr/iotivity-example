#! /usr/bin/make -f
# //******************************************************************
# //
# // Copyright 2014 Intel Corporation.
# // Copyright 2015 Eurogiciel <philippe.coval@eurogiciel.fr>
# // Copyright 2016 Samsung <philippe.coval@osg.samsung.com>
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

top/local/%: config.mk
	${MAKE} $@{F}

-include config.mk

user?=${USER}
deps+=config.mk
#platform?=arduino
platform?=linux
mode?=release
mode=debug
export platform
arch?=$(shell arch || echo ${ARCH})

CFLAGS+=-g -O0
CFLAGS+=-Wall
#eol

demo_time?=20
#demo_time=200

log_dir?=${CURDIR}/temp/user/${USER}/platform/${platform}/tmp/

linux: linux/default

arduino: arduino/default

arduino/% linux/%:
	make platform=${@D} ${@F}


include ${platform}.mk

cleanall: clean
	rm -rf bin config.mk *.temp

#echo TODO
iotivity_out:
	ls ${iotivity_out} || ${MAKE} platform?=${platform} deps ${iotivity_out}
	sync

demo/kill: bin/server bin/client
	-killall ${^F}

demo/linux: iotivity_out demo/kill
	${MAKE} platform=linux run &
	@echo "# please check it change state on server"
	sleep ${demo_time}
	@echo "# please check it change state on server"
	sleep ${demo_time}
	${MAKE}	demo/kill

demo/arduino: iotivity_out
	${MAKE} platform=arduino iotivity_out all
	${MAKE} platform=linux iotivity_out all
	@echo "# to try locally, press ctrl+c then type:"
	@echo "# make run "
	@echo "#"
	sleep 10
	@echo "# About to deploy to ardiuno "
	@echo "#  please plug it now with eth shield "
	@echo "#  or press Ctrl+C to stop this build : $@"
	@echo "#"
	sleep 10
	${MAKE} platform=arduino upload
	sleep 10
	${MAKE} platform=linux run/client

demo: demo/${platform}

demo/all: distclean
	${MAKE} platform=linux distclean
	${MAKE} platform=arduino distclean
	${MAKE} platform=linux demo
	${MAKE} platform=arduino demo

demo/check: demo/log
	grep "client_loop: gSwitch.value=1 (iterate)" ${log_dir}/client

demo/log: ${log_dir}/server  ${log_dir}/client
	grep "server.c:" ${log_dir}/server 
	grep "client.c:" ${log_dir}/client 


check: distclean demo demo/check

help:
	echo "iotivity_out=${iotivity_out}"

rebuild: cleanall all

config.mk:
	ls /usr/lib*/pkgconfig/iotivity.pc && \
 echo "export config_pkgconfig=1" \
 || echo "export config_pkgconfig=0" > $@

distclean: cleanall
	-rm -rf tmp build*
#	-rm -rf temp #TODO

deps: ${deps}

-include local.mk
