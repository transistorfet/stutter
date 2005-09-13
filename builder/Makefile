#
# Name:		Makefile
# Version:	0.1
# Description:	Builder Utilities Makefile
#


update:
	cvs export -D `date --iso-8601=seconds` -d /tmp/builder builder
	cp -R /tmp/builder/* .
	rm -R /tmp/builder

