
DIRS = libnit/src src

.PHONY: all $(DIRS)

all: $(DIRS)

$(DIRS):
	make -C $@

