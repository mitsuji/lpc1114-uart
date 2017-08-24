defines =
#defines = -D__STACKA_DEBUG_
#defines = -D__CELL_DEBUG_
#defines = -D__CELL_DEBUG_ -D__STACKA_DEBUG_


main:
	gcc $(defines) -m32 -c -std=c99 -o io.o io.c
	gcc $(defines) -m32 -c -std=c99 -o lexia.o lexia.c
	gcc $(defines) -m32 -c -std=c99 -o stacka.o stacka.c
	gcc $(defines) -m32 -c -std=c99 -o cell.o cell.c
	gcc $(defines) -m32 -c -std=c99 -o main.o main.c
	gcc $(defines) -m32 -o lisp io.o lexia.o stacka.o cell.o main.o


clean:
	rm lisp
	rm main.o
	rm cell.o
	rm stacka.o
	rm lexia.o
	rm io.o
