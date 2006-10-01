
FRONTEND = curses
MODULES = $(ROOT)/src/modules/base/modbase.a \
	  $(ROOT)/src/modules/irc/modirc.a

CONFIG_H = ../config.h
ARFLAGS = rc
CFLAGS += -g3 -DDEBUG -DCONFIG_H='<$(CONFIG_H)>' -I$(ROOT)/include

CC	= gcc
CXX	= g++
LD	= gcc
AS	= as
AR	= ar
RANLIB	= ranlib
DEPEND	= $(CC) -MM

