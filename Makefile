include ./env

./objects/agent.o: 
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/agent.cpp -o ./objects/agent.o

./objects/communicators.o: ./headers/agent.hpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/communicators.cpp -o ./objects/communicators.o

./objects/model.o: ./headers/model.hpp ./headers/communicators.hpp ./source/model.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/model.cpp -o ./objects/model.o


./objects/main.o: ./headers/model.hpp ./source/main.cpp 
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/main.cpp -o ./objects/main.o

build: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/agent.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main  ./objects/main.o ./objects/model.o $(REPAST_HPC_LIB) $(BOOST_LIBS)
clean:
	rm ./bin/*
	rm ./objects/*