

ROOT = ../../..

TARGET = $(ROOT)/bin/stutter
OBJS =	simple.o \
	input.o \
	window.o \
	statusbar.o \
	screen.o \
	net.o \
	$(ROOT)/src/main.a \
	$(ROOT)/src/modules/base/modbase.a \
	$(ROOT)/src/modules/irc/modirc.a

LDFLAGS = 
LIBS = -lncurses
CFLAGS = -g -I$(ROOT)/include -DNET_DIRECT

include $(ROOT)/rules.mk



