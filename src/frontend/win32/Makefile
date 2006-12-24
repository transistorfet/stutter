

ROOT = ../../..

TARGET = $(ROOT)/bin/winstutter.exe

OBJS =	main.o \
	fe.o \
	net.o \
	timer.o \
	execute.o \
	terminal.o \
	../common/cmds/insert.o \
	../common/core/widget.o \
	../common/types/colour.o \
	../common/types/command.o \
	../common/utils/format.o \
	../common/widgets/frame.o \
	../common/widgets/input.o \
	../common/widgets/scroll.o \
	../common/widgets/window.o \
	../common/widgets/statusbar.o \
	$(MODULES) \
	$(ROOT)/src/main.a

LIBS = -mwindows -lwsock32 -lgdi32

include $(ROOT)/rules.mk


