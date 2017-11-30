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

default: all

name?=iotivity-example

config_pkgconfig?=1
export config_pkgconfig


tmpdir?=tmp

version?=$(shell test -d ${CURDIR}/.git && git describe --always || echo 0.0.0)
package?=${name}-${version}
distdir?=${CURDIR}/..
tarball?=${distdir}/${package}.tar.gz

#TODO: workaround missing /usr/include/iotivity namespace
iotivity_dir?=iotivity
includedir?=/usr/include
include_dir?=${PKG_CONFIG_SYSROOT_DIR}/${includedir}

ifeq (${config_pkgconfig},1)
override CPPFLAGS+=$(shell pkg-config iotivity --cflags)
override LDLIBS+=$(shell pkg-config iotivity --libs)
iotivity_dir=${include_dir}/iotivity
else
override LDLIBS+=-loc -loc_logger -loctbstack
#TODO: Enable security here
#override LDLIBS+=-locpmapi
#override CPPFLAGS+=-D__WITH_DTLS__=1
override CPPFLAGS+=-I${iotivity_dir}
override CPPFLAGS+=-I${iotivity_dir}/resource
override CPPFLAGS+=-I${iotivity_dir}/resource/c_common
override CPPFLAGS+=-I${iotivity_dir}/resource/oc_logger
override CPPFLAGS+=-I${iotivity_dir}/resource/stack
override CPPFLAGS+=-I${iotivity_dir}/c_common
all+=${iotivity_dir}
srcs_all+=$(wildcard src/*.cpp)
endif

all+=${tmpdir}/${name}.service
override CPPFLAGS+=-I.

DESTDIR?=/
local_bindir?=bin
extradir?=/opt
install_dir?=${DESTDIR}${extradir}/${name}/
unitdir?=/usr/lib/systemd/system/
log_dir?=${CURDIR}/tmp/log

vpath+=src
VPATH+=src

override CPPFLAGS+=-DCONFIG_SERVER_MAIN=1
override CPPFLAGS+=-DCONFIG_CLIENT_MAIN=1
override CPPFLAGS+=-DCONFIG_OBSERVER_MAIN=1

CPPFLAGS+=-g -O0
run_args?=-v
V=1

override CXXFLAGS+=-std=c++0x

srcs?=src/platform.cpp src/common.cpp
objs?=${srcs:.cpp=.o}

client?=${local_bindir}/client

server?=${local_bindir}/server

observer?=${local_bindir}/observer

exes?=${client} ${observer}
exes+=${server}
all+=${exes}

#TODO: Add Security related files if SECURED
json_files?=$(wildcard *.json)
dat_files?=${json_files:.json=.dat}
all+=${json_files}
all+=${dat_files}
json2cbor?=$(shell ls /usr/lib*/iotivity/resource/csdk/security/tool/json2cbor | head -n1 || echo "json2cbor")

all: ${all}

dat: ${json_files} ${dat_files}
	ls -l $<

%.dat: %.json
	${json2cbor} $< $@
	ls -l $@

${local_bindir}/server: src/server.o ${server_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} ${LDFLAGS} $^ ${LDLIBS} -o ${@}

${local_bindir}/client: src/client.o ${client_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} ${LDFLAGS} $^ ${LDLIBS} -o ${@}

${local_bindir}/observer: src/observer.o ${observer_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} ${LDFLAGS} $^ ${LDLIBS} -o ${@}

${local_bindir}/%: %.o ${objs}
	@-mkdir -p ${@D}
	${CXX} ${LDFLAGS} $^ ${LDLIBS} -o ${@}

clean: Makefile
	${RM} *.o *~ ${objs} */*.o

cleanall: clean
	rm -f ${client} ${server}
	rm -rf ${local_bindir}

distclean: cleanall
	-rm iotivity

dist: ${tarball}
	ls -l $<

${tarball}: ${CURDIR} distclean
	cd ${<} && tar cfz "$@" \
 --transform "s|^./|${name}-${version}/|" \
 --exclude 'debian' --exclude-vcs \
 ./
	ls -l $@

install: ${exes} ${dat_files}
	install -d ${install_dir}/bin
	install -m755 $^ ${install_dir}/bin

rule/systemd/install: ${tmpdir}/${name}.service
	install -d ${DESTDIR}${unitdir}
	install -m644 $< ${DESTDIR}${unitdir}

${tmpdir}/${name}.service: extra/systemd/iotivity-example.service
	-mkdir -p ${@D}
	sed -e "s|ExecStart=.*|ExecStart=${extradir}/${name}/bin/server|g" < $< > $@

iotivity: ${include_dir}
	@echo "# TODO: workaround for namespace"
	-@rm -f $@
	ls $</iotivity && ln -fs $</iotivity $@ || ln -fs $< $@
	ls -l $@

${srcs_all}: ${iotivity_dir}
exe_args?=\
 LD_LIBRARY_PATH=${iotivity_out_dir} \
 stdbuf -oL -eL \
 ${exe} 

run/%: ${local_bindir}/% ${dat_files}
	@mkdir -p ${log_dir}
	${exe_args} ${<D}/${<F} ${run_args} \
 | tee ${log_dir}/${@F}

xterm/% : ${local_bindir}/%
	xterm -T "${@F}" -e ${MAKE} run/${@F} &
	sleep 5

run: run/server
	@echo "$@: $^"

stop:
	-killall client server observer
	-killall -9 client server observer

force/%:
	make ${@F}

auto: all xterm/server  run/client-auto force/stop
	@echo "$@: $^"

demo: stop all xterm/server run/observer force/stop
	@echo "$@: $^"

help: README.md
	cat $<
	@echo "# type make longhelp for more"

longhelp:
	@echo "# package=${package}"
	@echo "# iotivity_dir=${iotivity_dir}"
	@echo "# all=${all}"
	@echo "# config_pkgconfig=${config_pkgconfig}"
	set
