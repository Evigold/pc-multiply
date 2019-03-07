CC=gcc
CFLAGS=-pthread -I. -Wall -Wno-int-conversion -D_GNU_SOURCE

#binaries=queueprodcons cpa pthread_mult
binaries=pcMatrix

all: $(binaries)

pcMatrix: counter.c prodcons.c matrix.c pcmatrix.c 
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) -f $(binaries) *.o


 
