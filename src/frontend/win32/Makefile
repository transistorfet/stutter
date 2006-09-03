

ROOT = ../../..

TARGET = $(ROOT)/bin/winstutter.exe
#OBJS =	main.o \
	fe.o \
	net.o \
	terminal.o \
	input.o \
	window.o \
	statusbar.o \
	$(MODULES) \
	$(ROOT)/src/main.a

OBJS =	main.o \
	fe.o \
	net.o \
	terminal.o \
	../common/core/widget.o \
	../common/widgets/frame.o \
	../common/widgets/input.o \
	../common/widgets/scroll.o \
	../common/widgets/window.o \
	../common/widgets/statusbar.o \
	$(MODULES) \
	$(ROOT)/src/main.a

LIBS = -mwindows -lwsock32 -lgdi32

include $(ROOT)/rules.mk


