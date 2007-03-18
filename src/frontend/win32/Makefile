

ROOT = ../../..

TARGET = $(ROOT)/bin/winstutter.exe

OBJS =	main.o \
	cmds/play.o \
	core/fe.o \
	core/net.o \
	core/menu.o \
	core/timer.o \
	core/execute.o \
	core/terminal.o \
	$(ROOT)/src/frontend/common/fecommon.a \
	$(MODULES) \
	$(ROOT)/src/main.a

LIBS = -mwindows -lwsock32 -lgdi32 -lwinmm

include $(ROOT)/rules.mk


