CFLAGS = -Wall -pedantic -ansi  -g -c

all: i-banco

i-banco: commandlinereader.o contas.o i-banco.o 
	gcc -o i-banco commandlinereader.o contas.o i-banco.o

i-banco.o: i-banco.c
	gcc $(CFLAGS) i-banco.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc $(CFLAGS) -c commandlinereader.c

contas.o: contas.c contas.h
	gcc $(CFLAGS) contas.c

clean:
	rm -f *.o i-banco contas commandlinereader