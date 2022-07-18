CC = g++
prog: ex004.o ex004_array.o
	$(CC) -o prog ex004.o ex004_array.o
ex004.o: ex004.c ex004_array.h
	$(CC) -c ex004.c ex004_array.h
ex004_array.o: ex004_array.c ex004_array.h
	$(CC) -c ex004_array.c ex004_array.h