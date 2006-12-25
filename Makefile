
include config.mk

DIRS = src/frontend/$(FRONTEND)
CVSROOT = :pserver:anonymous@jabberwocky.hn.org:/home/cvs/cvsroot

.PHONY: all update $(DIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@

update:
	cvs -d$(CVSROOT) update -d

clean:
	make -C src clean && make -C src/frontend/$(FRONTEND) clean && make -C src/modules/irc clean && make -C src/modules/base clean

config:
	perl tools/config-gen.pl include/stutter/config-def.h
	perl tools/config-gen.pl include/stutter/frontend/config-def.h
	perl tools/config-gen.pl include/stutter/modules/base/config-def.h
	perl tools/config-gen.pl include/stutter/modules/irc/config-def.h

