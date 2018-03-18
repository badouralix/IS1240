DEBUG=no
TYPE_NUM= 0

CC=mpicc
ifeq ($(DEBUG),yes)
	CFLAGS=-Wall -g -D TYPE_NUM=$(TYPE_NUM)
	LDFLAGS=
else
	CFLAGS=-Wall -D TYPE_NUM=$(TYPE_NUM)
	LDFLAGS=
endif

EXECS= calculator generator
TESTS= t_matrix t_vector
LIBS= $(wildcard lib/*.c)
LIBS_OBJ= $(LIBS:.c=.o)
LIBS_HDR= $(LIBS:.c=.h)


all: $(EXECS) $(TESTS)

execs: $(EXECS)

tests: $(TESTS)


calculator: src/calculator.o $(LIBS_OBJ)
	@[ -d bin  ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)
	@[ -d data ] || mkdir data

calculator.o: $(LIBS_HDR)

generator: src/generator.o $(LIBS_OBJ)
	@[ -d bin  ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)
	@[ -d data ] || mkdir data

generator.o: $(LIBS_HDR)

t_matrix: tests/t_matrix.o lib/file.o lib/matrix.o
	@[ -d bin ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

t_matrix.o: lib/matrix.h

t_vector: tests/t_vector.o lib/file.o lib/vector.o
	@[ -d bin ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

t_vector.o: lib/vector.h


%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)


.PHONY: clean clean-tests flush mrproper

clean:
	rm -rf lib/*.o
	rm -rf src/*.o
	rm -rf tests/*.o

clean-tests: clean
	rm -rf bin/t_*

flush:
	rm -rf data/*

mrproper: clean
	rm -rf bin/
	rm -rf data/

