AFLAGS=-g -o
FLAGS=-g -o
CFLAGS=-g -c -Wall -Wextra -Wwrite-strings
LFLAGS=-g -o prog2

prog2: prog2.o abs.o average.o quadratic.o min.o sum.o product.o
	gcc ${LFLAGS} prog2.o abs.o average.o quadratic.o min.o product.o sum.o

prog2.o: prog2.c 
	gcc ${CFLAGS} prog2.c

abs.o: abs.S
	as ${AFLAGS} abs.o abs.S

average.o: average.S
	as ${AFLAGS} average.o average.S

quadratic.o: quadratic.S
	as ${AFLAGS} quadratic.o quadratic.S

min.o: min.S
	as ${AFLAGS} min.o min.S

sum.o: sum.S
	as ${AFLAGS} sum.o sum.S

product.o: product.S
	as ${AFLAGS} product.o product.S

clean:
	rm -rf prog1
	rm -rf *.o