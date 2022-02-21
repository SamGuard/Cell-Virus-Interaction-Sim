include ./env

./objects/data_collectors.o: ./source/data_collectors.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/data_collectors.cpp -o ./objects/data_collectors.o -Wall
./objects/agent.o: ./source/agent.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/agent.cpp -o ./objects/agent.o -Wall

./objects/communicators.o: ./headers/agent.hpp ./source/communicators.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/communicators.cpp -o ./objects/communicators.o -Wall

./objects/model.o: ./headers/model.hpp ./headers/communicators.hpp ./headers/data_collectors.hpp ./source/model.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/model.cpp -o ./objects/model.o -Wall

./objects/main.o: ./headers/model.hpp ./source/main.cpp 
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/main.cpp -o ./objects/main.o -Wall

build: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/agent.o ./objects/data_collectors.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main ./objects/* -Wall $(REPAST_HPC_LIB) $(BOOST_LIBS)

clean:
	- rm ./output/*
	- rm ./objects/*
	- rm ./bin/*

del_data:
	-rm ./output/*
