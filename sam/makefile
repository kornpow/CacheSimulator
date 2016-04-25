CC=gcc
CFLAGS=-I. -lpthread
DEPS = fifo.h
OBJ = fifo.o trace.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

trace: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)