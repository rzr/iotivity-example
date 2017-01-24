#!/bin/sh

set -e
set -x

arch=$(arch)


usage_()
{
cat<<EOF
https://wiki.iotivity.org/docker

https://docs.docker.com/compose/gettingstarted/

sudo addgroup $USER docker && su -l $USER


EOF
}


sources_()
{
    sudo apt-get install git scons
    url="https://github.com/tizenteam/iotivity"
    git clone $url 
}

servers_()
{

    cat<<EOF

EOF
    #git.sh op rm
    # app.sh -e docker_
    which docker || sudo apt-get install docker-engine # upstream
    which docker-compose || exit 1

    # make 80 port free to be used
    sudo service apache2 stop ||:

    sudo service docker restart
    sudo service docker status
    
    sudo=""
    groups | grep docker || sudo=sudo

    $sudo docker version # > 1.13

    branch=sandbox/pcoval/on/cloud-interface/devel
    git checkout $branch \
        || git checkout -b $branch remotes/origin/$branch

    ls $pwd/iotivity/cloud/docker-compose.yml || exit 1

    cat<<EOF | tee $pwd/iotivity-cloud.sh
#!/bin/sh
cd $pwd/iotivity/cloud
$sudo /usr/local/bin/docker-compose up \
| tee "$pwd/docker-compose-up.log" \
2>&1 

EOF
    xterm -e "bash $pwd/iotivity-cloud.sh" &
}


clients_()
{
    tls_mode=0

    #   url=137.116.207.78:5683 #@ondrejtomcik
    iface=docker0
    host=$(/sbin/ifconfig $iface \
        | awk '/inet addr/{split($2,a,":"); print a[2]}' \
        || echo 127.0.0.1)
    port=5683
    url=$host:$port
    auth=github

    auth_url='https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F'

    build=false
    cd $pwd/iotivity
    find out -iname '*controller' || build=true

    if $build ; then
        branch=sandbox/pcoval/on/master/upstream
        cborRevision='v0.3.2'
        cborRevision='v0.4'

        git checkout $branch \
            || git checkout -b $branch remotes/origin/$branch
        
        rm -rfv extlibs/tinycbor/tinycbor 
        
        git clone https://github.com/01org/tinycbor.git \
            extlibs/tinycbor/tinycbor -b ${cborRevision}

        git clone https://github.com/ARMmbed/mbedtls.git \
            extlibs/mbedtls/mbedtls 

        scons \
            cloud/samples/client/ \
            WITH_TCP=yes \
            RELEASE=yes \
            TARGET_TRANSPORT=IP \
            WITH_CLOUD=yes \
            WITH_MQ=PUB,SUB
    fi

    cd ./out/linux/${arch}/release/cloud/samples/client/ || exit 1

    x-www-browser $auth_url

    ./aircon_controlee

    echo "? code from url code=..  just type value (ie: 02d960d841569d0da36c)"
    read code    

    cat<<EOF>$pwd/aircon_controlee-auth.sh
./aircon_controlee $url $auth $code $tls_mode 2>&1 | tee "\$0.log"
EOF
    xterm -e "bash $pwd/aircon_controlee-auth.sh" &
    sleep 10;
    cat $pwd/aircon_controlee-auth.sh.log

    pid=$?

    if false ; then
        sleep 10
        killall aircon_controlee

        uuid=$(grep "uid:" aircon_controlee-auth.sh.log \
            | awk '{ print $2 }' )
        token=$(grep "accesstoken:" aircon_controlee-auth.sh.log \
            | awk '{ print $2 }' )

        cat<<EOF>$pwd/aircon_controlee-control.sh
set -x
./aircon_controlee $url $uuid $token $tls_mode  2>&1 | tee "\$0.log"
EOF
        xterm -e "bash $pwd/aircon_controlee-control.sh" &
        sleep 10
    fi

    if true ; then
        x-www-browser "$auth_url"
        echo "? code again"
        read code

        cat<<EOF>$pwd/aircon_controller-auth.sh
set -x
./aircon_controller $url $auth $code $tls_mode   2>&1 | tee "\$0.log"
EOF
        xterm -e "$SHELL $pwd/aircon_controller-auth.sh" &

        sleep 10

        uuid=$(grep "uid:" aircon_controller-auth.sh.log \
            | awk '{ print $2 }' )
        token=$(grep "accesstoken:" aircon_controller-auth.sh.log \
            | awk '{ print $2 }' )
    fi
    
    if false ; then
        killall aircon_controller

        cat<<EOF>$pwd/aircon_controller-control.sh
set -x
./aircon_controller $url $uuid $token $tls_mode  2>&1 | tee "\$0.log"
EOF
        xterm -e "$SHELL $pwd/aircon_controller-control.sh" &
    fi
}


