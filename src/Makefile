

ROOT = ..

#DIRS = $(FEDIR)
# $(LIBNIT_A)

TARGET = $(ROOT)/bin/stutter
OBJS =	main.o \
	command.o \
	variable.o \
	namespace.o \
	$(FEDIR)/$(FRONTEND).a \
	modules/base/modbase.a \
	modules/irc/modirc.a

LDFLAGS = 
LIBS = -lnit -lncurses
CFLAGS = -g -I$(ROOT)/include

include $(ROOT)/builder/builder.mk



