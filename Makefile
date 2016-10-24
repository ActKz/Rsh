CC=g++
CFLAGS=-std=c++11
BIN_EXEC=$(TARG_NOOP) $(TARG_NUM) $(TARG_RMT) $(TARG_RMT0) $(TARG_RSHD)
TARG_NOOP=./ras/bin/noop
TARG_NUM=./ras/bin/number
TARG_RMT=./ras/bin/removetag
TARG_RMT0=./ras/bin/removetag0
TARG_RSHD=rshd
all: rshd noop number removetag removetag0
rshd: conn_handler.cpp rshd.cpp rshd.h parser.cpp parser.h
	$(CC) $(CFLAGS) conn_handler.cpp rshd.cpp parser.cpp -o $(TARG_RSHD)
noop: ./commands/noop.cpp
	$(CC) $(CFLAGS) ./commands/noop.cpp -o $(TARG_NOOP)
number: ./commands/number.cpp
	$(CC) $(CFLAGS) ./commands/number.cpp -o $(TARG_NUM)
removetag: ./commands/removetag.cpp
	$(CC) $(CFLAGS) ./commands/removetag.cpp -o $(TARG_RMT)
removetag0: ./commands/removetag0.cpp
	$(CC) $(CFLAGS) ./commands/removetag0.cpp -o $(TARG_RMT0)
clean:
	rm -f $(BIN_EXEC)
