#!/bin/sh

set -x
SELF=$0
SELFDIR=$(dirname -- "$SELF")/../js/
cd $SELFDIR


usage_()
{
    cat<<EOF
URL: http://wiki.iotivity.org/automotive
Contact: https://wiki.tizen.org/wiki/User:Pcoval
Status: WIP

systemctl status iotivity-example-demo

systemctl stop iotivity-example-demo
sh /usr/lib/node_modules/iotivity-node/iotivity-example/extra/iotivity-example-demo.sh

TODO: 
client on linux pi, server on arduino:
error: in POST response: 132

cd /usr/lib/node_modules/iotivity-node/iotivity-example/
git commit -sam WIP && git push

systemctl restart iotivity-example-demo
journalctl -f
EOF
}


stop_()
{    
    systemctl stop iotivity-example
    systemctl stop iotivity-example-csdk
#   systemctl stop iotivity-example-demo
    systemctl stop iotivity-example-geolocation
    systemctl stop iotivity-example-gpio
    systemctl stop iotivity-example-mraa
    systemctl stop iotivity-example-number
    systemctl stop iotivity-example-switch

    killall server node

    ps | grep iotivity ||:
    ps | grep ocf ||:
    ps | grep node ||:
}


kill_()
{
    systemctl stop iotivity-example-demo
    stop_
}


download_()
{
    mkdir -p /usr/local/bin/
    cd /usr/local/bin/

    ping -c 1 githubusercontent.com \
        || ping -c 1 8.8.8.8 \
        && echo 'nameserver 8.8.8.8' \
        | sudo tee -a /etc/resolv.conf

    url='https://raw.githubusercontent.com/TizenTeam/iotivity-example/sandbox/pcoval/demo/extra/iotivity-example-demo.sh'
    curl "$url" > iotivity-example-demo.sh.tmp \
        && mv iotivity-example-demo.sh.tmp iotivity-example-demo.sh
    chmod a+rx iotivity-example-demo.sh
}



