CC=gcc
CFLAGS=-Wall -g -pedantic 
all: clean exec_dpll test_dpll

exec_dpll: exec_dpll.o dpll.o
	$(CC) $(CFLAGS) -o $@ $^

exec_dpll.o: exec_dpll.c dpll.h
	$(CC) $(CFLAGS) -o $@ -c $<

test_dpll: test_dpll.o dpll.o
	$(CC) $(CFLAGS) -o $@ $^

test_dpll.o: test_dpll.c dpll.h
	$(CC) $(CFLAGS) -o $@ -c $<

dpll.o: dpll.c dpll.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm -f dpll
	rm -f exec_dpll
	rm -f test_dpll
