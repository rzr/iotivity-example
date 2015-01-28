#! /usr/bin/make -f
# //******************************************************************
# //
# // Copyright 2014 Intel Corporation.
# // Copyright 2015 Eurogiciel <philippe.coval@eurogiciel.fr>
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

package?=iotivity-example

default: all

CXXFLAGS+=-std=c++11 -I/usr/include/iotivity -I.

LIBS+= -loc -loc_logger -loctbstack -lcoap
LIBS+=-lmraa

bindir?=bin
vpath+=src
VPATH+=src
srcs?=config.cpp sensors.cpp
objs?=${srcs:.cpp=.o}
client?=${bindir}/client
server?=${bindir}/server
DEST_LIB_DIR?=${DESTDIR}/usr/lib/${package}/

all: ${server} ${client}

run: ${client}
	${<D}/${<F}

bin/%: %.o ${objs}
	@-mkdir -p ${@D}
	${CXX} -o ${@} $^ ${LDFLAGS} ${LIBS}

clean:
	rm -f *.o *~ ${objs} */*.o

distclean: clean
	rm -f ${client} ${server}

install:${client} ${server}
	install -d ${DEST_LIB_DIR}
	install $^ ${DEST_LIB_DIR}
