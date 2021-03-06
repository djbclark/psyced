# how do i check if $ENV{PAGER} is already set?
# like this, says ketmar. probably gmake only.
PAGER ?= more

##############################################################################
## INSTALLATION

install: INSTALL.txt README.txt
#	@$(PAGER) echo Your environment variable PAGER is not configured!
	$(PAGER) INSTALL.txt
	@clear
	./install.sh || ksh install.sh

test:
	(cd bin && perl -c edata && perl -c psyconf && perl -c fmt2textdb)

clean:
	-rm INSTALL.txt world/LP_SWAP* world/OBJ_DUMP world/*.log world/log
	-rm world/WIZLIST world/.methods
#	find . -follow -name '.*.swp' -print
#	find . -follow -name '*.b' -print -exec rm {} \;

README.txt:
	lynx -dump http://www.psyced.org/README.html >$@

INSTALL.txt:
	lynx -dump http://www.psyced.org/INSTALL.html >$@

corpses:
	find . -name .#\* -print

# show me the fingerprint of the certificate i am about to install
fp: local/cert.pem
	openssl x509 -in local/cert.pem -fingerprint -sha1 | head -n 1

##############################################################################
## USER AND PLACE DATA MAINTAINANCE

MTIME = +120

clean.log:
	(cd data;nice find person -mtime $(MTIME) -print -exec rm {} \;) >$@
	wc clean.log

preview:
	(cd data;nice find person -mtime $(MTIME) -exec ls -l {} \;) | $(PAGER)
	(cd data;nice find place -mtime $(MTIME) -exec ls -l {} \;) | $(PAGER)

today:
	(cd data;nice find person -mtime -1 -print) | wc

##############################################################################
## THE REST OF THE MAKEFILE PROBABLY ISN'T VERY INTERESTING OR USEFUL FOR YOU

# generate a file that autoloads all lpc classes.. good only for debugging
local/all.ls:
	(cd world;find . -follow -name '*.c' -print) | grep -vw not >$@

.methods:
	(cd world;../bin/rmeth net/* pro/*) | sort >$@

.links:
	(cd world;../bin/findlinks */de/* */en/*) >$@

rights:
# how do i tell him to skip .git ?
	find . -type d -exec chmod 755 {} \;
	find . -type f -exec chmod 644 {} \;
	chmod +x bin/* run/* config/psyced.settings install.sh utility/multipatcher

