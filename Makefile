all: apodiformes tfidf kneserney

apodiformes: src/Apodiformes.cpp
	g++ -std=c++0x -o apodiformes src/Apodiformes.cpp src/VectorSpaceModel.cpp src/Document.cpp src/TFIDF.cpp src/KneserNey.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore
	
tfidf: src/TFIDF.cpp
	g++ -std=c++0x -o tfidf src/VectorSpaceModel.cpp src/Document.cpp src/TFIDF.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore

kneserney: src/KneserNey.cpp
	g++ -std=c++0x -o kneserney src/VectorSpaceModel.cpp src/Document.cpp src/KneserNey.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore

clean:
	rm *o fly