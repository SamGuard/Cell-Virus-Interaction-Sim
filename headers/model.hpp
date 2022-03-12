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

class SpaceTranslator {
    std::vector<double> cellVirOffset;
    std::vector<double> cellVirScale;

   public:
    SpaceTranslator() {}
    SpaceTranslator(repast::Point<double> virOrigin,
                    repast::Point<double> virExtent,
                    repast::Point<double> cellOrigin,
                    repast::Point<double> cellExtent) {
        cellVirOffset = std::vector<double>();
        cellVirScale = std::vector<double>();

        cellVirOffset.push_back(virOrigin[0] - cellOrigin[0]);
        cellVirOffset.push_back(virOrigin[1] - cellOrigin[1]);

        cellVirScale.push_back(virExtent[0] / (double)cellExtent[0]);
        cellVirScale.push_back(virExtent[1] / (double)cellExtent[1]);
    }

    repast::Point<int> virToCell(repast::Point<double> in) {
        repast::Point<int> out(
            (in[0] - cellVirOffset[0]) / cellVirScale[0] - 0.5,
            (in[1] - cellVirOffset[1]) / cellVirScale[1] - 0.5);

        return out;
    }

    repast::Point<double> cellToVir(repast::Point<int> in) {
        repast::Point<double> out(
            ((double)in[0] + 0.5) * cellVirScale[0] + cellVirOffset[0],
            ((double)in[1] + 0.5) * cellVirScale[1] + cellVirOffset[1]);

        return out;
    }
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
    SpaceTranslator spaceTrans;

    DataCollector dataCol;
    std::stringstream simData;
    std::ofstream simDataFile;

    // ------------------------------------------------

    void init();
    void initSchedule(repast::ScheduleRunner& runner);
    void balanceAgents();

    void move();
    void interact();
    void collectVirusData();
    void collectCellData();
    void outputTick();
    void printTick();
    void printAgentCounters();
};
#endif