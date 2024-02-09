all: prog0
prog0: prog0.o libuser.a
	gcc -Wall -g -o prog0 prog0.o -L. -luser
prog0.o: prog0.c user.h
	gcc -Wall -g -c prog0.c -I user.h
libuser.a: user.o
	ar rcs libuser.a user.o
user.o: user.c user.h
	gcc -Wall -g -c user.c -I user.h
clean:
	rm -f prog0 prog0.o libuser.a user.o