#!/bin/sh
set -x

cd /usr/lib/node_modules/iotivity-node/iotivity-example/js

echo "FOSDEM.org/2017" | node lcd.js

killall node ||:

echo "IoTivity.org"  | node lcd.js

node illuminance-server.js > /dev/null &

grep '^NAME=' /etc/os-release | cut -d'"' -f 2 | node lcd.js || sleep 10

echo "artik.io"  | node lcd.js

node illuminance-cloud.js

echo "@TizenHelper"  | node lcd.js
