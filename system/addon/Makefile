all: complex.o fft.o data/pre-fft.o
	g++ -o test_check complex.o fft.o test_check.cc

clean:
	rm -rf *.o test_fft */.o

reset:
	echo "0" > ./filenum
	rm filelib
