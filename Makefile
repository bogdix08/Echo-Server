CC=gcc

LIBSOCKET=-lnsl -lpthread

CCFLAGS=-Wall -g

SRV=server

CLT=client

QUEUE=queue

all: $(QUEUE).o $(SRV) $(CLT)

$(QUEUE).o:$(QUEUE).c $(QUEUE).h
	$(CC) -c $(QUEUE).c

$(SRV).o:$(SRV).c 
	$(CC) -c $(SRV).c $(LIBSOCKET)

$(SRV):$(SRV).o $(QUEUE).o
	$(CC) -o $(SRV) $(SRV).o $(QUEUE).o 


$(CLT):	$(CLT).c

	$(CC) -o $(CLT) $(LIBSOCKET) $(CLT).c



clean:

	rm -f *.o *~

	rm -f $(SRV) $(CLT)
