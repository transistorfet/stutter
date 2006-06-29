
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

