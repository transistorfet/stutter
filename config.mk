
FRONTEND = simple
MODULES = base irc

CONFIG_H = ../config.h
ARFLAGS += rc
CFLAGS += -g -DDEBUG -DCONFIG_H='<$(CONFIG_H)>' -I$(ROOT)/include

CC	= gcc
CXX	= g++
LD	= gcc
AS	= as
AR	= ar
RANLIB	= ranlib
DEPEND	= $(CC) -MM

