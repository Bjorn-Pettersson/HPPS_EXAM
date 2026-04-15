CC?=cc
CFLAGS?=-O3 -Wall -Wextra -pedantic -fopenmp
LDFLAGS?=-fopenmp -lm
PROGRAMS= gen-ints gen-points ints2text points2text strings2mapping strings2ints run-dist-histogram run-histogram

.SUFFIXES:

all: $(PROGRAMS)

run-dist-histogram: run-dist-histogram.o util.o dist-histogram.o
	$(CC) -o $@ $^ $(LDFLAGS)

run-histogram: run-histogram.o util.o histogram.o
	$(CC) -o $@ $^ $(LDFLAGS)

%: %.o util.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ $^ -c $(CFLAGS)

clean:
	rm -f *.o $(PROGRAMS)
