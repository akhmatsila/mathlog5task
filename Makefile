.PHONY: all, run, clean
SOURCES=$(shell find -type f -name "*.cpp")
HEADERS=$(shell find -type f -name "*.h")

all:main.o operation.o axiom.o parser.o proof.o axiom_formal.o A9.h axiom_arifmetic.o
	g++ main.o operation.o axiom.o parser.o proof.o axiom_formal.o A9.h axiom_arifmetic.o  --std=c++14 -o  program

main.o:main.cpp
	g++ -c main.cpp -o main.o

operation.o: operation.cpp operation.h
	g++ -c operation.cpp -o operation.o

axiom.o: axiom.cpp axiom.h
	g++ -c axiom.cpp -o axiom.o

parser.o: parser.cpp parser.h
	g++ -c parser.cpp -o parser.o

proof.o: proof.cpp proof.h
	g++ -c proof.cpp -o proof.o

axiom_formal.o: axiom_formal.cpp axiom_formal.h
	g++ -c axiom_formal.cpp -o axiom_formal.o

axiom_arifmetic.o: axiom_arifmetic.cpp axiom_arifmetic.h
	g++ -c axiom_arifmetic.cpp -o axiom_arifmetic.o

A9.o: A9.cpp A9.h
	g++ -c A9.cpp -o A9.o

run:
	./program

clean:
	rm program
	rm *.o


