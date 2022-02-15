#include "model.hpp"

#include <stdio.h>

#include <boost/mpi.hpp>
#include <vector>

#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/initialize_random.h"

Model::Model(std::string propsFile, int argc, char** argv,
             boost::mpi::communicator* comm) {
    std::cout << "Model created" << std::endl;
    props = new repast::Properties(propsFile, argc, argv, comm);
    initializeRandom(*props, comm);
    context = repast::SharedContext<Agent>(comm);
    provider = new AgentPackageProvider(&context);
    receiver = new AgentPackageReceiver(&context);

    repast::Point<double> origin(-100,-100);
    repast::Point<double> extent(200, 200);
    
    repast::GridDimensions gd(origin, extent);
    
    std::vector<int> processDims;
    processDims.push_back(2);
    processDims.push_back(2);
    
    discreteSpace = new repast::SharedDiscreteSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >("AgentDiscreteSpace", gd, processDims, 2, comm);
    continSpace = new repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders, repast::SimpleAdder<Agent> >("AgentContinuousSpace", gd, processDims, 0, comm);
	
    context.addProjection(continSpace);
    context.addProjection(discreteSpace);

}

void Model::init() {
    int countOfAgents = 5;
    int rank = repast::RepastProcess::instance()->rank();
    for (int i = 0; i < countOfAgents; i++) {
        repast::Point<int> initialLocationDiscrete(
            (int)discreteSpace->dimensions().origin().getX() + i,
            (int)discreteSpace->dimensions().origin().getY() + i);
        repast::Point<double> initialLocationContinuous(
            (double)continSpace->dimensions().origin().getX() + i,
            (double)continSpace->dimensions().origin().getY() + i);
        repast::AgentId id(i, rank, 0);
        id.currentRank(rank);
        Vector vel;
        vel.x = repast::Random::instance()->nextDouble();
        vel.y = repast::Random::instance()->nextDouble();
        Agent* agent = new Agent(id, vel);
        context.addAgent(agent);
        discreteSpace->moveTo(id, initialLocationDiscrete);
        continSpace->moveTo(id, initialLocationContinuous);
    }
}

void Model::initSchedule(repast::ScheduleRunner& runner) { return; }