#!/bin/sh
set -x

cat<<EOF

https://my.artik.cloud/data

EOF
cd /usr/lib/node_modules/iotivity-node/iotivity-example/js
killall node ||:

node bh1750-server-lowlevel.js  &

sleep 10

node main.js
