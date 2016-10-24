#! /usr/bin/make -f

default:all

package?=iotivityexamplegeolocation
package_name?=org.example.${package}
arch?=armv7l
profile?=tizen_2_4_mobile
gbsdir?=${CURDIR}/tmp/out/iotivity-example/${profile}/tmp/gbs/tmp-GBS-${profile}_${arch}/
gbsdir?="${HOME}/tmp/gbs/tmp-GBS-${profile}_${arch}/"
rootfs="${gbsdir}/local/BUILD-ROOTS/scratch.${arch}.0/"
rpmdir="${gbsdir}/local/repos/${profile}_${arch}/${arch}/RPMS/"
devel_rpm?=$(shell ls ${rpmdir}/iotivity-devel-${version}*-*${arch}.rpm)
rpm?=$(shell ls ${rpmdir}/iotivity-[0-9]*-*${arch}.rpm)
srcs?=$(shell find src)
srcs+=$(shell find lib)
tizen?=${HOME}/tizen-studio/tools/ide/bin/tizen
compiler?=gcc
arch_familly?=arm
cert?=tizen
build_dir?=./Debug
package_version?=1.0.0
iotivity_logo_url?=https://www.iotivity.org/sites/all/themes/iotivity/logo.png

all?=shared/res/logo.png 
all+=tmp/512x512.png
all+=rpm

all: ${all}

icon.png: docs/logo.png Makefile
	convert -resize 117x117! $< $@

docs/screenshot.png: docs/logo.png
	echo cp $< $@

tmp/screenshot.jpg: docs/screenshot.png
	convert $< $@

tmp/117x177/%: %
	mkdir -p ${@D}
	convert -resize '117x117!' $< $@


tmp/512x512.png: docs/logo.png
	mkdir -p ${@D}
	convert -resize '512x512!' $< $@

tmp/512x512.jpg: docs/logo.png
	mkdir -p ${@D}
	convert -resize '512x512!' $< $@

distclean: clean
	rm -f *.wgt
	rm -rf Debug Release
	rm -rf .package-stamp .settings .sign
	rm -rf lib usr tmp

clean:
	rm -f *~

docs/logo.jpg:
	mkdir -p $@{D}
	wget -O $@.tmp ${iotivity_logo_url}
	convert $@.tmp $@


docs/logo.png: docs/logo.svg
	convert $< $@

shared/res/logo.png: tmp/117x177/docs/logo.png
	mkdir -p ${@D}
	cp $< $@

setup: rpm

${rpm}: ${rpmdir}
	ls ${rpmdir}/iotivity-[0-9]*-*${arch}.rpm

rpm: ${rpmdir} ${rpm}
	ls -l $<


ls:
	ls ${rpmdir}
	ls ${rpmdir}/iotivity-[0-9]*-*${arch}.rpm

rpmdir: ${rpmdir}

${rpmdir}: extra/setup.sh
	profile=${profile} ${SHELL} -x $<

import: ${rpm} ${rpm_devel}
	ls .tproject 
	rm -rf usr lib
	mkdir -p usr/include lib
	unp ${rpm}
	unp ${devel_rpm}
	ln -fs ${rootfs}/usr/include/boost usr/include/
	@echo "# TODO: fix this upstream"
	cp -av ${rootfs}/usr/lib/libuuid.so.1.3.0 usr/lib/libuuid1.so  ||: #TODO might not be needed
	cp -av ${rootfs}/usr/lib/libconnectivity_abstraction.so  usr/lib/ ||: #TODO might not be needed
	rm -rf lib
	ln -fs usr/lib lib


build: usr/lib usr/include ${srcs}
	@rm -rf ${build_dir}
	${tizen} cli-config -l
	${tizen} build-native -a ${arch_familly} -c ${compiler} -- .


lib usr/lib usr/include: usr
	ls -l $@

usr: import
	ls -l $@

run: tpk
	ls ${build_dir}/${package_name}-${package_version}-${arch_familly}.tpk
	${tizen} install -n ${package_name}-${package_version}-${arch_familly}.tpk -- ${build_dir}
	${tizen} run --pkgid ${package_name} -- .

check: build run
	echo "$@: OK"

tpk: build
	-rm -rf usr/lib/*.a lib/*.a lib/pkgconfig bin
	${tizen} package -t $@ -s "${cert}" -- "${build_dir}"
	ls "${build_dir}/"*.$@

