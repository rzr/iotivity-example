url?=file://${CURDIR}/

update:
	git branch -a | grep -v '*' | grep -v remotes | while read branch; do \
  mkdir -p $${branch}.tmp ; rmdir $${branch}.tmp ;\
  git submodule add --force -b $${branch} ${url} $${branch} ;\
  done
	git submodule update
