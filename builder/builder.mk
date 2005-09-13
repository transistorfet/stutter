#
# Name:		builder.mk
# Version:	0.1
# Description:	Builder make include file
#

.SUFFIXES: .o .c .h .cpp .s .asm
.PHONY: $(DIRS)

CC	= gcc
LD	= gcc
AS	= as
AR	= ar
RANLIB	= ranlib
MAKE	= make
DEPEND	= $(CC) -MM

-include $(ROOT)/config.mk

ifndef CFLAGS
	CFLAGS = -g
endif
ifndef DIST
	DIST = $(TARGET)
endif

all: $(TARGET) $(LIB_TARGET)

$(TARGET): $(DIRS) $(OBJS)
	$(LD) $(LIBPATH) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(LIB_TARGET): $(DIRS) $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $(LIB_TARGET)

.c.o:
	$(CC) $(INCPATH) $(CFLAGS) -c -o $@ $<

%.a: FORCE
	$(MAKE) $(MAKEFLAGS) -C $(dir $*)

$(DIRS): FORCE
	$(MAKE) $(MAKEFLAGS) -C $@
FORCE:

depend:
	$(DEPEND) $(SRCS) > .depend

dist:
	tar -cvf $(DIST).tar $(SRCS)

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) .depend

count:
	cat `ls *.[ch]` | wc


