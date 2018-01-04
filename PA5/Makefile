#-----------------------------------------------------------
# 
# SWE2007: Software Experiment II (Fall 2017)
#
# Skeleton code for PA #5
# December 6, 2017
#
# Hayun Lee
# Embedded Software Laboratory
# Sungkyunkwan University
#
#-----------------------------------------------------------


CC		= gcc
CFLAGS		= -g -Wall -Werror
LDFLAGS		= -lpthread
RM		= rm
TAR		= tar

CSRCS		= webserver.c http.c string_sw.c
SUBCSRCS	= Makefile
TARGET		= webserver
OBJECTS		= $(CSRCS:.c=.o)

ifeq (tar,$(firstword $(MAKECMDGOALS)))
  TAR_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TAR_ARGS):;@:)
endif

all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

tar:
	$(TAR) zcvf $(TAR_ARGS).tar.gz $(CSRCS) $(SUBCSRCS)

.PHONY: clean

clean:
	$(RM) -f $(OBJECTS) $(TARGET) *~

