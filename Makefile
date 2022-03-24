include ./env

build: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/particle.o ./objects/cell.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main ./objects/* -Wall $(REPAST_HPC_LIB) $(BOOST_LIBS) $(COMP_FLAGS)

build_opt: ./objects/main.o ./objects/model.o ./objects/communicators.o ./objects/particle.o ./objects/cell.o
	$(MPICXX) $(BOOST_LIB_DIR) $(REPAST_HPC_LIB_DIR) -o ./bin/main ./objects/* -Wall $(REPAST_HPC_LIB) $(BOOST_LIBS) $(COMP_FLAGS) -O3

./objects/cell.o: ./headers/agentbase.hpp ./headers/constants.hpp ./headers/cell.hpp ./source/cell.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/cell.cpp -o ./objects/cell.o $(COMP_FLAGS)

./objects/particle.o: ./headers/constants.hpp ./headers/agentbase.hpp ./headers/particle.hpp ./source/particle.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/particle.cpp -o ./objects/particle.o $(COMP_FLAGS)

./objects/communicators.o: ./headers/constants.hpp ./headers/particle.hpp ./headers/cell.hpp ./headers/communicators.hpp ./source/communicators.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/communicators.cpp -o ./objects/communicators.o $(COMP_FLAGS)

./objects/model.o: ./headers/constants.hpp ./headers/model.hpp ./headers/communicators.hpp ./headers/data_collector.hpp ./headers/cell.hpp ./headers/model.hpp ./source/model.cpp
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/model.cpp -o ./objects/model.o $(COMP_FLAGS)

./objects/main.o: ./headers/constants.hpp ./headers/model.hpp ./source/main.cpp 
	$(MPICXX) $(REPAST_HPC_DEFINES) $(BOOST_INCLUDE) $(REPAST_HPC_INCLUDE) -c -I./headers/ ./source/main.cpp -o ./objects/main.o $(COMP_FLAGS)

./data_visualiser/vis_main.cpp:

build_vis: ./data_visualiser/vis_main.hpp
	cd ./data_visualiser \
	&& g++ vis_main.cpp -o vis_main -lsfml-graphics

del_data:
	- rm ./output/*
	- rm ./data_visualiser/images/*
	- rm ./data_visualiser/output.mp4

clean: del_data
	- rm ./objects/*
	- rm ./bin/*

run: del_data
	mpirun -n $(NUM_PROCS) ./bin/main config.props model.props procDimsX=$(procDimsX) procDimsY=$(procDimsY)

debug: del_data	
		mpirun -np $(NUM_PROCS) xterm -e gdb --args ./bin/main config.props model.props procDimsX=$(procDimsX) procDimsY=$(procDimsY)

run_vis:
	- rm ./data_visualiser/images/*
	cd ./data_visualiser \
	&& ./vis_main \

movie:
	cd ./data_visualiser/; \
	ffmpeg -framerate 20 -i ./images/tick_%00d.png -c:v libx264 -profile:v high -crf 10 -pix_fmt yuv420p output.mp4

full: build build_vis run run_vis movie	