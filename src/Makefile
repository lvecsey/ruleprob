
CC=gcc

all: ruleprob rp-send

ruleprob : LIBS=

ruleprob : CFLAGS=-O2 -Wall

ruleprob : ruleprob.o ruleprob.h
	$(CC) -o $@ $^ $(LIBS)

rp-send : LIBS=-lrt

rp-send : rp-send.o show_address.o show_address.h
	$(CC) -o $@ $^ $(LIBS)