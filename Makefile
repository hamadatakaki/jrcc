jrcc: jrcc.c
		cc -o jrcc jrcc.c

test: jrcc
		./test.sh

remove: jrcc
		rm jrcc

clean: 
		rm -f *.ll *.o tmp*