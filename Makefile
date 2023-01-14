DEBUG = 
EXECS = server client 

all: $(EXECS)

server : pps.c serverFunctions.h
	gcc $(DEBUG) -o server pps.c serverFunctions.c

client : pqc.c clientFunctions.h
	gcc $(DEBUG) -o client pqc.c clientFunctions.c -lpthread 

clean: 
	rm -f $(EXECS)

