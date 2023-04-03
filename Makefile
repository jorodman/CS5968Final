all: test

CC = g++
OPT= -g -flto -Ofast
CFLAGS = $(OPT) -Wall
LIBS = -lssl -lcrypto 
UTILS = c_utils

test: test.cc $(UTILS)/zipf.c $(UTILS)/hashutil.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f test test.o
