#!/usr/bin/gmake

#########################################################################
#                                                                       #
# File: Makefile                                                        #
#                                                                       #
# COPYRIGHT (C) Lehrstuhl fuer Informatik VI                            #
#               RWTH Aachen                                             #
#                                                                       #
# Authors: W. Macherey                                                  #
#                                                                       #
# Project: Lehrstuhl Praktikum                                          #
#                                                                       #
# Last Update: Sam Feb  8 00:30:49 CET 2003                             #
#                                                                       #
#########################################################################

#########################################################################
# determine machine type: intel, alpha, sparc64, mips                   #
#########################################################################
MACHINE_TYPE	:=	$(shell uname -m | sed -e s/i.86/intel/    \
					 | sed -e s/alpha/alpha/   \
					 | sed -e s/sun4u/sparc64/ \
					 | sed -e s/IP22/mips/)

#########################################################################
# set dependency file:                                                  #
#########################################################################
INSTDIR		=	$(shell pwd)
JOBSNUM		=	2
LOGFILE		=

#########################################################################
# define targets:                                                       #
#########################################################################
TARGETS		= sietill


#########################################################################
# compiler options                                                      #
#########################################################################
CC		=	gcc
LN		=	$(CC) #-static
CFLAGS		=	-Wall -O2 ##-O3 -ansi -pedantic
#DFLAGS		=	-g #-fno-inline
LIBS		=	-lm
SOURCES		=	$(wildcard *.c)
HEADERS		=	$(wildcard *.h)

#########################################################################
# all:                                                                  #
#########################################################################
all:
	@echo $(TARGETS) | xargs make -j $(JOBSNUM)

#########################################################################
# asr_engine:                                                           #
#########################################################################
SIETILL_O	= SieTill.$(MACHINE_TYPE).o \
		  SignalAnalysis.$(MACHINE_TYPE).o

sietill:
		make -j $(JOBSNUM) "GOAL=__SIETILL__ $(MACRO)" sietill_

sietill_:	$(SIETILL_O)
		$(LN) -o sietill.$(MACHINE_TYPE) \
		$(SIETILL_O) $(MACRO) $(DFLAGS) $(LIBS)

#########################################################################
# clean / clean_all:                                                    #
#########################################################################
clean:
	/bin/rm -f core core.* *~ *.$(MACHINE_TYPE).il *.$(MACHINE_TYPE).o

clean_all:
	make -j $(JOBSNUM) clean
	@echo $(TARGETS)" " | sed s/' '/'\.'$(MACHINE_TYPE)' '/g \
			    | xargs /bin/rm -f

#########################################################################
# create postscript file from header/source file                        #
#########################################################################
%.ps:	%.h %.c
	a2ps --pretty-print=c++ --highlight-level=normal --portrait \
	--line-numbers=5 --columns=1 --rows=1 --chars-per-line=130 \
	--file-align=fill --output=$@ --footer="`pwd`" $^

#########################################################################
# generate dependency file if necessary                                 #
#########################################################################
%.$(MACHINE_TYPE).o: %.c
	$(CC) -c $(CFLAGS) $(DFLAGS) $(MACRO) $< -o $@
