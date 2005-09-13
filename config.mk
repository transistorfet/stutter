

FRONTEND = simple
CONFIG_H = ../config.h
LIBNIT_A = $(ROOT)/../libnit

FEDIR = ../src/frontend/$(FRONTEND)
CFLAGS += -DDEBUG -DCONFIG_H='<$(CONFIG_H)>' -DFRONTEND_H='<$(FEDIR)/$(FRONTEND).h>'

LIBPATH += -L$(LIBNIT_A)/lib
INCPATH += -I$(LIBNIT_A)/include

