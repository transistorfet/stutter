

ROOT = ..

TARGET = $(ROOT)/bin/stutter
OBJS =	main.o \
	key.o \
	type.o \
	utils.o \
	variable.o \
	namespace.o \
	frontend/$(FRONTEND)/$(FRONTEND).a \
	modules/base/modbase.a \
	modules/irc/modirc.a

LDFLAGS = 
LIBS = -lnit -lncurses
CFLAGS = -g -I$(ROOT)/include

include $(ROOT)/builder/builder.mk



