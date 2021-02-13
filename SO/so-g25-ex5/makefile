#	Projeto de SO

#   Grupo: 25
#	Tiago Henriques- 81633
#	Pedro Cunha- 82343
#	Tomas Zaky- 79690 

# Compila o ficheiro par-shell.c --> nome do executavel: par-shell
# Compila o ficheiro par-shell-terminal-c --> nome do executavel: par-shell-terminal
# Compila o ficheiro fibonacci.c --> nome do executavel: fibonacci
# Compila o ficheiro div.c --> nome do executavel: div0
# make clean -> elimina os executavies, os ficheiros de textos(log.txt e par-shel-out*) e o pipe(par-shell-in)

CFLAGS = -g -Wall -pedantic

all: par-shell par-shell-terminal fibonacci div0

par-shell: par-shell.o list.o terminal.o commandlinereader.o
	gcc $(CFLAGS) -o par-shell par-shell.o terminal.o commandlinereader.o list.o -pthread

par-shell.o: par-shell.c
	gcc $(CFLAGS) -c  par-shell.c

par-shell-terminal: par-shell-terminal.o commandlinereader.o
	gcc $(CFLAGS) -o par-shell-terminal par-shell-terminal.o commandlinereader.o

par-shell-terminal.o: par-shell-terminal.c
	gcc $(CFLAGS) -c par-shell-terminal.c

terminal.o: terminal.c terminal.h
	gcc $(CFLAGS) -c terminal.c

list.o: list.c list.h
	gcc $(CFLAGS) -c list.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc $(CFLAGS) -c commandlinereader.c

fibonacci:
	gcc $(CFLAGS) -o fibonacci fibonacci.c

div0: div.c
	gcc $(CFLAGS) -o div0 div.c

clean:
	rm -f *.o par-shell par-shell-terminal par-shell-in fibonacci div0 log.txt par-shell-out* par-shell-stats*
