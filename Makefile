
.PHONY: default
default: all

srcdir=src
bindir=bin
incdir=inc
libdir=lib

cc=LD_LIBRARY_PATH=$(libdir) gcc
cflags=-I$(incdir) -Wall -g
exdir=ex

$(libdir)/libifcidc.so: $(srcdir)/ifcidc.c
	mkdir -p $(libdir)
	$(cc) $(cflags) -shared -fPIC -o $@ $^
all: $(libdir)/libifcidc.so
$(bindir)/ifcc: $(srcdir)/ifcc.c $(libdir)/libifcidc.so
	mkdir -p $(bindir)
	$(cc) $(cflags) -L$(libdir) -lifcidc -o $@ $^
all: $(bindir)/ifcc


.PHONY: test1pass
test1pass: $(bindir)/ifcc $(exdir)/uguids.txt $(exdir)/cguids.txt
	$(bindir)/ifcc -c -i ex/uguids.txt | diff -q - ex/cguids.txt;\
	$(bindir)/ifcc -x -i ex/cguids.txt | diff -q - ex/uguids.txt



.PHONY: test2pass
test2pass: 
	./bin/ifcc -c -i ex/uguids.txt | ./bin/ifcc -x | diff -q - ex/uguids.txt;\
	./bin/ifcc -x -i ex/cguids.txt | ./bin/ifcc -c | diff -q - ex/cguids.txt


.PHONY: check
check: test1pass test2pass

.PHONY: clean
clean:
	rm -rf $(bindir) $(incdir) $(srcdir) $(libdir) $(exdir) Makefile
