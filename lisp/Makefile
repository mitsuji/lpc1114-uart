main:
	gcc -std=c99 -m32 -c -o io.o io.c
	gcc -std=c99 -m32 -c -o lexia.o lexia.c
	gcc -std=c99 -m32 -c -o stacka.o stacka.c
	gcc -std=c99 -m32 -c -o cell.o cell.c
	gcc -std=c99 -m32 -c -o main.o main.c
	gcc -m32 -o lisp io.o lexia.o stacka.o cell.o main.o


clean:
	rm lisp
	rm main.o
	rm cell.o
	rm stacka.o
	rm lexia.o
	rm io.o
