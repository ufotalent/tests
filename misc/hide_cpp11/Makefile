.PHONY : all
all:
	g++ -std=c++11 -c -fPIC impl_cpp11.cc
	g++ --shared impl_cpp11.o -o libcalc.so
	gcc -c call.c
	gcc -o calc call.o -rdynamic -Wl,-L. -Wl,-rpath=\$ORIGIN/ -lcalc

.PHONY : clean
clean:
	rm -rf *.o *.so calc
