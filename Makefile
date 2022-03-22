include ./env

build: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/virus.o ./objects/cell.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main ./objects/* -Wall $(REPAST_HPC_LIB) $(BOOST_LIBS) $(COMP_FLAGS)

build_opt: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/virus.o ./objects/cell.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main ./objects/* -Wall $(REPAST_HPC_LIB) $(BOOST_LIBS) $(COMP_FLAGS) -O3

./objects/cell.o: ./headers/agentbase.hpp ./headers/constants.hpp ./headers/cell.hpp ./source/cell.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/cell.cpp -o ./objects/cell.o $(COMP_FLAGS)

./objects/virus.o: ./headers/constants.hpp ./headers/agentbase.hpp ./headers/virus.hpp ./source/virus.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/virus.cpp -o ./objects/virus.o $(COMP_FLAGS)

./objects/communicators.o: ./headers/constants.hpp ./headers/virus.hpp ./headers/cell.hpp ./headers/communicators.hpp ./source/communicators.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/communicators.cpp -o ./objects/communicators.o $(COMP_FLAGS)

./objects/model.o: ./headers/constants.hpp ./headers/model.hpp ./headers/communicators.hpp ./headers/data_collector.hpp ./headers/cell.hpp ./headers/model.hpp ./source/model.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/model.cpp -o ./objects/model.o $(COMP_FLAGS)

./objects/main.o: ./headers/constants.hpp ./headers/model.hpp ./source/main.cpp 
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/main.cpp -o ./objects/main.o $(COMP_FLAGS)

./data_visualiser/vis_main.cpp:

build_vis: ./data_visualiser/EasyBMP.hpp ./data_visualiser/vis_main.hpp
	cd ./data_visualiser \
	&& g++ vis_main.cpp -o vis_main

del_data:
	- rm ./output/*
	- rm ./data_visualiser/images/*
	- rm ./data_visualiser/output.mp4

clean: del_data
	- rm ./objects/*
	- rm ./bin/*

run: del_data
	mpirun -n 32 ./bin/main config.props model.props

debug: del_data	
		mpirun -np 4 xterm -e gdb --args ./bin/main config.props model.props

run_vis:
	- rm ./data_visualiser/images/*
	cd ./data_visualiser; \
	./vis_main; \

movie:
	cd ./data_visualiser/; \
	ffmpeg -framerate 20 -i ./images/tick_%00d.bmp -c:v libx264 -profile:v high -crf 10 -pix_fmt yuv420p output.mp4

full: build run run_vis movie	