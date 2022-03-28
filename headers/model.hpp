#ifndef MODEL
#define MODEL

#include <stdio.h>

#include <boost/mpi.hpp>
#include <fstream>
#include <iostream>
#include <tuple>

#include "communicators.hpp"
#include "data_collector.hpp"
#include "human_response.hpp"
#include "particle.hpp"
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

struct AgentContexts {
    repast::SharedContext<Particle>* part;
    repast::SharedContext<Cell>* cell;
};

struct AgentSpaces {
    repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                  repast::SimpleAdder<Particle>>* partCont;
    repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                repast::SimpleAdder<Particle>>* partDisc;

    repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                repast::SimpleAdder<Cell>>* cellDisc;
};

struct AgentComm {
    ParticlePackageProvider* partProv;
    ParticlePackageReceiver* partRec;

    CellPackageProvider* cellProv;
    CellPackageReceiver* cellRec;
};

class Model {
   public:
    Model(std::string propsFile, int argc, char** argv,
          boost::mpi::communicator* comm);

    int rank, worldSize;
    unsigned int lifetime;
    int virusCount, cellCount;
    std::string propsFile;
    repast::Properties* props;

    AgentContexts contexts;
    AgentSpaces spaces;
    AgentComm comms;

    HumanResponse human;

    DataCollector dataCol;
    std::stringstream simData;
    std::ofstream simDataFile;

    repast::SVDataSet* agentTotalData;

    // ------------------------------------------------
    void initDataLogging();
    void init();
    void initSchedule(repast::ScheduleRunner& runner);
    void balanceAgents();

    void addParticle(repast::Point<double> loc, AgentType t);
    void addParticle(AgentType t);
    void addParticles(
        std::vector<std::tuple<repast::Point<double>, AgentType>>& p);
    void removeParticle(Particle* p);
    void removeParticles(std::set<Particle*>& p);

    void move();
    void interact();
    void collectParticleData();
    void collectCellData();
    void outputTick();
    void printTick();
    void printAgentCounters();
};
#endif