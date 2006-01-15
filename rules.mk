#
# Name:		rules.mk
# Version:	0.1
# Description:	Make Rules
#

.SUFFIXES: .o .c .h .cpp .s .asm
.PHONY: $(DIRS)

CC	= gcc
LD	= gcc
AS	= as
AR	= ar
RANLIB	= ranlib
DEPEND	= $(CC) -MM

include $(ROOT)/config.mk


all: $(TARGET) $(LIB_TARGET)

$(TARGET): $(DIRS) $(OBJS)
	$(LD) $(LIBPATH) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(LIB_TARGET): $(DIRS) $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $(LIB_TARGET)

.c.o:
	$(CC) $(INCPATH) $(CFLAGS) -c -o $@ $<

%.a:
	$(MAKE) -C $(dir $*)

$(DIRS):
	$(MAKE) -C $@

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

