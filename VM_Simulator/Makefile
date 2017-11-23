# Makefile for vmsim
# COMP1521 17s2 Assignment 2

CC=gcc
CFLAGS=-Wall -Werror -std=c99

vmsim : vmsim.o Memory.o PageTable.o Stats.o
vmsim.o : vmsim.c Memory.h PageTable.h
Memory.o : Memory.c Stats.h Memory.h
PageTable.o : PageTable.c Stats.h PageTable.h Memory.h
Stats.o : Stats.c Stats.h

mktrace : mktrace.c

clean:
	rm -f vmsim vmsim.o Memory.o PageTable.o Stats.o mktrace mktrace.o core
