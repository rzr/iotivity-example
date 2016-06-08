SUMMARY = "Iotivity Example"
DESCRIPTION = "Minimalist Iotivity Client/Server application that controle single LED resource using GPIO"
HOMEPAGE = "https://notabug.org/tizen/iotivity-example"
SECTION = "apps"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://COPYING;md5=3b83ef96387f14655fc854ddc3c6bd57"

hash = "8865ad69215cbf4c04365b1f673793b48cc175df"
SRC_URI = "git://notabug.org/tizen/iotivity-example/;branch=master;tag=${hash};protocol=http"

S = "${WORKDIR}/git"

LOCAL_OPT_DIR = "/opt"
LOCAL_OPT_DIR_D = "${D}${LOCAL_OPT_DIR}"

DEPENDS += "iotivity "

config_mraa="1"
DEPENDS += "mraa"
RDEPENDS_${PN} += "mraa"

DEPENDS_${PN} += "iotivity-resource-dev iotivity-resource-thin-staticdev iotivity-service-dev iotivity-service-staticdev"

BBCLASSEXTEND = "native nativesdk"

EXTRA_OEMAKE = " \
   "

do_configure() {
}

do_compile() {
 cd ${S}
 LANG=C
 export LANG
 unset DISPLAY
 LD_AS_NEEDED=1; export LD_AS_NEEDED ;
 
 oe_runmake all \
  config_mraa=${config_mraa} 
}

do_install() {
 export RPM_BUILD_ROOT=${D}
 cd ${S}
 LANG=C
 export LANG
 unset DISPLAY
 rm -rf ${D}
 install -d ${D}
 oe_runmake \
  install \
  DESTDIR=${LOCAL_OPT_DIR_D} \
  config_mraa=${config_mraa} \
  #eol
}

FILES_${PN} += "${LOCAL_OPT_DIR}/${PN}/client"
FILES_${PN} += "${LOCAL_OPT_DIR}/${PN}/server"
FILES_${PN}-dbg = "${LOCAL_OPT_DIR}/${PN}/.debug"
RDEPENDS_${PN} += "iotivity-resource"
BBCLASSEXTEND = "native nativesdk"
