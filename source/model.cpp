#include "model.hpp"


#include <stdio.h>
#include <vector>
#include <boost/mpi.hpp>
#include "repast_hpc/AgentId.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/initialize_random.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "repast_hpc/Point.h"

void Model::init(std::string propsFile, int argc, char** argv, boost::mpi::communicator* comm){
  props = new repast::Properties(propsFile, argc, argv, comm);
  int countOfAgents = 5;
  int rank = repast::RepastProcess::instance()->rank();
	for(int i = 0; i < countOfAgents; i++){
        repast::Point<int> initialLocationDiscrete((int)discreteSpace->dimensions().origin().getX() + i,(int)discreteSpace->dimensions().origin().getY() + i);
        repast::Point<double> initialLocationContinuous((double)continSpace->dimensions().origin().getX() + i,(double)continuousSpace->dimensions().origin().getY() + i);
		repast::AgentId id(i, rank, 0);
		id.currentRank(rank);
    Vector vel;
    vel.x = repast::Random::instance()->nextDouble();
    vel.y = repast::Random::instance()->nextDouble();
		Agent* agent = new Agent(id, vel);
		context.addAgent(agent);
        discreteSpace->moveTo(id, initialLocationDiscrete);
        continuousSpace->moveTo(id, initialLocationContinuous);
	}
}

void Model::initSchedule(){
  return;
}