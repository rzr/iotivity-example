sync:
	dropbox  status | grep 'Up to date' || sleep 10 
	dropbox  status | grep 'Up to date' || make sync


ci:
	while [ ! -r "$log" ] ; do ${MAKE} log || : ; sleep 10 ; done

wait/%:
	while [ ! -r "log-philippe.txt" ] ; do sleep 10 ; ${MAKE} -f arduino.mk log || :  ; done

philippe: wait/philippe


top/local/help:
	ls -l ${iotivity_libs}
	@echo "LOCAL_OBJS=${LOCAL_OBJS}"
	@echo "LIBS=${LIBS}" | tr ' ' '\n'
	@echo "USER_LIB_PATH=${USER_LIB_PATH}"


local/%: arduino/% linux/%
	date


todo/local/reset:
	rm -rf *
	make sync
	git checkout .

rebuild: local/reset log
	sync


log?=${log_dir}/all

log: distclean ${log}
	date

demo_platform=all

${log}:
	make distclean 
	mkdir -p ${@D}
	script -e -c "make demo/${demo_platform}" $@

log/%:
	mkdir -p "${log_dir}/${@D}/${@}.tmp"
	script -e -c "make demo/{demo_platform}" "${log_dir}/$@"


local/setup: ${iotivity_out}
	sudo addgroup ${USER} dialout
	sudo chown ${USER} /dev/ttyACM*
	groups
	ls -l /dev/ttyACM*

local/help:
	ls -l ${iotivity_libs}
	@echo "LOCAL_OBJS=${LOCAL_OBJS}"
	@echo "LIBS=${LIBS}" | tr ' ' '\n'
	@echo "USER_LIB_PATH=${USER_LIB_PATH}"
	find ${iotivity_out} -iname "lib*.a"



CPPFLAGS += -DCONFIG_GPIO_PIN=13
CPPFLAGS += -DCONFIG_MAC_1=0x90
CPPFLAGS += -DCONFIG_MAC_2=0xA2
CPPFLAGS += -DCONFIG_MAC_3=0xDA

ifeq (${user}, philippe)
# 90:a2:da:f9:97:b3
CPPFLAGS += -DCONFIG_MAC_4=0xF9
CPPFLAGS += -DCONFIG_MAC_5=0x97
CPPFLAGS += -DCONFIG_MAC_6=0xB3
MONITOR_PORT=/dev/ttyUSB0
MONITOR_RATE?=9600
else
CPPFLAGS += -DCONFIG_MAC_4=0x0F
CPPFLAGS += -DCONFIG_MAC_5=0x97
CPPFLAGS += -DCONFIG_MAC_6=0xB3
endif
# 

#192.168.0.37       aes.lan 90A2DA0F97B3.local 37.todo WIZnet0F97B3 # ardunio at mega sheild


/dev/ttyACM0: ${MONITOR_PORT}
	sudo ln -fs $< $@

local/console: ${MONITOR_PORT}
	screen $< ${MONITOR_RATE}

local/test: ${iotivity_dir} /dev/ttyACM0
	cd $< && \
 scons \
 TARGET_OS=arduino \
 UPLOAD=1 \
 BOARD=mega \
 #eol


# /usr/share/arduino/hardware/tools/avrdude
# -q -V -D -p atmega2560 -C /usr/share/arduino/hardware/tools/avrdude.conf -c wiring -b 115200 -P /dev/ttyUSB1 \
                        -U flash:w:build-mega2560/iotivity-example.hex:i

# /home/philippe/usr/local/src/iotivity-1.2-rel/extlibs/arduino/arduino-1.5.8/hardware/tools/avr/bin/avrdude -C/home/philippe/usr/local/src/iotivity-1.2-rel/extlibs/arduino/arduino-1.5.8/hardware/tools/avr/etc/avrdude.conf -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:/home/philippe/usr/local/src/iotivity-1.2-rel/out/arduino/avr/release//resource/csdk/stack/samples/arduino/SimpleClientServer/ocserver/SimpleClientServer.hex:i
