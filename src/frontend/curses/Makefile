

ROOT = ../../..

TARGET = $(ROOT)/bin/stutter
OBJS =	main.o \
	core/fe.o \
	core/net.o \
	core/desc.o \
	core/timer.o \
	core/execute.o \
	core/terminal.o \
	../common/cmds/insert.o \
	../common/core/colourmap.o \
	../common/core/widget.o \
	../common/types/attrib.o \
	../common/types/colour.o \
	../common/types/command.o \
	../common/types/style.o \
	../common/utils/format.o \
	../common/widgets/frame.o \
	../common/widgets/input.o \
	../common/widgets/scroll.o \
	../common/widgets/statusbar.o \
	../common/widgets/window.o \
	$(MODULES) \
	$(ROOT)/src/main.a

LIBS = -lncurses

include $(ROOT)/rules.mk



