setup_()
{
    url=https://github.com/tizenteam/iotivity
    git clone $url 
    cd iotivity
}


cloud_()
{
    pwd=$PWD

    server=true
    [ "" = "$1" ] || server=false

    if $server ; then
        git checkout sandbox/pcoval/on/cloud-interface/devel
        #git.sh op rm
        
        app.sh -e docker_
        sudo apt-get install docker-engine # upstream
        
        sudo service apache2 stop
        sudo service docker restart
        sudo service docker status
        
        cd cloud && \
            sudo /usr/local/bin/docker-compose up 2>&1 \
            | tee "docker-compose-up.log"
    fi

    cd $pwd
 
    killall xterm ||:

    build=false
    find . -iname '*controller' || build=true

    build=true

    if $build ; then
        git.sh op rm
        git fetch --all
        git checkout sandbox/pcoval/on/master/upstream
    
        rm -rfv extlibs/tinycbor/tinycbor 
        cborRevision='v0.3.2'
        cborRevision='v0.4'
        
        git clone https://github.com/01org/tinycbor.git extlibs/tinycbor/tinycbor -b ${cborRevision}

        git clone https://github.com/ARMmbed/mbedtls.git extlibs/mbedtls/mbedtls 

        scons \
            cloud/samples/client/ \
            WITH_TCP=yes \
            RELEASE=yes \
            TARGET_TRANSPORT=IP WITH_CLOUD=yes WITH_MQ=PUB,SUB
    fi

    auth_url='https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F'
    x-www-browser $auth_url

    echo "? code from url code=?[deadbeef]"
    read code

#   url=137.116.207.78:5683 #@ondrejtomcik
    iface=docker0
    host=$(/sbin/ifconfig $iface \
        | awk '/inet addr/{split($2,a,":"); print a[2]}' \
        || echo 127.0.0.1)
    port=5683
    url=$host:$port

    cd ./out/linux/x86_64/release/cloud/samples/client/
    auth=github

    cat<<EOF>aircon_controlee-auth.sh
./aircon_controlee $url $auth $code 2>&1 | tee "\$0.log"
EOF
    xterm -e "bash ./aircon_controlee-auth.sh" &

    pid=$?

    if false ; then
        sleep 10
        killall aircon_controlee

        uuid=$(grep "uid:" aircon_controlee-auth.sh.log \
            | awk '{ print $2 }' )
        token=$(grep "accesstoken:" aircon_controlee-auth.sh.log \
            | awk '{ print $2 }' )

        cat<<EOF>aircon_controlee-control.sh
./aircon_controlee $url $uuid $token 1 2>&1 | tee "\$0.log"
EOF
    xterm -e "bash ./aircon_controlee-control.sh" &
    sleep 10
   fi

    if true ; then
        x-www-browser "$auth_url"
        echo "? code"
        read code

        cat<<EOF>aircon_controller-auth.sh
./aircon_controller $url $auth $code 2>&1 | tee "\$0.log"
EOF
        xterm -e "$SHELL ./aircon_controller-auth.sh" &

        sleep 10

        uuid=$(grep "uid:" aircon_controller-auth.sh.log \
            | awk '{ print $2 }' )
        token=$(grep "accesstoken:" aircon_controller-auth.sh.log \
            | awk '{ print $2 }' )
    fi
    
    if false ; then
        killall aircon_controller

        cat<<EOF>aircon_controller-control.sh
./aircon_controller $url $uuid $token 1 2>&1 | tee "\$0.log"
EOF
        xterm -e "$SHELL aircon_controller-control.sh" &
    fi

    exit 0
    
    which mvn || sudo apt-get install maven
    # sudo apt-get install mongodb mongodb-dev libboost-dev

# https://wiki.iotivity.org/doku.php?id=iotivity_cloud_-_programming_guide

    cd ~/mnt/iotivity
    git checkout sandbox/pcoval/on/next/tizen


    sudo docker pull opencf/iotivity-resourcedirectory
    sudo docker pull opencf/iotivity-accountserver
    sudo docker pull opencf/iotivity-messagequeue
    sudo docker pull opencf/iotivity-interface

    sudo docker run opencf/iotivity-accountserver

    sudo 
    exit 0

https://docs.docker.com/compose/gettingstarted/


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


ls iotivity || setup_
cloud_

