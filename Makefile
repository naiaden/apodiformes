all:
	g++ -std=c++0x -o fly src/Apodiformes.cpp src/VectorSpaceModel.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore
	
clean:
	rm *o fly