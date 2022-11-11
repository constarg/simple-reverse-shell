


all: dir_make
	gcc -Wall ./src/main.c -o ./bin/revershell

dir_make:
	mkdir -p ./bin/
