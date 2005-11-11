

ROOT = ../../..

LIB_TARGET = modirc.a
OBJS =	init.o \
	cmds/join.o \
	cmds/quit.o \
	cmds/leave.o \
	cmds/msg.o \
	cmds/me.o \
	cmds/nick.o \
	cmds/notice.o \
	cmds/say.o \
	core/msg.o \
	core/user.o \
	core/channel.o \
	core/server.o \
	msgs/nick.o \
	msgs/join.o \
	msgs/kick.o \
	msgs/names.o \
	msgs/notice.o \
	msgs/part.o \
	msgs/privmsg.o \
	msgs/quit.o \
	msgs/topic.o \
	msgs/default.o \
	utils/current.o \
	utils/status.o \
	utils/dispatch.o \
	utils/format.o

ARFLAGS = rc
CFLAGS = -g -I$(ROOT)/include

include $(ROOT)/builder/builder.mk

