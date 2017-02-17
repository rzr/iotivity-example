name?=iotivity-example-demo
install_dir?=${DESTDIR}/opt/${name}

srcs?=$(wildcard js/*.js)

all: ${srcs}

install: ${srcs}
	install -d ${install_dir}/js
	install $^ ${install_dir}/js
	install -d ${install_dir}/bin
	install extra/${name}.sh ${install_dir}/bin/
