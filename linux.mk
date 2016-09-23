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
	date

package?=iotivity-example

local_bindir?=bin
local_bindir?=opt
DEST_LIB_DIR?=${DESTDIR}${local_optdir}/${package}/

config_pkgconfig?=1


ifeq (${config_pkgconfig},1)
iotivity_dir?=$(PKG_CONFIG_SYSROOT_DIR)/usr/include/iotivity
iotivity_out?=$(PKG_CONFIG_SYSROOT_DIR)/usr/lib
CPPFLAGS+=$(shell pkg-config iotivity --cflags)
else
include iotivity.mk
LDFLAGS+=-L${iotivity_out}
#LIBS+=${iotivity_libs}
CPPFLAGS+=${iotivity_includedirs_cflags}
#CPPFLAGS+=-pthread
endif

LIBS+=-loc_logger -loctbstack
LIBS+=-lconnectivity_abstraction

ifeq (${PLATFORM}, TIZEN)
CPPFLAGS+=-D__TIZEN__=1 
CPPFLAGS+=$(shell pkg-config dlog --cflags)
LIBS+=$(shell pkg-config dlog --libs)
else
CPPFLAGS+=-I${iotivity_dir}/resource/stack
CPPFLAGS+=-I${iotivity_dir}/resource/c_common
endif
CFLAGS+=-fPIC

# for artik10
#CPPFLAGS+=-DCONFIG_GPIO=22
#CPPFLAGS+=-DCONFIG_WANT_GPIO=1

client?=${local_bindir}/client
server_objs?=
server?=${local_bindir}/server
client_objs?=

all+=${server}
all+=${client}
all+=${observer}

all: ${all}

clean:
	-rm -f *.o *~ ${objs} */*.o
	find . -iname "*~" -exec rm '{}' \;
	find . -iname "*.o" -exec rm -v '{}' \;

distclean: clean
	rm -rf bin typescript build*
	rm -f ${client} ${server}

install: ${all}
	install -d ${DEST_LIB_DIR}
	install $^ ${DEST_LIB_DIR}

libs: ${iotivity_out}
	@echo "sudo dpkg -r iotivity iotivity-dev"

${local_bindir}/server: src/server.o ${server_objs} ${objs} ${libs}
	@-mkdir -p ${@D}
	${CC} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/client: src/client.o ${client_objs} ${objs} ${libs}
	@-mkdir -p ${@D}
	${CC} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/%: %.o ${objs}
	@-mkdir -p ${@D}
	${CC} -o ${@} $^ ${LDFLAGS} ${LIBS}

run/%: ${CURDIR}/bin/%
	mkdir -p ${log_dir}
	cd ${iotivity_out} && \
LD_LIBRARY_PATH=. stdbuf -oL -eL ${exe} ${<D}/${<F} \
| tee ${log_dir}/${@F}


xterm/% : bin/%
	xterm -e ${MAKE} run/${@F}  &
	sleep 5

run: xterm/server xterm/client

scons_flags+=TARGET_OS=linux TARGET_ARCH=${arch}
