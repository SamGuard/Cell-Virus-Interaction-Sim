#ifndef MODEL
#define MODEL

#include <stdio.h>

#include <boost/mpi.hpp>
#include <iostream>

#include "agent.hpp"
#include "communicators.hpp"
#include "repast_hpc/AgentRequest.h"
#include "repast_hpc/GridComponents.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedDiscreteSpace.h"
#include "repast_hpc/TDataSource.h"

class Model {
   public:
    Model(std::string propsFile, int argc, char** argv,
          boost::mpi::communicator* comm);

    unsigned int lifetime;
    int countOfAgents;
    std::string propsFile;
    repast::Properties* props;

    AgentPackageProvider* provider;
    AgentPackageReceiver* receiver;

    repast::SharedDiscreteSpace<Agent, repast::StrictBorders,
                                repast::SimpleAdder<Agent> >* discreteSpace;
    repast::SharedContinuousSpace<Agent, repast::StrictBorders,
                                  repast::SimpleAdder<Agent> >* continSpace;

    repast::SharedContext<Agent> context;

    void init();
    void initSchedule(repast::ScheduleRunner& runner);

    void move();
    void interact();
    void printAgentCounters();
};
#endif