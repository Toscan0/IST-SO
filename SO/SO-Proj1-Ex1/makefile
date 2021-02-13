#	SO - 1 Entrega 

#   Grupo: 25
#	Tiago Henriques- 81633
#	Pedro Cunha- 82343
#	Tomas Zaky- 79690 

# Compila o ficheiro fibonacci.c ; nome do executavel: fibonacci
# Compila o ficheiro par-shell.c ; nome do executavel: main

all: fibonacci par-shell

fibonacci: fibonacci.o
	gcc -o fibonacci fibonacci.o

fibonacci.o: fibonacci.c
	gcc -Wall -g -c fibonacci.c

par-shell: commandlinereader.o par-shell.o
	gcc -o par-shell par-shell.o commandlinereader.o

par-shell.o: commandlinereader.c par-shell.c
	gcc -Wall -g -c  par-shell.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

clean:
	rm -f *.o par-shell fibonacci
