rayCaster: rayCaster.cpp file_utils.o utils.o
	g++ -o rayCaster rayCaster.cpp file_utils.o utils.o

file_utils: file_utils.h file_utils.cpp utils.o
	g++ -o file_utils file_utils.h file_utils.cpp utils.o

utils: utils.h utils.cpp
	g++ -o utils utils.h utils.cpp


clean:
	rm -f utils.o
	rm -f file_utils.o
	rm -f rayCaster
	rm -f a.out