#-----------------------------------------------------------
# 
# SWE2007: Software Experiment II (Fall 2017)
#
# Skeleton code for PA #1
# September 13, 2017
#
# Jong-Won Park
# Embedded Software Laboratory
# Sungkyunkwan University
#
#-----------------------------------------------------------

CC		= gcc
CFLAGS		= -g -Wall

RM		= rm
TAR		= tar

CSRCS		= main.c convert.c string_sw.c
SUBCSRCS	= Makefile convert.h string_sw.h
TARGET		= convert
OBJECTS		= $(CSRCS:.c=.o)

ifeq (tar,$(firstword $(MAKECMDGOALS)))
  TAR_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TAR_ARGS):;@:)
endif

all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $< -g

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^

tar:
	$(TAR) zcvf $(TAR_ARGS).tar.gz $(CSRCS) $(SUBCSRCS)

.PHONY:	clean

clean:
	$(RM) -f $(OBJECTS) $(TARGET) *.out

