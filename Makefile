all: test

CC = g++
OPT= -g -flto -Ofast
CFLAGS = $(OPT) -Wall
LIBS = -lssl -lcrypto 

test: plagiarism_detection.cc zipf.c hashutil.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f test test.o
