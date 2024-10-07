CC 			= clang
CFLAGS 	= -pedantic -Wall -Os -std=c99
LIBS 		= -lpthread -lconfig
OBJECTS = server.o clist.o ping.o tloop.o shared.o

all: tcpio

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clist.o: clist.c
	$(CC) $(CFLAGS) -c clist.c

tloop.o: tloop.c
	$(CC) $(CFLAGS) -c tloop.c

shared.o: shared.c
	$(CC) $(CFLAGS) -c shared.c

ping.o: ping.c
	$(CC) $(CFLAGS) -c ping.c

tcpio: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o ../tcpio

all clean:
	rm -f *.o
