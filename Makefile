all: apodiformes# tfidf kneserney

apodiformes: src/Apodiformes.cpp src/KneserNey.cpp
	g++ -std=c++0x -g -O0 -o apodiformes src/Apodiformes.cpp src/KneserNey.cpp src/File.cpp src/Common.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore -lbz2 -I/home/lonrust/local/include -L/home/lonrust/local/lib -lglog
	
#tfidf: src/TFIDF.cpp
#	g++ -std=c++0x -o tfidf src/VectorSpaceModel.cpp src/Document.cpp src/TFIDF.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore

#kneserney: src/KneserNey.cpp
#	g++ -std=c++0x -o kneserney src/VectorSpaceModel.cpp src/Document.cpp src/KneserNey.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore

clean:
	rm *.o apodiformes tfidf kneserney
