CC=gcc
CFLAGS=-I. -lpthread
DEPS = 
OBJ1 = trace.o
OBJ2 = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

trace: $(OBJ1)
	gcc -o $@ $^ $(CFLAGS)

main: $(OBJ2)
	gcc -o $@ $^ $(CFLAGS)