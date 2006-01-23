

ROOT = ..

TARGET = $(ROOT)/bin/stutter
OBJS =	main.o \
	key.o \
	type.o \
	utils.o \
	variable.o \
	namespace.o \
	lib/list.o \
	lib/memory.o \
	lib/string.o \
	modules/base/modbase.a \
	modules/irc/modirc.a \
	frontend/$(FRONTEND)/$(FRONTEND).a

LDFLAGS = 
LIBS = -lncurses
CFLAGS = -g -I$(ROOT)/include

include $(ROOT)/rules.mk



