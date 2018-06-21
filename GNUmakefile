#! /usrbin/make -f

default: all

url?=https://github.com/tizenteam/iotivity-example
#url?=file://${CURDIR}/

all: src help

help: README.md
	cat README.md

distclean:
	rm -rf src .gitmodules .git/modules

src: rule/src

rule/src: README.md
	grep -o '^* branch=[^\:]*' $< | cut -d= -f2 | while read branch ; do \
 cd "${CURDIR}" ; \
 git submodule add -b "$${branch}" "${url}" "src/$${branch}"; \
 git submodule init "src/$${branch}"; \
 done ; \
 git submodule update

rule/branches:
	git branch -a | grep -v '*' | grep -v remotes | while read branch; do \
  mkdir -p src/$${branch}.tmp ; rmdir src/$${branch}.tmp ;\
  git submodule add --force -b $${branch} ${url} src/$${branch} ;\
  done
	git submodule update
