

ROOT = ../../..

LIB_TARGET = modirc.a
OBJS =	init.o \
	cmds/ctcp.o \
	cmds/disconnect.o \
	cmds/join.o \
	cmds/leave.o \
	cmds/me.o \
	cmds/msg.o \
	cmds/names.o \
	cmds/nick.o \
	cmds/notice.o \
	cmds/ping.o \
	cmds/reconnect.o \
	cmds/say.o \
	cmds/server.o \
	cmds/whois.o \
	core/msg.o \
	core/user.o \
	core/channel.o \
	core/server.o \
	msgs/default.o \
	msgs/join.o \
	msgs/kick.o \
	msgs/names.o \
	msgs/nick.o \
	msgs/notice.o \
	msgs/part.o \
	msgs/privmsg.o \
	msgs/quit.o \
	msgs/topic.o \
	msgs/ctcp/action.o \
	msgs/ctcp/ping.o \
	msgs/ctcp/version.o \
	utils/current.o \
	utils/dispatch.o \
	utils/format.o \
	utils/quit.o

include $(ROOT)/rules.mk

init.o: $(CONFIG_H)
$(CONFIG_H):

