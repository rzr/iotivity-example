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
ENV package ${project}-${version}

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
ENV branch ${branch:-sandbox/${user}/on/previous/devel}
ENV EMAIL ${EMAIL:-nobody@localhost}
ENV NAME ${NAME:-Nobody}

RUN echo "#log: ${project}: Preparing system (DISTTAG=${DISTTAG})" \
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

RUN useradd -ms /bin/bash ${user}

USER ${user}
WORKDIR /home/${user}/
ENV project_dir /home/${user}/${package}

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

RUN echo "#log: ${project}: Building RPMs (${project}-${version}.${release})" \
 && cd "/root/rpmbuild/SOURCES/" \
 && ls \
 && time rpmbuild -ba "${project}.spec" \
  --define "version ${version}" \
  --define "release ${release}" \
 && find /root/rpmbuild/ -iname "*.*rpm" \
 && sync

RUN echo "#log: ${project}: Installing RPMs" \
 && rpm -i /root/rpmbuild/RPMS/*/*.rpm \
 && rpm -ql ${project} \
 && rpm -ql ${project}-service \
 && rpm -ql ${project}-devel \
 && rpm -ql ${project}-test \
 && rpm -ql ${project}-debuginfo \
 && sync


ENV project ${project}-example
ENV example_URL http://github.com/${team}/${project}
ARG example_branch
ENV example_branch ${example_branch:-sandbox/pcoval/previous}
ENV example_project_dir /home/${user}/${project}
ENV example_project ${project}
#TODO:
ENV example_project ${project}-switch
ENV example_package ${example_project}-${version}

RUN echo "#log: ${project}: Building: ${example_URl}#${example_branch}" \
 && git clone --depth 1 -b "${example_branch}" "${example_URL}" "${project}" \
 && echo "TODO: possible conflicts on variables name" \
 && unset package \
 && make -C "${project}" dist name="${example_project}" version="${version}" \
 && ls ${example_project}-${version}.tar.gz \
 && sync

RUN echo "#log: ${project}: Setup rpm-build (${example_package})" \
 && mkdir -p "/root/rpmbuild/SOURCES/" \
 && ln -vfs "${example_project_dir}/packaging/"* "/root/rpmbuild/SOURCES/" \
 && ln -vfs "${example_project_dir}/../${example_project}-${version}.tar.gz"  "/root/rpmbuild/SOURCES/" \
 && sync 

RUN echo "#log: ${example_project}: Building RPMs" \
 && cd "/root/rpmbuild/SOURCES/" \
 && ls -l \
 && time rpmbuild -ba "${example_project}.spec" \
  --define "version ${version}" \
  --define "release ${release}" \
 && find /root/rpmbuild/ -iname "*.*rpm" \
 && sync

RUN echo "#log: ${example_project}: Installing RPMs" \
 && rpm --force -i /root/rpmbuild/RPMS/*/*.rpm \
 && rpm -ql ${example_project} \
 && sync

RUN echo "#log: ${example_project}: Checking" \
  && { /opt/${example_project}/bin/server -v & server=$!; } \
  && sleep 5 \
  && { /opt/${example_project}/bin/client -v & client=$!; } \
  && sleep 5 \
  && kill -9 ${client} ${server} \
  && sync

