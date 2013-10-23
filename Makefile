all:
	clang++ -Iinclude -std=c++11 -stdlib=libc++ -I/usr/local/include/colibri-core  -I/usr/local/include -I/usr/local/Cellar/libxml2/2.9.1/include/libxml2 -L/usr/local/include  -L/usr/local/lib -lcolibricore src/apodiformes.cpp -o fly
    
clean:
	rm -f *o fly
