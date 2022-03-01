include ./env

build: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/virus.o ./objects/data_collectors.o ./objects/cell.o ./objects/constants.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main ./objects/* -Wall $(REPAST_HPC_LIB) $(BOOST_LIBS) $(COMP_FLAGS)

build_opt: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/virus.o ./objects/data_collectors.o ./objects/cell.o ./objects/constants.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main ./objects/* -Wall $(REPAST_HPC_LIB) $(BOOST_LIBS) $(COMP_FLAGS) -O3

./objects/constants.o: ./source/constants.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/constants.cpp -o ./objects/constants.o $(COMP_FLAGS)

./objects/cell.o: ./headers/agentbase.hpp ./headers/constants.hpp ./source/cell.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/cell.cpp -o ./objects/cell.o $(COMP_FLAGS)

./objects/data_collectors.o: ./headers/constants.hpp ./source/data_collectors.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/data_collectors.cpp -o ./objects/data_collectors.o $(COMP_FLAGS)

./objects/virus.o: ./headers/constants.hpp ./headers/agentbase.hpp ./source/virus.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/virus.cpp -o ./objects/virus.o $(COMP_FLAGS)

./objects/communicators.o: ./headers/constants.hpp ./headers/virus.hpp ./headers/cell.hpp ./source/communicators.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/communicators.cpp -o ./objects/communicators.o $(COMP_FLAGS)

./objects/model.o: ./headers/constants.hpp ./headers/model.hpp ./headers/communicators.hpp ./headers/data_collectors.hpp ./headers/cell.hpp ./source/model.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/model.cpp -o ./objects/model.o $(COMP_FLAGS)

./objects/main.o: ./headers/constants.hpp ./headers/model.hpp ./source/main.cpp 
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/main.cpp -o ./objects/main.o $(COMP_FLAGS)


del_data:
	-rm ./output/*

clean: del_data
	- rm ./objects/*
	- rm ./bin/*

run: del_data
	mpirun -n 4 ./bin/main config.props model.props

debug: del_data
		mpirun -np 4 xterm -e gdb --args ./bin/main config.props model.props

run_vis:
	cd ./data_visualiser; \
	python3 ./visuals.py

movie:
	cd ./data_visualiser/images/; \
	ffmpeg -framerate 20 -i tick_%00d.png -c:v libx264 -profile:v high -crf 20 -pix_fmt yuv420p output.mp4
