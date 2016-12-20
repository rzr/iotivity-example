#!/bin/sh
# //******************************************************************
# //
# // Copyright 2016 Samsung <philippe.coval@osg.samsung.com>
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# //
# // Licensed under the Apache License, Version 2.0 (the "License");
# // you may not use this file except in compliance with the License.
# // You may obtain a copy of the License at
# //
# //      http://www.apache.org/licenses/LICENSE-2.0
# //
# // Unless required by applicable law or agreed to in writing, software
# // distributed under the License is distributed on an "AS IS" BASIS,
# // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# // See the License for the specific language governing permissions and
# // limitations under the License.
# //
# //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

set -e
set -x

usage_()
{
    cat<<EOF
Subject: IoTivity as Native Tizen application
Info: https://wiki.iotivity.org/tizen
Contact: https://wiki.tizen.org/wiki/User:Pcoval


home=$HOME tmpdir=$HOME/tmp/$PWD profile="tizen_2_3_1_wearable" arch="armv7l" ./extra/setup.sh 

EOF
}


die_()
{
    local r=$?
    echo "error: $@"
    echo "exit: $r"
    return $r
}


setup_debian_()
{
    which apt-get \
        || die_ "TODO: port to non debian systen"

    which sudo \
        || die_ "TODO: install sudo"

    sudo apt-get install \
        curl \
        file \
        git \
        make \
        realpath \
        rpm \
        rpm2cpio \
        unp \
        wget \
        zip \
        || die_ "TODO: missing tools"

    sudo apt-get install gbs \
        || die_ "TODO: missing tools"
}


setup_fedora_()
{
    sudo yum install \
        curl \
        file \
        git \
        make \
        realpath \
        wget \
        zip \
        || die_ "TODO: missing tools"
}

setup_()
{
    which dirname \
        && which realpath \
        && which git \
        && which gbs \
        && which make \
        && return 0 \
        || echo "installing: "

    if [ -f /etc/debian_version ]; then
        setup_debian_
    elif [ -f /etc/fedora_release ]; then
        setup_fedora_
    fi
}


build_()
{
    local config_all=true
    local package="tizen-helper"
    local url="https://notabug.org/tizen/${package}"
    local branch="master"

    ls "$package" || \
        $git clone --depth 1 -b "${branch}" "${url}"

    local make="make -f ${PWD}/tizen-helper/Makefile profile.${profile}_${arch}"

    echo "# check toolchain"
    make -C "${package}" deploy

    if ${config_all} ; then
        make -C "${package}" "profile.${profile}_${arch}"
    fi

    for t in $deps; do
        if [ "$t" = "boost" ] ; then

            package="boost-jam"
            branch="tizen"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
            
            package="dos2unix"
            branch="tizen"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"

            package="boost"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            url="https://github.com/tizenorg/platform.upstream.${package}"
            if [ "tizen_2_3_1_wearable" = "$profile" ] ; then
                branch="sandbox/pcoval/tizen_2.3.1"
                url="https://github.com/tizenteam/platform.upstream.${package}"
            fi
            package=$(basename -- "${url}")
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
            fi
    done

    # Scons is not available on Tizen:2.3.1:Wearable (and earlier) yet
    for t in $deps; do
        if [ "$t" = "scons" ] ; then
            package="scons"
            branch="tizen"
            url="https://git.tizen.org/cgit/platform/upstream/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done


    for t in $deps; do
        if [ "$t" = "libx11" ] ; then
            package="libx11"
            branch="master"
            url="https://git.tizen.org/cgit/framework/uifw/xorg/lib/libx11"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done

    for t in $deps; do
        if [ "$t" = "iotivity" ] ; then

            package="iotivity"
            url="https://git.tizen.org/cgit/contrib/${package}"
            branch="tizen"
            #TODO
#           url="http://github.com/iotivity/${package}"
#           branch="master"
            url="http://github.com/tizenteam/${package}"
            branch="sandbox/pcoval/on/previous/tizen"
            args=" -b "${branch}""
#           args=" $args --depth 1"
            ls "$package" || \
                $git clone $args -- "${url}"
#           $make -C "${package}"
            cd "${package}"

            gbs_arch=${arch}
            gbs_profile=${profile}_${gbs_arch}

            gbs build -P $gbs_profile -A $gbs_arch --include-all \
                --define 'LOGGING 1' --define 'RELEASE False'
            cd -
        fi
    done

    for t in $deps; do
        if [ "$t" = "mraa" ] ; then
            # Only for example
            package="mraa"
            branch="sandbox/pcoval/tizen"
            url="https://git.tizen.org/cgit/contrib/${package}"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done

    for t in $deps; do
        if [ "$t" = "iotivity-example" ] ; then

            # Checking
            package="iotivity-example"
            url="http://github.com/tizenteam/${package}"
            branch="tizen"
            ls "$package" || \
                $git clone -b "${branch}" "${url}"
            $make -C "${package}"
        fi
    done
}


unp_()
{
    # TODO: use unp if present
    rpm2cpio "$1" | cpio -idmv
}

deploy_()
{
    ls .tproject || die_ "TODO"

    rm -rf usr lib
    mkdir -p usr lib

    unp_ ${rpmdir}/iotivity-${version}[0-9]*-*.${arch}.rpm
    unp_ ${rpmdir}/iotivity-devel-${version}[0-9]*-*${arch}.rpm

    ln -fs ${rootfs}/usr/include/boost usr/include/
    # TODO
    cp -av ${rootfs}/usr/lib/libuuid.so.1.3.0 usr/lib/libuuid1.so  ||: #TODO might not be needed
    cp -av ${rootfs}/usr/lib/libconnectivity_abstraction.so  usr/lib/ ||: #TODO might not be needed
    rm -rf lib
    ln -fs usr/lib lib
}


main_()
{
    package="iotivity-example"

    thisdir=$(dirname -- "$0")
    thisdir=$(realpath -- "$thisdir/..")

    [ "" != "${profile}" ] || profile="tizen_2_4_mobile"

    [ "" != "${tmpdir}" ] \
        || tmpdir="${PWD}/tmp"

    [ "" != "${home}" ] \
        || home="${tmpdir}/out/${package}/${version}/${profile}"

    [ "" != "${version}" ] \
        || version=""

    [ "" != "${arch}" ] || arch="armv7l"
    git="git"

    thisdir=$(dirname -- "$0")
    thisdir=$(realpath -- "$thisdir/..")
    HOME="${home}"

    export HOME

    gbsdir="${HOME}/tmp/gbs/tmp-GBS-${profile}_${arch}/"
    rootfs="${gbsdir}/local/BUILD-ROOTS/scratch.${arch}.0/"
    rpmdir="${gbsdir}/local/repos/${profile}_${arch}/${arch}/RPMS/"

    mkdir -p "${HOME}"
    mkdir -p "${tmpdir}"
    cd "${tmpdir}"

    case "$profile" in
        "tizen_2_3_1_wearable")
            deps="scons libx11 iotivity"
            ;;
        "tizen_2_4_mobile")
            deps="iotivity"
            ;;
        *)
            deps="scons iotivity iotivity-example"
            ;;
    esac

    usage_

    # checking tools
    setup_

    ls ${rpmdir}/iotivity-devel-${version}[0-9]*-*${arch}.rpm \
        || build_ "${profile}" ;

    cd "${thisdir}"
    deploy_
}


main_ "$@"
