all: test

CC = g++
OPT = -g -flto -Ofast
CFLAGS = $(OPT) -Wall
LIBS = -lssl -lcrypto 
UTILS = c_utils

test: test.o plagiarism_detection.o $(UTILS)/hashutil.o $(UTILS)/solution_helpers.o $(UTILS)/test_helpers.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

test.o: test.cc $(UTILS)/test_helpers.h plagiarism_detection.cc
	$(CC) $(CFLAGS) -c $< -o $@

plagiarism_detection.o: plagiarism_detection.cc $(UTILS)/solution_helpers.h
	$(CC) $(CFLAGS) -c $< -o $@

$(UTILS)/hashutil.o: $(UTILS)/hashutil.h $(UTILS)/hashutil.c
	$(CC) $(CFLAGS) -c $(UTILS)/hashutil.c -o $@

$(UTILS)/solution_helpers.o: $(UTILS)/solution_helpers.h $(UTILS)/solution_helpers.c $(UTILS)/hashutil.h
	$(CC) $(CFLAGS) -c $(UTILS)/solution_helpers.c -o $@

$(UTILS)/test_helpers.o: $(UTILS)/test_helpers.h $(UTILS)/test_helpers.c
	$(CC) $(CFLAGS) -c $(UTILS)/test_helpers.c -o $@

clean:
	rm -f test *.o
