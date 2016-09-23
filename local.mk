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


local/reset:
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
