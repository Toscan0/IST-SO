#	SO - 1 Entrega 

#   Grupo: 25
#	Tiago Henriques- 81633
#	Pedro Cunha- 82343
#	Tomas Zaky- 79690 

# Compila o ficheiro fibonacci.c ; nome do executavel: fibonacci
# Compila o ficheiro par-shell.c ; nome do executavel: par-shell

CFLAGS = -g -Wall -pedantic

all: par-shell fibonacci div0

par-shell: par-shell.o list.o commandlinereader.o
	gcc $(CFLAGS) -o par-shell par-shell.o commandlinereader.o list.o -pthread

par-shell.o: par-shell.c
	gcc $(CFLAGS) -c  par-shell.c

list.o: list.c list.h
	gcc $(CFLAGS) -c list.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc $(CFLAGS) -c commandlinereader.c

fibonacci:
	gcc $(CFLAGS) -o fibonacci fibonacci.c

div0: div.c
	gcc $(CFLAGS) -o div0 div.c

clean:
	rm -f *.o par-shell fibonacci div0