build_()
{
    exit 1
    
    which mvn || sudo apt-get install maven
    # sudo apt-get install mongodb mongodb-dev libboost-dev

    # https://wiki.iotivity.org/doku.php?id=iotivity_cloud_-_programming_guide


    projectdir="$PWD"
    arch=$(arch || echo unknown)
    
    cd ${projectdir}/cloud/stack
    mvn install -Dmaven.test.skip=true
    
    cd ${projectdir}/cloud/resourcedirectory 
    mvn install -Dmaven.test.skip=true

    cd ${projectdir}/cloud/account 
    mvn install -Dmaven.test.skip=true

    cd ${projectdir}/cloud/messagequeue
    mvn install -Dmaven.test.skip=true

    cd ${projectdir}/cloud/interface 
    mvn install -Dmaven.test.skip=true

    cd ${projectdir}/cloud/../
    #find . -iname 'SCons*'
    git cherry-pick 10c287fa62763927619d5fb0a20c8a43f2e41e62

    cd ${projectdir}/cloud/resourcedirectory/target 
    java -jar CloudResourceDirectory-0.0.1-SNAPSHOT.jar 5684 0
    # &

    cd ${projectdir}/cloud/account/target
    java -jar CloudAccount-0.0.1-SNAPSHOT.jar 5685 0
    # & # TODO
    # 2017-01-13 19:43:30:194 [E] [CertificateConstants.java, CertificateConstants.<clinit>(), line:60] properties/config.properties (No such file or directory)        
    cd ${projectdir}/cloud/messagequeue/target
    java -jar CloudMessageQueue-0.0.1-SNAPSHOT.jar 5686 127.0.0.1 2181 127.0.0.1 9092 0

    cd ${projectdir}/cloud/interface/target
    java -jar CloudInterface-0.0.1-SNAPSHOT.jar 5683 127.0.0.1 5684 127.0.0.1 5685 127.0.0.1 5686 0



    http://www.example.com/oauth_callback/?code=c03b2439f9f3c9b23731

    cd ${projectdir}/out/linux/${arch}/release/cloud/samples/client
    ./aircon_controlee



    git checkout sandbox/pcoval/on/next/devel

    git checkout sandbox/pcoval/devel
    #git rebase -i remotes/upstream/1.2-rel
    curl -L "https://github.com/docker/compose/releases/download/1.9.0/docker-compose-$(uname -s)-$(uname -m)" | sudo tee /usr/local/bin/docker-compose
    sudo chmod +x /usr/local/bin/docker-compose
}

docker_()
{
    exit 1;

    cd ~/mnt/iotivity
    git checkout sandbox/pcoval/on/next/tizen

    sudo docker pull opencf/iotivity-resourcedirectory
    sudo docker pull opencf/iotivity-accountserver
    sudo docker pull opencf/iotivity-messagequeue
    sudo docker pull opencf/iotivity-interface

    sudo docker run opencf/iotivity-accountserver
}



main_()
{
    pwd=$PWD
    usage_
    killall xterm ||:
    
    sudo sync

    ls iotivity || sources_
    cd iotivity # && git fetch --all
    servers_
    sleep 10
    cat<<EOF
wait servers to be started, please return once ready:
# iotivity-interface_1          | press 'q' to terminate
EOF
    read t 

    clients_ 
}


[ "" != "$1" ] || main_

$@
