#include "model.hpp"
#include "ctime"


int main(int argc, char** argv){
	std::time_t startTime = std::time(NULL);

  std::string configFile = argv[1]; // The name of the configuration file is Arg 1
	std::string propsFile  = argv[2]; // The name of the properties file is Arg 2
	
	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator world;

	repast::RepastProcess::init(configFile);

	Model model = Model(propsFile, argc, argv, &world);
	repast::ScheduleRunner& runner = repast::RepastProcess::instance()->getScheduleRunner();

	model.initDataLogging();
	model.init();
	model.initSchedule(runner);

	runner.run();  

	repast::RepastProcess::instance()->done();

	std::time_t endTime = std::time(NULL);

	if(repast::RepastProcess::instance()->rank() == 0){
		cout << "Simulation Finished in " << endTime - startTime << " seconds" << std::endl;
	}
}