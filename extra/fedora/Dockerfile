#!/bin/echo docker build . -f
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

FROM fedora:24
MAINTAINER Philippe Coval (philippe.coval@osg.samsung.com)

ENV project iotivity
ARG version
ENV version ${version:-1.3.1}
ARG release
ENV release ${release:-0~0+fedora24}
ENV package ${project}-${version}

ENV archive_url http://mirrors.kernel.org/${project}/${version}/${project}-${version}.tar.gz
ENV URL http://github.com/tizenteam/${project}
ARG branch
ENV branch ${branch:-sandbox/pcoval/on/next/devel}
ENV EMAIL ${EMAIL:-nobody@localhost}
ENV NAME ${NAME:-Nobody}

RUN echo "#log: ${project}: Preparing system" \
 && dnf update -y \
 && dnf install -y \
  git \
  make \
  sudo \
  rpm-build \
\
  autoconf \
  automake \
  boost-devel \
  bzip2 \
  chrpath \
  expat-devel \
  findutils \
  gcc-c++\
  gettext-devel \
  git \
  glib2-devel \
  libcurl-devel \
  libtool \
  libuuid-devel \
  openssl-devel \
  scons \
  sqlite-devel \
  unzip \
  wget \
  xz \
 && dnf clean -y all \
 && sync

RUN useradd -ms /bin/bash abuild

USER abuild
WORKDIR /home/abuild/
ENV project_dir /home/abuild/${package}

RUN echo "#log: TODO: ${project}: ADD ${archive_url} # once patches merged" \
 && git config --global user.name "${NAME}" \
 && git config --global user.email "${EMAIL}" \
 && git clone -b "${branch}" "${URL}" ${package} --depth 1 \
 && make -C "${package}" dist name="${project}" version="${version}" \
 && sync

USER root

RUN echo "#log: ${project}: TODO: Upgrade unsupported tools (ie: scon-2.4.1-1.fc24)" \
 && scons --version \
 && dnf remove -y scons \
 && rpm -i ${project_dir}/tools/tizen/scons*.rpm \
 && scons --version \
 && sync

RUN echo "#log: ${project}: Setup rpm-build" \
 && mkdir -p "/root/rpmbuild/SOURCES/" \
 && ln -fs "${project_dir}/tools/tizen/"* "/root/rpmbuild/SOURCES/" \
 && ln -fs "${project_dir}/../${package}.tar.gz"  "/root/rpmbuild/SOURCES/" \
 && sync 

#TODO: Spec can be overriden while developing
#ADD tools/tizen/${project}.spec "/root/rpmbuild/SOURCES/" 

RUN echo "#log: ${project}: Building RPMs" \
 && cd "/root/rpmbuild/SOURCES/" \
 && ls \
 && time rpmbuild -ba "${project}.spec" \
  --define "version ${version}" \
  --define "release ${release}" \
 && find /root/rpmbuild/ -iname "*.*rpm" \
 && sync

RUN echo "#log: ${project}: Installing RPMs" \
 && rpm -i /root/rpmbuild/RPMS/*.rpm \
 && rpm -ql ${project} \
 && rpm -ql ${project}-service \
 && rpm -ql ${project}-devel \
 && rpm -ql ${project}-test \
 && rpm -ql ${project}-debuginfo \
 && sync

ENV example_URL http://github.com/tizenteam/${project}-example
ARG example_branch
ENV example_branch ${example_branch:-sandbox/pcoval/devel}
ENV example_project iotivity-example-switch

RUN echo "#log: ${example_project}: Building: ${example_URl}#${example_branch}" \
 && git clone -b "${branch}" "${URL}" ${example_project} --depth 1 \
 && make -C "${example_project}" dist name="${example_project}" version="${version}" \
 && sync

RUN echo "#log: ${example_project}: Setup rpm-build" \
 && mkdir -p "/root/rpmbuild/SOURCES/" \
 && ln -fs "packaging/${example_project}.spec" "/root/rpmbuild/SOURCES/" \
 && ln -fs "${project_dir}/../${example_project}.tar.gz"  "/root/rpmbuild/SOURCES/" \
 && sync 

RUN echo "#log: ${example_project}: Building RPMs" \
 && cd "/root/rpmbuild/SOURCES/" \
 && ls \
 && time rpmbuild -ba "${example_project}.spec" \
  --define "version ${version}" \
  --define "release ${release}" \
 && find /root/rpmbuild/ -iname "*.*rpm" \
 && sync

RUN echo "#log: ${example_project}: Installing RPMs" \
 && rpm -i /root/rpmbuild/RPMS/*.rpm \
 && rpm -ql ${example_project} \
 && sync
