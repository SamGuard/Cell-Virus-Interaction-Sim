#include "model.hpp"


int main(int argc, char** argv){
  std::string configFile = argv[1]; // The name of the configuration file is Arg 1
	std::string propsFile  = argv[2]; // The name of the properties file is Arg 2
	
	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator world;

	repast::RepastProcess::init(configFile);

	Model model = Model();
	model.init(propsFile, argc, argv, &world);
	model.initSchedule();

  
}