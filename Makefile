all: apodiformes# tfidf kneserney

apodiformes: src/Apodiformes.cpp src/KneserNey.cpp
	g++ -std=c++11 -g -O0 -o apodiformes src/Apodiformes.cpp src/KneserNey.cpp src/File.cpp src/Common.cpp -I/users/spraak/onrust/software/apodiformes/include -I/users/spraak/onrust/software/colibri-core/include -L/users/spraak/onrust/software/colibri-core/src/.libs -lcolibricore -lbz2 -I/users/spraak/lonrust/local/include -L/users/spraak/onrust/local/lib -lglog
	
#tfidf: src/TFIDF.cpp
#	g++ -std=c++0x -o tfidf src/VectorSpaceModel.cpp src/Document.cpp src/TFIDF.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore

#kneserney: src/KneserNey.cpp
#	g++ -std=c++0x -o kneserney src/VectorSpaceModel.cpp src/Document.cpp src/KneserNey.cpp -I/home/lonrust/Software/apodiformes/include -I/home/lonrust/Software/colibri-core/include -L/home/lonrust/Software/colibri-core/src/.libs -lcolibricore

clean:
	rm *.o apodiformes tfidf kneserney
