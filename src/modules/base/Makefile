

ROOT = ../../..

LIB_TARGET = modbase.a
OBJS =	init.o \
	cmds/bind.o \
	cmds/next.o \
	cmds/previous.o \
	cmds/clear.o \
	cmds/scroll.o \
	cmds/get.o \
	cmds/set.o \
	cmds/parse.o \
	cmds/remove.o \
	types/time.o \
	types/status.o \
	types/command.o

ARFLAGS = rc
CFLAGS = -g -I$(ROOT)/include

include $(ROOT)/builder/builder.mk

