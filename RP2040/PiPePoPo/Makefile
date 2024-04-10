
#Compiles test version of the fixed and floating point yin versions
compile:
	gcc -c yin_fixed.c 			-W -Wall -std=c11 -pedantic -O2
	gcc -c yin_floating.c 		-W -Wall -std=c11 -pedantic -O2
	gcc -c fixedpoint_math.c 	-W -Wall -std=c11 -pedantic -O2 
	gcc -c fixed_point.c 		-W -Wall -std=c11 -pedantic -O2
	mkdir -p bin
	gcc -o bin/yin_fixed *.o 	-lc -lm -ffast-math -pthread

clean:
	rm -f *.o
	rm -f bin/*
	rmdir bin
