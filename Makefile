# $RCSfile: Makefile,v $
# $Author: steve $
# $Date: 1999/05/04 01:13:33 $
# Rev Hist: 
#           $Log: Makefile,v $
#           Revision 1.7  1999/05/04 01:13:33  steve
#           dunno
#
#           Revision 1.6  1999/04/20 04:02:52  steve
#           added VGA libs and generally neatened things up
#
#           Revision 1.5  1999/04/14 05:42:59  steve
#           finished fixing header vars
#

CC          = gcc
CPP         = g++
CFLAGS      = 
CPPFLAGS    = -ggdb
LDFLAGS     = $(CFLAGS) -lm -lvgagl -lvga #-lefence

CFILES      = $(wildcard *.C)
OBJS        = $(patsubst %.C,%.o,$(CFILES))

APP         = fleaz

all: $(APP)

fleaz: $(OBJS) 
	$(CPP) $(OBJS) $(LDFLAGS) -o $(APP)


clean:
	rm -rf $(APP) *.o core

.c.o:
	$(CC) $(CFLAGS) -c $<
.C.o:
	$(CPP) $(CPPFLAGS) -c $<

dep: 
	rm .depend
	$(CPP) -MM $(CFILES) > .depend  

.depend:
	$(CPP) -MM $(CFILES) > .depend  

# DEPENDENCIES # 
include .depend
