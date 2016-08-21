COCO_INCLUDE=/home/louis/local/include/colibri-core
COCO_LIB=/home/louis/local/lib

apodiformes: src/Apodiformes.cpp 
	$(CXX) -std=c++11 -g -O3 -Isrc -Iinclude -L$(COCO_LIB) $< -o $@ -lcolibricore -lboost_filesystem -lboost_system -I$(COCO_INCLUDE) -lboost_serialization -lbz2 -lglog

