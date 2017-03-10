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

#TODO: workaround missing /usr/include/iotivity namespace
iotivity_dir?=iotivity
includedir?=/usr/include
include_dir?=${PKG_CONFIG_SYSROOT_DIR}/${includedir}
log_dir?=${CURDIR}/tmp/log

ifeq (${config_pkgconfig},1)
CPPFLAGS+=$(shell pkg-config iotivity --cflags)
LIBS+=$(shell pkg-config iotivity --libs)
iotivity_dir=${include_dir}/iotivity
else
arch?=$(shell uname -m)
os?=linux
build_dir?=debug
iotivity_out_dir?=${iotivity_dir}/out/${os}/${arch}/${build_dir}/
iotivity_include_dir?=${iotivity_out_dir}/include
LDFLAGS+=-L${iotivity_out_dir}
LIBS+=-loc -loctbstack -lconnectivity_abstraction -loc_logger 
LIBS+=-lc_common
iotivity_include_dir?=${iotivity_dir}/
CPPFLAGS+=-I${iotivity_include_dir}
CPPFLAGS+=-I${iotivity_include_dir}/resource
CPPFLAGS+=-I${iotivity_include_dir}/c_common
CPPFLAGS+=-I${iotivity_include_dir}/resource/oc_logger
CPPFLAGS+=-I${iotivity_include_dir}/resource/stack
all+=${iotivity_dir}
srcs_all+=$(wildcard src/*.cpp)
endif

all+=${name}.service
CPPFLAGS+=-I.

DESTDIR?=/
local_bindir?=bin
optdir?=/opt
install_dir?=${DESTDIR}${optdir}/${name}/
_unitdir?=/usr/lib/systemd/system/
log_dir?=${CURDIR}/tmp/log

vpath+=src
VPATH+=src

CPPFLAGS+=-DCONFIG_SERVER_MAIN=1
CPPFLAGS+=-DCONFIG_CLIENT_MAIN=1
CPPFLAGS+=-DCONFIG_OBSERVER_MAIN=1

CPPFLAGS+=-g -O0
run_args?=-v

V=1

CXXFLAGS+=-std=c++0x

srcs?=platform.cpp common.cpp
objs?=${srcs:.cpp=.o}

client?=${local_bindir}/client

server?=${local_bindir}/server

observer?=${local_bindir}/observer

exes?=${client} ${observer}
exes+=${server}
all+=${exes}


${local_bindir}/server: server.o ${server_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/client: client.o ${client_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/observer: observer.o ${observer_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

all: ${all}

${local_bindir}/%: %.o ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

clean:
	rm -f *.o *~ ${objs} */*.o

cleanall: clean
	rm -f ${client} ${server}
	rm -rf ${local_bindir}

distclean: cleanall
	-rm iotivity

install: ${exes}
	install -d ${install_dir}/bin
	install $^ ${install_dir}/bin

rule/install/service: ${name}.service
	install -d ${DESTDIR}${_unitdir}/
	install $< ${DESTDIR}${_unitdir}/

${name}.service: extra/iotivity-example.service
	sed -e "s|ExecStart=.*|ExecStart=${optdir}/${name}/bin/server|g" < $< > $@

iotivity:
	@echo "# TODO: workaround for namespace"
	-@rm -f $@
	ls ${include_dir}/iotivity && ln -fs ${include_dir}/iotivity $@ || ln -fs ${include_dir} $@
	ls -l $@

rule/pkg-config/0:
	@echo "TODO"
	-ln -fs ${HOME}/mnt/iotivity iotivity
	-ln -fs . ${iotivity_out_dir}/include/iotivity ||:

iotivity: 


${srcs_all}: ${iotivity_dir}

#exe_args?=stdbuf -oL -eL ${exe} 

exe_args?=\
 LD_LIBRARY_PATH=${iotivity_out_dir} \
 stdbuf -oL -eL \
 ${exe} 

run/%: ${local_bindir}/%
	@mkdir -p ${log_dir}
	${exe_args} ${<D}/${<F} ${run_args} \
 | tee ${log_dir}/${@F}

xterm/% : ${local_bindir}/%
	xterm -T "${@F}" -e ${MAKE} run/${@F} &
	sleep 5

run: run/server

auto: all xterm/server  run/client-auto
	killall client server

demo: all xterm/server run/client
	killall client server

check: rule/check

rule/check: 
	@rm -fv ${log_dir}/*
	${MAKE} demo &
	sleep 30
	killall client server
	grep 'ERROR:' ${log_dir}/* && exit 1 || exit 0


rule/demo: all
	{ sleep 5 ; make xterm/client ; } &
	${MAKE} run/server
	killall client server

help: README.md
	cat $<
	@echo "# type make longhelp for more"

longhelp:
	@echo "# iotivity_dir=${iotivity_dir}"
	@echo "# all=${all}"
	@echo "# config_pkgconfig=${config_pkgconfig}"
	@echo "# exe_args=${exe_args}"
	set
