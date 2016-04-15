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

profile="tizen_2_4_mobile"
version="1.0.0"
arch="armv7l"
gbsdir="${HOME}/tmp/gbs/tmp-GBS-${profile}-${arch}/"
rootfs="${gbsdir}/local/BUILD-ROOTS/scratch.${arch}.0/"
rpmdir="${gbsdir}/local/repos/${profile}_${arch}/${arch}/RPMS/"


die_()
{
    local r=$?
    echo "error: $@"
    echo "exit: $r"
    return $r
}


setup_()
{
    which apt-get || die_ "TODO: port to non debian systen"
    sudo apt-get install make git wget curl
}


build_()
{
    package="tizen-helper"
    url="https://notabug.org/tizen/${package}"
    branch="tizen"
    git clone -b ${branch} "$url"

    echo "# check toolchain"
    make -C ${package} deploy
    make -C ${package} profile.${profile}-${arch}

    make="make -f ${PWD}/tizen-helper/Makefile profile.${profile}-${arch}"
    
    # Scons is not available on Tizen:2.3.1:Wearable yet
    package="scons"
    branch="sandbox/pcoval/tizen_2.3"
    url="https://git.tizen.org/cgit/platform/upstream/${package}.git"
    git clone -b ${branch} ${url}
    $make -C ${package}
    
    package="iotivity"
    branch="sandbox/pcoval/tizen_2.4"
    url="https://git.tizen.org/cgit/contrib/${package}.git"
    git clone -b ${branch} ${url}
    $make -C ${package}
    
    package="mraa"
    branch="sandbox/pcoval/tizen"
    url="https://git.tizen.org/cgit/contrib/${package}.git"
    git clone -b ${branch} ${url}
    $make -C ${package}

    package="iotivity-example"
    url="https://github.com/TizenTeam/${package}"
    branch="tizen"
    git clone -b ${branch} "$url"
    $make -C ${package}
}


cat<<EOF
Check:

https://wiki.iotivity.org/tizen
EOF

projectdir=$(pwd)
which git || setup_

mkdir -p ${projectdir}/tmp
cd ${projectdir}/tmp && build_
