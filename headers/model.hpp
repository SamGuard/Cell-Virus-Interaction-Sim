#ifndef MODEL
#define MODEL

#include <stdio.h>

#include <boost/mpi.hpp>
#include <fstream>
#include <iostream>
#include <tuple>

#include "communicators.hpp"
#include "repast_hpc/AgentRequest.h"
#include "repast_hpc/GridComponents.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "repast_hpc/SVDataSource.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedDiscreteSpace.h"
#include "repast_hpc/TDataSource.h"
#include "virus.hpp"


struct AgentContexts{
    repast::SharedContext<Virus>* virus;
};

struct AgentSpaces {
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus>>* virusCont;
    repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                repast::SimpleAdder<Virus>>* virusDisc;
};

struct AgentComm{
    VirusPackageProvider* virusProv;
    VirusPackageReceiver* virusRec;
};

class Model {
   public:
    Model(std::string propsFile, int argc, char** argv,
          boost::mpi::communicator* comm);

    int rank, worldSize;
    unsigned int lifetime;
    int countOfAgents;
    std::string propsFile;
    repast::Properties* props;

    AgentContexts contexts;
    AgentSpaces spaces;
    AgentComm comms;

    // Virus stuff
    /*
    VirusPackageProvider* virusProvider;
    VirusPackageReceiver* virusReceiver;
    repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                repast::SimpleAdder<Virus>>* virusDiscreteSpace;
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus>>* virusContinSpace;
    repast::SharedContext<Virus> context;
    */
    std::stringstream virusPosData;
    std::ofstream virusPosDataFile;

    // ------------------------------------------------

    void init();
    void initSchedule(repast::ScheduleRunner& runner);
    void balanceAgents();

    void move();
    void interact();
    void collectVirusData();
    void writeVirusData();
    void printTick();
    void printAgentCounters();
};
#endif