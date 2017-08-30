all: ifcidc

ifcidc: ifcidc.c
	gcc -o ifcidc ifcidc.c
