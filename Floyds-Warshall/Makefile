CC=gcc
CFLAGS=-g -Wall -Wextra -Wpedantic -Wstrict-prototypes -std=gnu99
LFLAGS=-lm
ALL_LFLAGS= $(LFLAGS) -lpthread
CPROGS=make-graph make-graph-hard-coded print-graph floyds pth-floyds

all: $(CPROGS)
make-graph: utilities.o make-graph.o
	$(CC) $(LFLAGS) -o make-graph utilities.o make-graph.o
make-graph-hard-coded: utilities.o make-graph-hard-coded.o
	$(CC) $(LFLAGS) -o make-graph-hard-coded utilities.o make-graph-hard-coded.o
print-graph: utilities.o print-graph.o
	$(CC) $(LFLAGS) -o print-graph utilities.o print-graph.o
floyds: utilities.o floyds.o
	$(CC) $(LFLAGS) -o floyds utilities.o floyds.o
pth-floyds: utilities.o pth-floyds.o
	$(CC) -o pth-floyds utilities.o pth-floyds.o $(ALL_LFLAGS)
utilities.o: utilities.c
	$(CC) $(CFLAGS) -c utilities.c
make-graph.o: make-graph.c
	$(CC) $(CFLAGS) -c make-graph.c
make-graph-hard-coded.o: make-graph-hard-coded.c
	$(CC) $(CFLAGS) -c make-graph-hard-coded.c
print-graph.o: print-graph.c
	$(CC) $(CFLAGS) -c print-graph.c
floyds.o: floyds.c
	$(CC) $(CFLAGS) -c floyds.c
pth-floyds.o: pth-floyds.c
	$(CC) $(CFLAGS) -c pth-floyds.c
clean:
	rm -f *.o $(CPROGS) 
delete-data:
	rm -f *.dat