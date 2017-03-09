#! /usr/bin/make -f
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
rule/task/%: local.mk
	${MAKE} ${@F}

exe_args?=-v

package?=iotivity-example
local_bindir?=bin
local_optdir?=opt
install_dir?=${DESTDIR}/${local_optdir}/${package}/

-include local.mk
demo_time?=20
log_dir?=${CURDIR}/tmp/log
platform?=default

user?=${USER}
deps+=local.mk
export platform
arch?=$(shell arch || echo ${ARCH})


rule/default: ${platform}/default

all: help local.mk
	${MAKE} ${platform}/$@

help: README.md
	@cat $<
	@echo "# Type make demo , client/server usage"
	@echo "# iotivity_version=${iotivity_version}"
	@echo "# config_pkgconfig=${config_pkgconfig}"
	@echo "# iotivity_cflags=${iotivity_cflags}"
	@echo "# CPPFLAGS=${CPPFLAGS}"

demo: demo/linux demo/arduino
	@echo "# $@: $^"

demo/%:
	${MAKE} ${@F}/${@D}

demo/${platform}: iotivity_out demo/kill
	${MAKE} platform=${@F} platform/demo &
	@echo "# please check it change state on server"
	sleep ${demo_time}
	@echo "# please check it change state on server"
	sleep ${demo_time}
	${MAKE}	demo/kill

demo/all: distclean
	${MAKE} platform=${platform} distclean
	${MAKE} platform=${platform} demo

default/% linux/% arduino/%:
	${MAKE} platform=${@D} ${@D}/${@F}

platform/demo: xterm/server xterm/client
	@echo "# $@: $^"

include ${platform}.mk


install: ${all}
	install -d ${install_dir}/bin
	install $^ ${install_dir}/bin
	install README.md ${install_dir}


iotivity_out:
	ls ${iotivity_out} || ${MAKE} platform=${platform} deps ${iotivity_out}
	@echo "# $@: $^"

demo/kill: bin/server bin/client
	-killall ${^F}

demo/check: demo/log
	grep "client_loop: gSwitch.value=1 (iterate)" ${log_dir}/client

demo/log: ${log_dir}/server  ${log_dir}/client
	grep "server.c:" ${log_dir}/server
	grep "client.c:" ${log_dir}/client

run/%: bin/%
	mkdir -p ${log_dir}
	cd ${iotivity_out} && \
 LD_LIBRARY_PATH=. stdbuf -oL -eL ${exe} \
 ${CURDIR}/${<D}/${<F} ${exe_args} \
 | tee ${log_dir}/${@F}

xterm/% : bin/%
	xterm -e ${MAKE} run/${@F}  &
	sleep 5

run: run/client
	@echo "# $@: $^"

platform/demo: xterm/server xterm/client
	@echo "# $@: $^"

check: ${all}
	@echo "# $@: $^: TODO"

rebuild: cleanall all
	@echo "# $@: $^"

local.mk:
	ls $(PKG_CONFIG_SYSROOT_DIR)/usr/lib*/pkgconfig/iotivity.pc && \
 echo "export config_pkgconfig=1" \
 || echo "export config_pkgconfig=0" > $@
	@echo "# type make help for usage"

clean:
	find . -iname "*~" -exec rm '{}' \;
	find . -iname "*.o" -exec rm -v '{}' \;

cleanall: clean
	rm -rf bin local.mk *.temp

distclean: ${platform}/cleanall cleanall
	-rm -rf build* tmp temp
	-${MAKE} ${platform}/$@


deps: ${deps}
	@echo "# $@: $^"

-include config.mk

%.mk:
	touch $@
