CC=gcc

LIBSOCKET=-lnsl

LIBPTHREAD=-lpthread

CCFLAGS=-Wall -g

SRV=server

CLT=client

QUEUE=queue

all: $(QUEUE).o $(SRV) $(CLT)

$(QUEUE).o:$(QUEUE).c $(QUEUE).h
	$(CC) -c $(QUEUE).c

$(SRV).o:$(SRV).c common.h
	$(CC) -c $(SRV).c

$(SRV):$(SRV).o $(QUEUE).o
	$(CC) -o $(SRV) $(SRV).o $(QUEUE).o $(LIBSOCKET) $(LIBPTHREAD)


$(CLT):	$(CLT).c common.h
	$(CC) -o $(CLT) $(LIBSOCKET) $(CLT).c



clean:

	rm -f *.o *~

	rm -f $(SRV) $(CLT)
