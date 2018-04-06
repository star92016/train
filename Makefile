.PHONY:clean all install
all:server client
server:server.o
	cc -o server server.o
client:client.o
	cc -o client client.o -lncurses
.c.o:
	gcc -c -o $*.o $<
clean:
	rm -rf server client *.o
