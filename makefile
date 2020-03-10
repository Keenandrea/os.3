CC = gcc
CFLAGS = -g
BIN_NATIVE = master
BIN_TARGET = bin_addr
OBJ_NATIVE = master.o
OBJ_TARGET = bin_addr.o

.SUFFIXES:
.SUFFIXES: .c .o .h

all: master bin_addr
$(BIN_NATIVE): $(OBJ_NATIVE)
	$(CC) -o $(BIN_NATIVE) $(OBJ_NATIVE) -lm -lpthread
$(BIN_TARGET): $(OBJ_TARGET)
	$(CC) -o $(BIN_TARGET) $(OBJ_TARGET) -lpthread
$(OBJ_NATIVE): master.c
	$(CC) $(CFLAGS) -c master.c shmem.h
$(OBJ_TARGET): bin_addr.c
	$(CC) $(CFLAGS) -c bin_addr.c shmem.h
clean:
	/bin/rm -f *.o $(BIN_NATIVE) $(BIN_TARGET)

