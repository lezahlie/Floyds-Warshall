CC=gcc
CFLAGS=-g -Wall -Wextra -Wpedantic -Wstrict-prototypes -std=gnu99
LFLAGS=-lm 
ALL_LFLAGS= $(LFLAGS) -lpthread
CPROGS=make-2d print-2d stencil-2d pth-stencil-2d 

all: $(CPROGS)
make-2d: utilities.o make-2d.o
	$(CC) $(LFLAGS) -o make-2d utilities.o make-2d.o
print-2d: utilities.o print-2d.o
	$(CC) $(LFLAGS) -o print-2d utilities.o print-2d.o
stencil-2d: utilities.o stencil-2d.o
	$(CC) $(LFLAGS) -o stencil-2d utilities.o stencil-2d.o
pth-stencil-2d: utilities.o pth-stencil-2d.o
	$(CC)  -o pth-stencil-2d utilities.o pth-stencil-2d.o $(ALL_LFLAGS)
utilities.o: utilities.c
	$(CC) $(CFLAGS) -c utilities.c
make-2d.o: make-2d.c
	$(CC) $(CFLAGS) -c make-2d.c
print-2d.o: print-2d.c
	$(CC) $(CFLAGS) -c print-2d.c
stencil-2d.o: stencil-2d.c
	$(CC) $(CFLAGS) -c stencil-2d.c
pth-stencil-2d.o: pth-stencil-2d.c
	$(CC) $(CFLAGS) -c pth-stencil-2d.c
clean:
	rm -f *.o $(CPROGS) 
delete-data:
	rm -f *.dat *.raw