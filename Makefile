rayCaster: rayCaster.cpp utils.o
	g++ -o rayCaster rayCaster.cpp utils.o

utils: utils.h utils.cpp
	g++ -o utils utils.h utils.cpp

clean:
	rm -f utils
	rm -f rayCaster
	rm -f a.out