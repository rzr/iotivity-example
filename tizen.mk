#! /usr/bin/make -f

rule/default: local/rpm local/usr local/lib local/all
	-ls lib/*.so
	@date

### Configuration ###

app_profile?=mobile
app_profile_upcase?=$(shell echo $(app_profile) | tr a-z A-Z)
app_profile_version?=2.4
app_profile_version_alpha?=$(shell echo ${app_profile_version} | tr '.' '_')
tizen_studio_package?=${app_profile_upcase}-${app_profile_version}-NativeAppDevelopment-CLI
tizen_profile?=tizen_${app_profile_version_alpha}_${app_profile}
gbs_arch?=armv7l
gbs_profile?=tizen_${app_profile_version_alpha}_${app_profile}_${gbs_arch}
#
project_name?=iotivity-example
app_package_exe?=iotivityexample
package_version?=1.0.0
app_package_name?=org.example.${app_package_exe}
srcs+=lib
iotivity_logo_url?=https://www.iotivity.org/sites/all/themes/iotivity/logo.png
#all?=shared/res/logo.png
#all+=tmp/512x512.png
srcs+=usr


### Default ###
self?=tizen.mk
thisdir?=$(shell dirname $(realpath ${self}))
make?=${MAKE} -f ${self}
export make
MAKEFLAGS=-j1
tmpdir?=${CURDIR}/tmp
gbsdir?=${tmpdir}/out/${project_name}/${tizen_profile}/tmp/gbs/tmp-GBS-${gbs_profile}/
#gbsdir="${HOME}/tmp/gbs/tmp-GBS-${gbs_profile}/"
rootfs="${gbsdir}/local/BUILD-ROOTS/scratch.${gbs_arch}.0/"
rpmdir="${gbsdir}/local/repos/${gbs_profile}/${gbs_arch}/RPMS/"
devel_rpm?=$(shell \
 ls ${rpmdir}/iotivity-devel-${iotivity_version}*-*${gbs_arch}.rpm 2>/dev/null \
 || echo TODO)
rpm?=$(shell \
 ls ${rpmdir}/iotivity-[0-9]*-*${gbs_arch}.rpm 2>/dev/null\
 || echo TODO)
srcs?=$(shell find src lib)

tizen_helper_dir?=tmp/tizen-helper
-include ${tizen_helper_dir}/bin/mk-tizen-app.mk


### Rules ###

%: local/%
	@echo "# $@: $^"

local/%: bootstrap
	${make} ${@F}

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

${rpm}: ${rpmdir}
	ls $@

rpm: ${rpm}
	-ls ${rpmdir}/iotivity-[0-9]*-*${arch}.rpm
	ls -l $<

rpms: ${rpmdir} ${rpm} ${rpm_devel}
	ls -l $<

ls:
	ls ${gbsdir}
	ls ${rpmdir}
	ls ${rootfs}
	ls ${rpmdir}/iotivity-[0-9]*-*${arch}.rpm


${rpmdir}: tmp/rule/bootstrap
	ls -l $<

tmp/rule/bootstrap: extra/tizen/setup.sh
	profile="${tizen_profile}" arch="${gbs_arch}" ${SHELL} ${<D}/${<F}
	echo 'include ${tizen_helper_dir}/bin/mk-tizen-app.mk' > local.mk
	mkdir -p ${@D}
	touch $@

bootstrap: tmp/rule/bootstrap
	ls -l $<
	@echo "# $@: done"


rule/import: local/rpms
	ls .tproject
	rm -rf usr lib
	mkdir -p usr/include lib
	rpm -qip ${rpm}
	unp ${rpm}
	unp ${devel_rpm}
	ln -fs ${rootfs}/usr/include/boost usr/include/
	@echo "# TODO: fix this upstream"
	@echo "# TODO: might not be needed"
	cp -av ${rootfs}/usr/lib/libuuid.so.1.3.0 usr/lib/libuuid1.so  ||:
	@echo "# TODO might not be needed"
	cp -av ${rootfs}/usr/lib/libconnectivity_abstraction.so  usr/lib/ ||:
	rm -rf lib

lib: usr/lib
	ln -fs $< $@

usr/lib usr/include: usr
	ls -l $@

usr:
	ls -l $@ || ${make} rule/import

dist_files?=\
 lib Release src inc packaging docs README.md *.prop *.xml tizen.mk COPYING extra res shared \
 .project .cproject .tproject .exportMap Makefile

dist: ${dist_files}
	${make} clean
	zip -r9 ${project_name}.zip $^
