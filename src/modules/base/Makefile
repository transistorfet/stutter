

ROOT = ../../..

LIB_TARGET = modbase.a
OBJS =	init.o \
	cmds/alias.o \
	cmds/bind.o \
	cmds/clear.o \
	cmds/clearline.o \
	cmds/close.o \
	cmds/concat.o \
	cmds/context.o \
	cmds/echo.o \
	cmds/evaluate.o \
	cmds/exec.o \
	cmds/expand.o \
	cmds/get.o \
	cmds/parse.o \
	cmds/quit.o \
	cmds/redirect.o \
	cmds/remove.o \
	cmds/scroll.o \
	cmds/select.o \
	cmds/set.o \
	cmds/source.o \
	cmds/timer.o \
	cmds/unbind.o \
	sigs/print.o \
	types/command.o \
	types/format.o \
	types/status.o \
	types/string.o \
	types/string-ref.o \
	types/table.o \
	types/time.o \
	types/timer.o \
	types/wildcard.o

include $(ROOT)/rules.mk

init.o: $(CONFIG_H) $(ROOT)/include/stutter/modules/base/config-def.h
$(CONFIG_H):

