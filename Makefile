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

CPPFLAGS+=$(shell pkg-config iotivity --cflags)
LDFLAGS+=$(shell pkg-config iotivity --libs --ldflags)
V=1

IOTIVITY_DIR=${CURDIR}/iotivity
IOTIVITY_DIR?=$(PKG_CONFIG_SYSROOT_DIR)/usr/include/iotivity

CPPFLAGS+=\
 -I$(IOTIVITY_DIR) \
 -I$(IOTIVITY_DIR)/resource/ \
 -I$(IOTIVITY_DIR)/resource/c_common \
 -I$(IOTIVITY_DIR)/resource/oc_logger \
 -I$(IOTIVITY_DIR)/resource/stack \
 -I. \
 #eol

CXXFLAGS+=-std=c++0x

LIBS+= -loc -loc_logger -loctbstack

srcs?=config.cpp
objs?=${srcs:.cpp=.o}

client?=${local_bindir}/client
server_objs?=
server?=${local_bindir}/server
client_objs?=
observer?=${local_bindir}/observer

all?=${client} ${observer}

all+=${server}

${local_bindir}/server: server.o ${server_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/client: client.o ${client_objs} ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

all: setup ${all}

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

iotivity: ${IOTIVITY_DIR}

${IOTIVITY_DIR}: $(PKG_CONFIG_SYSROOT_DIR)/usr/include
	ls $</iotivity && ln -fs $</iotivity $@ || ln -fs $< $@

setup: iotivity
	ls $</resource

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
