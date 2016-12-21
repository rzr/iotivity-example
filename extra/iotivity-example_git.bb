SUMMARY = "IoTivity Switch Example"
DESCRIPTION = "Minimalist Iotivity Client/Server application that share a resource"
HOMEPAGE = "https://github.com/TizenTeam/iotivity-example"
SECTION = "apps"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://COPYING;md5=3b83ef96387f14655fc854ddc3c6bd57"

SRCREV = "master"
SRC_URI = "git://github.com/TizenTeam/iotivity-example.git/;protocol=http;nobranch=1"

S = "${WORKDIR}/git"

inherit systemd pkgconfig

LOCAL_OPT_DIR = "/opt"

DEPENDS += " iotivity "
BDEPENDS += " iotivity-dev "

DEPENDS_${PN} += "iotivity-resource-dev iotivity-resource-thin-staticdev iotivity-service-dev iotivity-service-staticdev"

BBCLASSEXTEND = "native nativesdk"
RDEPENDS_${PN} += " iotivity-resource "

SYSTEMD_SERVICE_${PN} = "${PN}.service"
EXTRA_OEMAKE = " package=${PN} "

do_configure() {
}

do_compile_prepend() {
    export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}"
    export PKG_CONFIG="PKG_CONFIG_SYSROOT_DIR=\"${PKG_CONFIG_SYSROOT_DIR}\" pkg-config"
    export LD_FLAGS="${LD_FLAGS}"
}

do_compile() {
 cd ${S}
 LANG=C
 export LANG
 unset DISPLAY
 LD_AS_NEEDED=1; export LD_AS_NEEDED ;
 
 oe_runmake all
}

do_install() {
 export RPM_BUILD_ROOT=${D}
 cd ${S}
 LANG=C
 export LANG
 unset DISPLAY
 rm -rf ${D}
 install -d ${D}

 oe_runmake install \
  DESTDIR=${D} \
  #eol

  install -d ${D}${base_libdir}/systemd/system
  install ${S}/extra/iotivity-example.service ${D}${base_libdir}/systemd/system/${PN}.service
}

FILES_${PN} += "${LOCAL_OPT_DIR}/${PN}/*"
FILES_${PN}-dbg += "${LOCAL_OPT_DIR}/${PN}/.debug"
