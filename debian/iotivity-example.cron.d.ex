#
# Regular cron jobs for the iotivity-example package
#
0 4	* * *	root	[ -x /usr/bin/iotivity-example_maintenance ] && /usr/bin/iotivity-example_maintenance
