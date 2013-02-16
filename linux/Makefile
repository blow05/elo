###########################################
# Simple Makefile for HIDAPI test program
#
# Alan Ott
# Signal 11 Software
# 2010-06-01
###########################################

all: hidtest

CC       ?= gcc
CFLAGS   ?= -Wall -g -O3

CXX      ?= g++
#CXXFLAGS ?= -Wall -g
CXXFLAGS = $(CFLAGS)

COBJS     = hid-libusb.o
CPPOBJS   = ../elo_input/elo_input.o
OBJS      = $(COBJS) $(CPPOBJS)
LIBS      = `pkg-config libusb-1.0 libudev --libs`
INCLUDES ?= -I../hidapi `pkg-config libusb-1.0 --cflags`


hidtest: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o elo_input

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(CPPOBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJS) elo_input

.PHONY: clean
