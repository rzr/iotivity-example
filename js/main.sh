#!/bin/sh
set -x

log_()
{
    echo "$@                                                  " | \
        cut -b1-16 | \
        node -i lcd.js
}

main_()
{
    cd /usr/lib/node_modules/iotivity-node/iotivity-example/js

    # wait all: agl
    log_ "@TizenHelper"

    node illuminance-server.js > /dev/null &
    node main.js \
        | /opt/iotivity-example-gpio/client
}
[ "" != "$1" ] || main_

$@
