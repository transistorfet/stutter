

ROOT = ../../..

TARGET = $(ROOT)/bin/stutter
OBJS =	\
	main.o \
	core/execute.o \
	core/fe.o \
	core/interface.o \
	core/tcp.o \
	core/terminal.o \
	core/timer.o \
	$(ROOT)/src/frontend/common/fecommon.a \
	$(MODULES) \
	$(ROOT)/src/main.a

LIBS = -lncurses

include $(ROOT)/rules.mk

