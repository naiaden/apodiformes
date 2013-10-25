all: fly

fly:
	g++ -std=c++0x -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore src/Apodiformes.cpp -o fly
	
clean:
	rm *o fly