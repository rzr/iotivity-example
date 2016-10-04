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

package?=iotivity-example

local_bindir?=bin
local_optdir?=opt
DEST_LIB_DIR?=${DESTDIR}/${local_optdir}/${package}/

vpath+=src
VPATH+=src


#TODO: workaround missing /usr/include/iotivity namespace
prep_tasks+=iotivity
config_pkgconfig?=1
usr_include_dir?=${PKG_CONFIG_SYSROOT_DIR}/usr/include
iotivity_dir?=${CURDIR}/iotivity
ifeq (${config_pkgconfig},1)
CPPFLAGS+=$(shell pkg-config iotivity --cflags)
LIBS+=$(shell pkg-config iotivity --libs)
else
LIBS+=-loc -loc_logger -loctbstack
CPPFLAGS+=\
 -I${iotivity_dir} \
 -I${iotivity_dir}/resource/ \
 -I${iotivity_dir}/resource/c_common \
 -I${iotivity_dir}/resource/oc_logger \
 -I${iotivity_dir}/resource/stack \
 #eol
endif
CPPFLAGS+=-I.


CPPFLAGS+=-DCONFIG_SERVER_MAIN=1
CPPFLAGS+=-DCONFIG_CLIENT_MAIN=1
CPPFLAGS+=-DCONFIG_OBSERVER_MAIN=1

V=1

CXXFLAGS+=-std=c++0x

srcs?=common.cpp
objs?=${srcs:.cpp=.o}

client_objs?=platform.o
client?=${local_bindir}/client

server_objs?=platform.o
server?=${local_bindir}/server

observer?=${local_bindir}/observer

all?=${client} ${observer}


all+=${server}

${local_bindir}/server: server.o ${server_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/client: client.o ${client_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

all: ${all}

${local_bindir}/%: %.o ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

clean:
	rm -f *.o *~ ${objs} */*.o iotivity

distclean: clean
	rm -f ${client} ${server}

install: ${all}
	install -d ${DEST_LIB_DIR}
	install $^ ${DEST_LIB_DIR}

iotivity: ${iotivity_dir}
	@echo "TODO: workaround for namespace"

${iotivity_dir}: ${usr_include_dir}
	-rm $@
	ls $</iotivity && ln -fs $</iotivity $@ || ln -fs $< $@
	ls $@/resource

prep: ${prep_tasks}

run/%: ${CURDIR}/bin/%
	${<D}/${<F}

xterm/% : bin/%
	xterm -e ${MAKE} run/${@F} &
	sleep 5

run: xterm/server xterm/client

run/server-auto:  ${CURDIR}/bin/server
	while true ; do date ; sleep 1 ;  done | $<

xterm/server-auto : bin/server
	xterm -e ${MAKE} run/${@F} &
	sleep 5

demo: xterm/server-auto xterm/client
	sync

help:
	@echo "# iotivity_dir=${iotivity_dir}"
