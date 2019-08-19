compile: jrcc.c
		cc -Wall -o jrcc jrcc.c

test: jrcc
		./test.sh

remove: jrcc
		rm jrcc

clean: 
		rm -f *.ll *.o tmp*

compile-strong: jrcc.c
		cc -Weverything -o jrcc jrcc.c