enable_()
{
    unit=iotivity-example-demo
    exe=/usr/lib/node_modules/iotivity-node/$unit/extra/$unit.sh
    service=/usr/local/share/$unit.service
    chmod a+rx $exe
    chmod a-x /lib/systemd/system/*.service $service

    mkdir -p /usr/local/share && cat<<EOF | tee "$service"
[Unit]
Description=$unit
After=network.target

[Service]
ExecStart=${exe}
Restart=always

[Install]
WantedBy=default.target
EOF

    systemctl disable $service ||:
    systemctl stop $unit ||:
    systemctl enable $service
    systemctl start $unit
    systemctl status $unit

    ps | grep $unit
    ps | grep iotivity

}

fetch_()
{
    # TODO: Unpackaged demo
    cd /usr/lib/node_modules/iotivity-node/
    url=http://github.com/TizenTeam/iotivity-example
    branch=sandbox/pcoval/demo
    unit=iotivity-example-demo
    rm -rfv /usr/lib/node_modules/iotivity-node/$unit
    git config --global user.email "iotivity-example-demo@localhost"
    git config --global user.name "iotivity-example-demo developer"
    git clone $url -b $branch $unit
    cd /usr/lib/node_modules/iotivity-node/$unit/js
    chmod a+rx *.sh
}


setup_()
{
    sudo="sudo"
    which $sudo || sudo=""

    # Stop
    stop_
    systemctl disable iotivity-example ||:
    systemctl disable iotivity-example-csdk ||:
    systemctl disable iotivity-example-demo ||:
    systemctl disable iotivity-example-geolocation ||:
    systemctl disable iotivity-example-gpio ||:
    systemctl disable iotivity-example-mraa ||:
    systemctl disable iotivity-example-number ||:
    systemctl disable iotivity-example-switch ||:
    systemctl stop iotivity-example-demo ||:
    systemctl disable iotivity-example-demo ||:

    # TODO: patch OS
    # https://jira.automotivelinux.org/browse/SPEC-366
    cd /
    file=etc/systemd/system/sysinit.target.wants/psplash-start.service
    if [ -e "$file" ] ; then
        $sudo systemctl disable psplash-start
        $sudo rm -v "$file"
    fi

    fetch_

    # TODO: Unpackaged depandencies for demo
    npm install lcd
    npm install bh1750
    npm install node-rest-client


    enable_
}


bootstrap_()
{
    download_
    setup_
    sudo reboot
}


log_()
{
    f="lcd.js"
    if [ -r "$f" ] ; then

        echo "$@                                                  " | \
            cut -b1-16 | \
            node -i lcd.js
    else
        echo "$@"
    fi
}


illuminance_()
{
    pattern="illuminance"
    log_ "{ $pattern:"

    i2cdetect -y 1

    cd /usr/lib/node_modules/iotivity-node/iotivity-example/js/
    log=$(mktemp)
    stdbuf -oL node bh1750.js | stdbuf -oL tee $log &
    for i in $(seq 1 10); do
        line=$(grep $pattern $log \
            | grep -o '[0-9]*' | tail -n1 \
            || echo "0")
        sleep 1
        log_ "$line @ $pattern"
        [ 8 = $i ] && killall node
        [ 0 -ge 0$line ] || {
            killall node;
            return;
        }
    done
    log_ "} $pattern $line"
}


geolocation_()
{
    pattern="GeolocationResURI"
    log_ "{ $pattern:"

    log_ "Geolocation"

    systemctl restart iotivity-example-geolocation
    sleep 1
    found=false

    pattern='/opt/iotivity-example-geolocation/server'
    ps auxf |  grep -v grep | grep "$pattern" && found=true

    log="$(mktemp)"
    while ! $found ; do
        log_ $pattern
        found=false
        /opt/iotivity-example-gpio/client -v 2>&1 \
            | stdbuf -oL grep --line-buffered -m 1 "$pattern" \
            | head -n1 | tee "$log" &

        sleep 1
        grep "$pattern" "$log" && found=true ||:
        killall client
        log_ "0: $pattern"
    done
    log_ "} $pattern $line"
}


binaryswitch_()
{
    h=192.100.0.10
    pattern='log: Resource: uri: /BinarySwitchResURI'
    pattern="BinarySwitchResURI"
    log_ "{ $pattern $line"
    found=false
    log="$(mktemp).$pattern"
    rm -fv "$log"

    while ! $found ; do
        log_ ping $h
        
        ping -c 1 $h \
            || while [ 0 != $? ] ; do
            ping -c 1 $h \
                ||:
        done
#       found=true # TODO
        line="1: $h $found"
        log_ "$line"
        sleep 1
        if ! $found ; then
            /opt/iotivity-example-gpio/client -v 2>&1 \
                | stdbuf -oL grep --line-buffered -m 1 "$pattern" \
                | head -n1 | tee "$log" &

            sleep 4
            grep "$pattern" "$log" && declare found=true ||:
            killall client
        fi

        #        line=$(/opt/iotivity-example-gpio/client -v 2>&1 \
        #           | stdbuf -oL grep -rl "$pattern" | head -n1 )\
        #

        #        sleep 5
        log_ "0: $pattern"
    done
    log_ "1: $pattern"
    log_ "} $pattern $line"
}


check_()
{
    cd /usr/lib/node_modules/iotivity-node/iotivity-example/js

    if false ; then
        cd /usr/lib/node_modules/iotivity-node/iotivity-example/js
        # LCD
        node lcd.js "Testing LCD" &
        pid=$!
        sleep 5
        killall $pid ||:
        
        # i2c
        i2cdetect -y 1 # on rpi2+
        app=bh1750.js
        node $app &
        pid=$?
        sleep 5
        killall $pid ||:
        
        # ARTIK
        node artik-rest.js &
        pid=$?
        sleep 5
        killall $pid ||:
    fi

    # wait all: agl
    log_ "@TizenHelper"

    grep '^NAME=' /etc/os-release | cut -d'"' -f 2 | node lcd.js || sleep 10

    log_ "Stop"
    stop_
    killall node ||:

    illuminance_

    log_ "host?"
    get_host_
    log_ $host
    
    f=/etc/resolv.conf 
    [ ! -r $f.orig ] || cp $f $f.orig
    
    echo 'nameserver 8.8.8.8' | tee $f

    l="api.artik.cloud $host 8.8.8.8 52.86.204.150   208.67.222.222"
    for h in $l; do
        #h="8.8.8.8"
        line="0: $h"
        log_ "$line"
        ping -c 1 $h && line="1: $h"
        log_ "$line"
    done

#   geolocation_


    log_ "binaryswitch"
    binaryswitch_

    log_ "IoTivity.org"
    log_ "FOSDEM.org/2017"
}


journal_()
{
    journalctl -f &
    pid=$!
    sleep 10
    killall $! ||:
}


loop_()
{

    [ "" != "$1" ] || while true ; do
        main_
        sleep 10
    done

}


get_host_()
{
    pattern="host"
    log_ "{ $pattern"
    host="127.0.0.1"
    iface=eth0

    host=$(/sbin/ifconfig $iface \
        | grep -v '^lo' \
        | awk '/inet addr/{split($2,a,":"); print a[2]}' | head -n 1 \
        || echo 127.0.0.1 \
        )
    
    while [ "127.0.0.1" = "$host" ] ; do
        log_ "dchp? $iface $host"
        sleep 5
        dhclient eth0
        host=$(/sbin/ifconfig $iface \
            | grep -v '^lo' \
            | awk '/inet addr/{split($2,a,":"); print a[2]}' | head -n 1 \
            || echo 127.0.0.1 \
            )
        log_ "$host"
    done
    log_ "$host"
    log_ "} $pattern: $host"
}


ping_()
{
    get_host_

    h=$host

    h=$(echo $h | sed -e "s|\(.*\..*\..*\)\..*|\1\.|g")
    for i in $(seq 1 255); do
        ping -c 1 $h$i && log_ "ping: $i"
    done
}


journal_()
{


    # journalctl | awk 'match($0, /.*"illuminance": ([0-9]+)/, matches) {print matches[1]}' |  node -i ~/cat.js


    while true ; do
        journalctl -f \
            | awk 'match($0, /.*"illuminance": ([0-9]+)/, matches) {print matches[1]}' \
            | node -i lcd.js
        sleep 1
    done
}


run_()
{
    cd /usr/lib/node_modules/iotivity-node/iotivity-example/js

    sh -x main.sh

    exit 0
    node illuminance-server.js > /dev/null &

    node main.js \
        | /opt/iotivity-example-gpio/client

    exit 0
    while true ; do
        node main.js \
            | while true ; do
            /opt/iotivity-example-gpio/client \
                | grep error && kill client ||:
            sleep 1
        done
    done
}


main_()
{
    log_ "{ main"
    #   download_
    #   bash -x ~/iotivity-example-demo.sh setup_
    check_

    #    sh -x /usr/lib/node_modules/iotivity-node/iotivity-example/js/main.sh

    # wait all: agl
    log_ "OCF Automotive"

    run_

    log_ "} main"
}

###

[ "" != "$1" ] || main_
$@

