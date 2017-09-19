all: bin/libifcidc.so
all: doc/ifcidc.html

all: bin/ifcc

src:; \
        mkdir -p $@
ex:;\
        mkdir -p $@
bin:; \
        mkdir -p $@
doc:;\
        mkdir -p $@

src/ifcidc.c: ifcidc.nw | src;\
        notangle -Rifcidc.c ifcidc.nw > $@

src/ifcidc.h: ifcidc.nw | src;\
        notangle -Rifcidc.h ifcidc.nw > $@

bin/ifcidc.o: src/ifcidc.c src/ifcidc.h | bin;\
        gcc -c -o $@ -Wall -Werror -fpic src/ifcidc.c

bin/libifcidc.so: bin/ifcidc.o | bin;\
        gcc -shared -o $@ bin/ifcidc.o

doc/ifcidc.html: ifcidc.nw | doc;\
        noweave -html ifcidc.nw > $@




src/ifcc.c: src/ifcidc.h | src;\
        notangle -Rifcc.c ifcidc.nw > $@

bin/ifcc: src/ifcc.c bin/libifcidc.so | src bin;\
        gcc -o $@ src/ifcc.c -Lbin -lifcidc

ex/cguids.txt: ex;\
        notangle -Rcguids.txt ifcidc.nw > $@

ex/uguids.txt: ex;\
        notangle -Ruguids.txt ifcidc.nw > $@

.PHONY: abtest
abtest: ex/cguids.txt ex/uguids.txt;\
        ./bin/ifcc -c -i ex/uguids.txt | diff -q - ex/cguids.txt;\
        ./bin/ifcc -x -i ex/cguids.txt | diff -q - ex/uguids.txt

.PHONY: twopasstest
twopasstest: ex/cguids.txt ex/uguids.txt;\
        ./bin/ifcc -c -i ex/uguids.txt | ./bin/ifcc -x | diff -q - ex/uguids.txt;\
        ./bin/ifcc -x -i ex/cguids.txt | ./bin/ifcc -c | diff -q - ex/cguids.txt


.PHONY: test
test: abtest twopasstest



