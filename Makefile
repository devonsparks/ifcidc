all: ifcidc

ifcidc: src/ifcc.c
	gcc -Wall -o ifcc src/ifcc.c src/ifcidc.c

ifctest: src/ifctest.c
	gcc -Wall -o ifctest src/ifctest.c src/ifcidc.c
