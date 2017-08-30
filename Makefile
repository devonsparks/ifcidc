all: ifcidc

ifcidc: src/ifcidc.c
	gcc -o ifcidc src/ifcidc.c
