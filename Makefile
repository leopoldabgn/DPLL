CC=gcc
CFLAGS=-Wall -g -pedantic 
all: clean dpll test_dpll

dpll: dpll.o cnf.o
	$(CC) $(CFLAGS) -o $@ $^

dpll.o: dpll.c cnf.h
	$(CC) $(CFLAGS) -o $@ -c $<

test_dpll: test_dpll.o cnf.o
	$(CC) $(CFLAGS) -o $@ $^

test_dpll.o: test_dpll.c cnf.h
	$(CC) $(CFLAGS) -o $@ -c $<

cnf.o: cnf.c cnf.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm -f dpll
	rm -f test_dpll
