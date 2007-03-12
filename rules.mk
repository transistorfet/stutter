#
# Name:		rules.mk
# Version:	0.1
# Description:	Make Rules
#

.SUFFIXES: .o .c .h .cpp .s .asm .rc
.PHONY: $(DIRS)

include $(ROOT)/config.mk

all: $(TARGET) $(LIB_TARGET)

$(TARGET): $(DIRS) $(OBJS)
	$(LD) $(LIBPATH) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

$(LIB_TARGET): $(DIRS) $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $(LIB_TARGET)

.c.o:
	$(CC) $(INCPATH) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CXX) $(INCPATH) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

.rc.o:
	$(RC) $< $@

%.a: FORCE
	$(MAKE) -C $(dir $*)

$(DIRS): FORCE
	$(MAKE) -C $@

FORCE:

depend:
	$(DEPEND) $(SRCS) > .depend

dist:
	tar -cvf $(DIST).tar $(SRCS)

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) .depend


