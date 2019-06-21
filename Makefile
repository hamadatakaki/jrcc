jrcc: jrcc.c
		rm jrcc
		cc -o jrcc jrcc.c

test: jrcc
		./test.sh

clean: 
		rm -f *.ll *.o tmp*