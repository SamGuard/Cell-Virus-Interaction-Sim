#ifndef MODEL
#define MODEL

#include <stdio.h>

#include <boost/mpi.hpp>
#include <fstream>
#include <iostream>
#include <tuple>

#include "communicators.hpp"
#include "data_collector.hpp"
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

struct AgentContexts {
    repast::SharedContext<Virus>* virus;
    repast::SharedContext<Cell>* cell;
};

struct AgentSpaces {
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus>>* virusCont;
    repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                repast::SimpleAdder<Virus>>* virusDisc;

    repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                repast::SimpleAdder<Cell>>* cellDisc;
};

struct AgentComm {
    VirusPackageProvider* virusProv;
    VirusPackageReceiver* virusRec;

    CellPackageProvider* cellProv;
    CellPackageReceiver* cellRec;
};

class Model {
   public:
    Model(std::string propsFile, int argc, char** argv,
          boost::mpi::communicator* comm);

    int rank, worldSize;
    unsigned int lifetime;
    int countOfAgents, cellCount;
    std::string propsFile;
    repast::Properties* props;

    AgentContexts contexts;
    AgentSpaces spaces;
    AgentComm comms;

    DataCollector dataCol;
    std::stringstream simData;
    std::ofstream simDataFile;

    // ------------------------------------------------

    void init();
    void initSchedule(repast::ScheduleRunner& runner);
    void balanceAgents();

    void addVirus(repast::Point<double> loc);

    void move();
    void interact();
    void collectVirusData();
    void collectCellData();
    void outputTick();
    void printTick();
    void printAgentCounters();
};
#endif