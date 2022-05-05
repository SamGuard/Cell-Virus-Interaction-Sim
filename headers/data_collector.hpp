#ifndef DATA_COLLECTOR
#define DATA_COLLECTOR

#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "cell.hpp"
#include "globals.hpp"
#include "math.h"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/TDataSource.h"

// Used to write visual data to files
class DataCollector {
   private:
    std::stringstream *data;
    std::ofstream *file;

   public:
    DataCollector() {
        this->data = NULL;
        this->file = NULL;
    }
    DataCollector(std::stringstream *data, std::ofstream *file) {
        this->data = data;
        this->file = file;
    }

    void newAgent(repast::AgentId id);
    void killAgent(repast::AgentId id);
    void setPos(repast::AgentId id, std::vector<double> pos,
                bool printInstruction);
    void setState(repast::AgentId id, int state, bool printInstruction);
    void writeData() {
        if (!VIS_DATA_OUTPUT) return;
        (*file) << data->rdbuf();
    }
};

// This class tracks particle counts
template <class T>
class AgentTotals : public repast::TDataSource<int> {
   private:
    AgentType type;
    repast::SharedContext<T> *cont;

   public:
    AgentTotals(repast::SharedContext<T> *cont, AgentType type) {
        this->cont = cont;
        this->type = type;
    }

    int getData() {
        int count = 0;
        for (typename repast::SharedContext<T>::const_local_iterator it =
                 cont->localBegin();
             it != cont->localEnd(); it++) {
            if ((*it)->getAgentType() == type) {
                count++;
            }
        }
        return count;
    }
};

// This class tracks cell type counts
template <>
class AgentTotals<Cell> : public repast::TDataSource<int> {
   private:
    AgentType type;
    repast::SharedContext<Cell> *cont;
    CellState stateFilter;

   public:
    AgentTotals(repast::SharedContext<Cell> *cont, AgentType type,
                CellState state) {
        this->cont = cont;
        this->type = type;
        this->stateFilter = state;
    }

    int getData() {
        int count = 0;
        for (repast::SharedContext<Cell>::const_local_iterator it =
                 cont->localBegin();
             it != cont->localEnd(); it++) {
            if ((*it)->getAgentType() == type &&
                (*it)->getState() == stateFilter) {
                count++;
            }
        }
        return count;
    }
};

// Output time in minutes instead of ticks
class TrueTime : public repast::TDataSource<double> {
   public:
    double getData() {
        return (int)repast::RepastProcess::instance()
                   ->getScheduleRunner()
                   .currentTick() *
               SIM_TIME_SCALE / repast::RepastProcess::instance()->worldSize();
    }
};
#endif