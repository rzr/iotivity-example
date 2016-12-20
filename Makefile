install_dir?=${DESTDIR}/usr/lib/node_modules/iotivity-node/extra/js

srcs?=$(wildcard js/*.js)

all: ${srcs}

install: ${srcs}
	install -d ${install_dir}
	install $^ ${install_dir}
