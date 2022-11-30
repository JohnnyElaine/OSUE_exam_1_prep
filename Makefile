CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809 -D_XOPEN_SOURCE
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)
LIBS = -lrt -pthread -lpthread -lm

OBJECTS_SERVER = server.o shared_mem.o utils.o
OBJECTS_CLIENT = client.o shared_mem.o utils.o

.PHONY: all clean clean_after

all: client server

client: $(OBJECTS_CLIENT)
	$(CC) -o $@ $^ $(LIBS)

server: $(OBJECTS_SERVER)
	$(CC) -o $@ $^ $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean_after:
	rm -rf *.o

clean:
	rm -rf *.o client server

# dependencies
client.o: client.c
server.o: server.c
shared_mem.o: shared_mem.c shared_mem.h
utils.o: utils.c utils.h
