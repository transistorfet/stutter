

ROOT = ../../..

TARGET = $(ROOT)/bin/stutter
OBJS =	main.o \
	core/fe.o \
	core/net.o \
	core/desc.o \
	core/timer.o \
	core/execute.o \
	core/terminal.o \
	$(ROOT)/src/frontend/common/fecommon.a \
	$(MODULES) \
	$(ROOT)/src/main.a

LIBS = -lncurses -L$(ROOT)/src/modules/sdrl/lib/sdrl/lib -lbase -lsdrl

include $(ROOT)/rules.mk

core/fe.o: ../../../include/stutter/frontend/common/api.h

