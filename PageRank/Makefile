DEBUG = no
WITH_LIBPNG = yes

CC = gcc

CFLAGS = -Wall $(shell mpicc -showme:compile)
LDFLAGS = -lm $(shell mpicc -showme:link)
ifeq ($(DEBUG),yes)
	CFLAGS += -g
endif
ifeq ($(WITH_LIBPNG),yes)
	CFLAGS += -D WITH_LIBPNG -fopenmp
	LDFLAGS += -lpng -fopenmp
endif

EXECS = main
TESTS = t_matrix t_pagerank t_vector

LIBS = $(wildcard lib/*.c)
LIBS_OBJ = $(LIBS:.c=.o)
LIBS_HDR = $(LIBS:.c=.h)


all: $(EXECS) $(TESTS)

execs: $(EXECS)

tests: $(TESTS)

main: src/main.o src/pagerank.o $(LIBS_OBJ)
	@[ -d bin ] || mkdir bin
	$(CC) -o bin/pagerank.run $^ $(LDFLAGS)
	@[ -d output ] || mkdir output

main.o: src/main.h src/pagerank.h lib/file.h

pagerank.o: $(LIBS_HDR)


t_matrix: tests/t_matrix.o lib/file.o lib/matrix.o
	@[ -d bin ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)
	@[ -d output ] || mkdir output

t_matrix.o: lib/file.h lib/matrix.h

t_pagerank: tests/t_pagerank.o src/pagerank.o $(LIBS_OBJ)
	@[ -d bin ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)
	@[ -d output ] || mkdir output

t_pagerank.o: lib/file.h lib/matrix.h lib/vector.h src/pagerank.h

t_vector: tests/t_vector.o lib/file.o lib/vector.o
	@[ -d bin ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)
	@[ -d output ] || mkdir output

t_vector.o: lib/file.h lib/vector.h


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
	rm -rf output/*

mrproper: clean
	rm -rf bin/
	rm -rf output/

