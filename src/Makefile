
CC=gcc

all: ruleprob rp-send

ruleprob : LIBS=-lrt

ruleprob : CFLAGS=-O2 -Wall

ruleprob : ruleprob.o ruleprob.h cmd_magic.o cmd_magic.h
	$(CC) -o $@ $^ $(LIBS)

rp-send : LIBS=-lrt

rp-send : rp-send.o show_address.o show_address.h cmd_magic.o cmd_magic.h
	$(CC) -o $@ $^ $(LIBS)