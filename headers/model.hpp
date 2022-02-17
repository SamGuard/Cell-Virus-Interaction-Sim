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

    // Agent stuff
    VirusPackageProvider* virusProvider;
    VirusPackageReceiver* virusReceiver;
    repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                repast::SimpleAdder<Virus> >* virusDiscreteSpace;
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus> >* virusContinSpace;
    repast::SharedContext<Virus> context;
    // ------------------------------------------------

    void init();
    void initSchedule(repast::ScheduleRunner& runner);

    void move();
    void interact();
    void printAgentCounters();
};
#endif