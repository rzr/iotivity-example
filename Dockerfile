#!/bin/echo docker build . -f
#{
# Copyright 2017 Samsung Electronics France SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#}

FROM ubuntu:14.04
MAINTAINER Philippe Coval (philippe.coval@osg.samsung.com)

ENV project iotivity
ARG version
ENV version ${version:-1.3.1}
ENV package ${project}-${version}


ARG SCONSFLAGS
ENV SCONSFLAGS ${SCONSFLAGS:-"VERBOSE=1 ERROR_ON_WARN=False"}

ARG prefix
ENV prefix ${prefix:-/usr/}
ARG destdir
ENV destdir ${destdir:-/usr/local/opt/${project}}

ENV DEBIAN_FRONTEND noninteractive
ENV LC_ALL en_US.UTF-8
ENV LANG ${LC_ALL}

RUN echo "#log: Configuring locales" \
  && apt-get update \
  && apt-get install -y locales \
  && echo "${LC_ALL} UTF-8" | tee /etc/locale.gen \
  && locale-gen ${LC_ALL} \
  && dpkg-reconfigure locales

RUN echo "#log: ${project}: Preparing system" \
  && apt-get update -y \
  && apt-get install -y \
    devscripts \
    debhelper \
    base-files \
\
    autoconf \
    automake \
    autotools-dev \
    bash \
    git \
    libtool \
    make \
    python-dev \
    scons \
    sudo \
    unzip \
    valgrind \
    wget \
\
    libboost-date-time-dev \
    libboost-iostreams-dev \
    libboost-log-dev \
    libboost-program-options-dev \
    libboost-regex-dev \
    libboost-system-dev \
    libboost-thread-dev \
    libbz2-dev \
    libcurl4-openssl-dev \
    libglib2.0-dev \
    libicu-dev \
    libsqlite3-dev \
    uuid-dev \
  && apt-get clean \
  && sync


ARG user
ENV user ${user:-pcoval}

ARG release
ENV release ${release:-0~${user}0+${DISTTAG}}

ARG team
#TODO: use upstream when ready
ENV team ${team:-tizenteam}
ENV archive_url http://mirrors.kernel.org/${project}/${version}/${project}-${version}.tar.gz
ENV URL http://github.com/${team}/${project}
ARG branch
ENV branch ${branch:-sandbox/${user}/on/next/devel}
ENV EMAIL ${EMAIL:-nobody@localhost}
ENV NAME ${NAME:-Nobody}

RUN useradd -ms /bin/bash ${user}

USER ${user}
WORKDIR /home/${user}/
ENV project_dir /home/${user}/${package}

RUN echo "#log: TODO: ${project}: ADD ${archive_url} # once patches merged" \
  && git config --global user.name "${NAME}" \
  && git config --global user.email "${EMAIL}" \
  && git clone -b "${branch}" "${URL}" ${package} --depth 1 \
  && cd ${package} \
  && scons --prefix=/usr \
  && scons--install-sandbox=/usr/local/opt/iotivity --prefix=/usr install \
  && sync

ENV project ${project}-example
ENV example_project ${project}
ENV example_project ${project}-switch
ENV example_package ${example_project}-${version}
ENV example_project_dir /home/${user}/${example_project}


ARG workdir
ENV workdir ${workdir:-${HOME}/usr/local/src/${project}}
WORKDIR ${workdir}

COPY . ${WORKDIR}

RUN echo "#log: ${example_project}: Building:" \
  && pwd \
  && make -C "${example_project}" dist name="${example_project}" version="${version}" \
  && ls ${example_project}-${version}.tar.gz \
  && sync
