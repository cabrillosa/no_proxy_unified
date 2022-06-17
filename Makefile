CC=gcc
CFLAGS=-g

noproxy: noproxy.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o noproxy