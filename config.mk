

FRONTEND = simple
MODULES = base irc

CONFIG_H = ../config.h
CFLAGS += -DDEBUG -DCONFIG_H='<$(CONFIG_H)>'

LIBNIT_A = $(ROOT)/libnit

LIBPATH += -L$(LIBNIT_A)/lib
INCPATH += -I$(LIBNIT_A)/include

