iotivity_url?=http://github.com/iotivity/iotivity
iotivity_version?=1.1.1
#iotivity_version=1.1.0

#overide here
#iotivity_version=master
iotivity_version=1.2-rel

tinycbor_url?=https://github.com/01org/tinycbor.git 
ifeq (${iotivity_version}, 1.1.1)
tinycbor_version?=v0.2.1
else
tinycbor_version=v0.3.1
endif

gtest_url?=http://pkgs.fedoraproject.org/repo/pkgs/gtest/gtest-1.7.0.zip/2d6ec8ccdf5c46b05ba54a9fd1d130d7/gtest-1.7.0.zip
gtest_url?=https://github.com/google/googletest/archive/release-1.7.0.zip

#maybe overriden
iotivity_dir?=${HOME}/usr/local/src/iotivity-${iotivity_version}
#iotivity_dir?=${CURDIR}/tmp/src/iotivity-${iotivity_version}

export iotivity_dir

iotivity_out?=${iotivity_dir}/out/${platform}/${arch}/${mode}

iotivity_includedirs?=\
${iotivity_dir}/resource/c_common/ \
${iotivity_dir}/resource/csdk/stack/include \
${iotivity_dir}/resource/csdk/logger/include \
 #eol

#TODO
iotivity_includedirs_cflags?=\
-I${iotivity_dir}/resource/c_common/ \
-I${iotivity_dir}/resource/csdk/stack/include \
-I${iotivity_dir}/resource/csdk/logger/include \
 #eol

iotivity_libs?=\
${iotivity_out}/resource/csdk/liboctbstack.a \
${iotivity_out}/resource/csdk/routing/libroutingmanager.a \
${iotivity_out}/resource/csdk/security/libocsrm.a \
${iotivity_out}/resource/csdk/logger/liblogger.a \
${iotivity_out}/resource/csdk/connectivity/src/libconnectivity_abstraction.a \
${iotivity_out}/resource/c_common/libc_common.a \
${iotivity_out}/libcoap.a \
 #eol

CFLAGS+=-DTB_LOG=1 -DROUTING_GATEWAY=1


${iotivity_out}: ${iotivity_dir}
#	cd ${<} && ./auto_build.sh ${platform}_unsecured_with_rd
	cd ${<} && scons resource ${scons_flags}

 # DEBUG=0 \
 # 
 #

setup: /etc/debian_version
	sudo apt-get install libboost1.55-dev libboost1.55-all-dev libboost-thread1.55-dev \
	|| sudo apt-get install libboost-dev libboost-program-options-dev libexpat1-dev libboost-thread-dev uuid-dev
	sudo apt-get install psmisc

${iotivity_dir}:
	mkdir -p ${@D}
	cd ${@D} && git clone ${iotivity_url} -b ${iotivity_version} ${@}

gtest: ${iotivity_dir}/extlibs/gtest/gtest-1.7.0.zip


${iotivity_dir}/extlibs/gtest/gtest-1.7.0.zip: 
	${MAKE} ${iotivity_dir}
	mkdir ${@D} ; cd ${@D} 
	wget -c ${gtest_url} -O ${@}

${iotivity_dir}/extlibs/gtest/gtest-1.7.0.zip/github: 
	${MAKE} ${iotivity_dir}
	mkdir ${@D} ; cd ${@D} 
	wget -c ${gtest_url} -O ${@}
	cd ${@D} && unp $@ && mv googletest-release-1.7.0 gtest-1.7.0

${iotivity_dir}/extlibs/tinycbor/tinycbor:
	${MAKE} ${iotivity_dir}
	cd ${iotivity_dir} && git clone ${tinycbor_url} -b ${tinycbor_version} "extlibs/tinycbor/tinycbor"

#TODO
iotivity_out: 
	ls ${iotivity_out} || ${MAKE} platform?=${platform} deps ${iotivity_out}
	sync

deps+=${iotivity_dir} tinycbor gtest

tinycbor: ${iotivity_dir}/extlibs/tinycbor/tinycbor

iotivity_out?=${iotivity_dir}/out/${platform}/${arch}/${mode}

all+=deps
all+=iotivity_out

scons_flags+=TARGET_TRANSPORT=IP
scons_flags+=RELEASE=1
scons_flags+=SECURED=0
#scons_flags+=LOGGING=0
#scons_flags+=DEBUG=0

#scons_flags+=LOGGING=1 


# tizen
scons_flags+=WITH_TCP=1 
scons_flags+=WITH_CLOUD=0
#scons_flags+=LOGGING=1 
scons_flags+=ROUTING=EP 
#ES_TARGET_ENROLLEE=tizen 
#LIB_INSTALL_DIR=/usr/lib 
scons_flags+=WITH_PROXY=0


scons_flags+=VERBOSE=1 

ifeq (${iotivity_version}, 1.1.1)
#scons_flags+=WITH_RD=1
endif

