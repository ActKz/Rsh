CC=gcc
CFLAGS=
OBJS=rshd
all: rshd
rshd: conn_handler.c rshd.c rshd.h
	$(CC) conn_handler.c rshd.c -o rshd
clean:
	rm -f $(OBJS)
