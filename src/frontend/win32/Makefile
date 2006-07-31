

ROOT = ../../..

TARGET = $(ROOT)/bin/winstutter.exe
OBJS =	main.o \
	fe.o \
	net.o \
	terminal.o \
	input.o \
	window.o \
	statusbar.o \
	$(MODULES) \
	$(ROOT)/src/main.a

LIBS = -mwindows -lwsock32 -lgdi32

include $(ROOT)/rules.mk


