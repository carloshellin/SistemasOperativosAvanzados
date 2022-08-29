all: fatsoa

fatsoa: fatsoa.o parser.o
	gcc -g -Wall -o fatsoa fatsoa.o parser.o

parser.o: parser.c parser.h
	gcc -g -Wall -c -o parser.o parser.c

fatsoa.o: fatsoa.c fatsoa.h
	gcc -g -Wall -c -o fatsoa.o fatsoa.c

clean:
	rm -f fatsoa
	rm *.o

