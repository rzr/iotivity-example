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

default/default: default/all
	date

config_pkgconfig?=1

ifeq (${config_pkgconfig},1)
iotivity_dir?=$(PKG_CONFIG_SYSROOT_DIR)/usr/include/iotivity
iotivity_out?=$(PKG_CONFIG_SYSROOT_DIR)/usr/lib
iotivity_cppflags+=$(shell pkg-config iotivity --cflags)
iotivity_cppflags+=-I${iotivity_dir}/..
else
include iotivity.mk
LDFLAGS+=-L${iotivity_out}
endif
CPPFLAGS+=${iotivity_cppflags}
LIBS+=-loc_logger
LIBS+=-loctbstack
LIBS+=-lconnectivity_abstraction

CFLAGS+=-fPIC
CPPFLAGS+=-Isrc

client?=${local_bindir}/client
server_objs?=src/server/platform/default/platform.o
server?=${local_bindir}/server
client_objs?=

all+=${server}
all+=${client}
all+=${observer}


default/all: ${all}
	@echo "# $@: $^"

default/clean:
	-rm -f *.o *~ ${objs} */*.o

default/cleanall: default/clean
	-rm -rf bin
	-rm -f *.o *~ ${objs} */*.o

default/distclean: default/cleanall
	rm -rf typescript build*
	rm -f ${client} ${server}

${local_bindir}/server: src/server.o ${server_objs} ${objs} ${libs}
	@-mkdir -p ${@D}
	${CC} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/client: src/client.o ${client_objs} ${objs} ${libs}
	@-mkdir -p ${@D}
	${CC} -o ${@} $^ ${LDFLAGS} ${LIBS}

${local_bindir}/%: %.o ${objs}
	@-mkdir -p ${@D}
	${CC} -o ${@} $^ ${LDFLAGS} ${LIBS}

default/libs: ${iotivity_out}
	@echo "sudo dpkg -r iotivity iotivity-dev"

${platform}/demo: platform/demo
	@echo "# $@: $^"
