
DIRS = libnit/src src
CVSROOT = :pserver:anonymous@jabberwocky.hn.org:/home/cvs/cvsroot

.PHONY: all update $(DIRS)

all: $(DIRS)

$(DIRS):
	$(MAKE) -C $@

update:
	cvs -d$(CVSROOT) update -d

