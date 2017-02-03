#!/bin/sh
set -x
SELF=$0
SELFDIR=$(dirname -- "$SELF")/../js/
cd $SELFDIR


log_()
{
    echo "$@                                                  " | \
        cut -b1-16 | \
        node -i lcd.js
}

main_()
{
    node illuminance-server.js > /dev/null &
    pid=$!

    node main.js \
        | /opt/iotivity-example-gpio/client

    kill $pid
}

[ "" != "$1" ] || main_

$@
