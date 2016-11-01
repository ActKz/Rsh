CC=g++
CFLAGS=-std=c++11 -g
BIN_EXEC=$(TARG_NOOP) $(TARG_NUM) $(TARG_RMT) $(TARG_RMT0) $(TARG_RSHD) $(OBJS)
TARG_NOOP=./ras/bin/noop
TARG_NUM=./ras/bin/number
TARG_RMT=./ras/bin/removetag
TARG_RMT0=./ras/bin/removetag0
TARG_RSHD=rshd
CPP=./TokenizerClass.cpp ./RshClass.cpp ./RshServer.cpp ./main.cpp
all:
	$(CC) $(CFLAGS) $(CPP) -o $(TARG_RSHD)

bin: noop number removetag removetag0
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
