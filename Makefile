all: main.o filehandle.o piece.o process.o
	mpiCC main.o filehandle.o piece.o process.o -o main

main.o: main.cpp
	mpiCC -c main.cpp

filehandle.o: filehandle.cpp filehandle.hpp
	mpiCC -c filehandle.cpp

piece.o: piece.cpp piece.hpp
	mpiCC -c piece.cpp

process.o: process.cpp process.hpp
	mpiCC -c process.cpp

clean:
	rm main *.o