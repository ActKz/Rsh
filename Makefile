CC=g++
CFLAGS=-std=c++11
OBJS=rshd
all: rshd
rshd: conn_handler.cpp rshd.cpp rshd.h
	$(CC) $(CFLAGS) conn_handler.cpp rshd.cpp -o rshd
clean:
	rm -f $(OBJS)
