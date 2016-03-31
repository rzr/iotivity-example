#!/bin/sh
#! /usr/bin/make -f
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


tizen_2_3_1_wearable_()
{

    profile="profile.tizen_2_3_1_wearable-armv7l"

    package="tizen-helper"
    url="https://notabug.org/tizen/${package}"
    branch="tizen"
    git clone -b ${branch} "$url"

    echo "# check toolchain"
    make -C ${package} deploy
    make -C ${package} ${profile}

    make="make -f ${PWD}/tizen-helper/Makefile ${profile}"
    
    # Scons is not available on Tizen:2.3.1:Wearable yet
    package="scons"
    branch="sandbox/pcoval/tizen_2.3"
    url="https://git.tizen.org/cgit/platform/upstream/${package}.git"
    git clone -b ${branch} ${url}
    $make -C ${package}
    
    package="iotivity"
    branch="sandbox/pcoval/latest"
    url="https://git.tizen.org/cgit/contrib/${package}.git"
    git clone -b ${branch} ${url}
    $make -C ${package}
    
    package="mraa"
    branch="sandbox/pcoval/tizen"
    url="https://git.tizen.org/cgit/contrib/${package}.git"
    git clone -b ${branch} ${url}
    $make -C ${package}

    package="iotivity-example"
    url="https://notabug.org/tizen/${package}"
    branch="tizen"
    git clone -b ${branch} "$url"
    $make -C ${package}

    find ${HOME}/tmp/gbs/tmp-GBS-tizen_2_3_1_wearable-armv7l/local/repos/tizen_2_3_1_wearable_armv7l/armv7l/RPMS

}

which git || setup_

tizen_2_3_1_wearable_

