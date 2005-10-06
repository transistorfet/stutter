

FRONTEND = simple
CONFIG_H = ../config.h
LIBNIT_A = $(ROOT)/../libnit

FEDIR = ../src/frontend/$(FRONTEND)
CFLAGS += -DDEBUG -DCONFIG_H='<$(CONFIG_H)>'

LIBPATH += -L$(LIBNIT_A)/lib
INCPATH += -I$(LIBNIT_A)/include

