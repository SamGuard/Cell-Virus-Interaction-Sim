#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include "repast_hpc/AgentId.h"

/* Data Collection */
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

    void newAgent(repast::AgentId id) {
        *data << "created:" << id.id() << "|" << id.startingRank() << "|"
              << id.agentType() << std::endl;
    }

    void setPos(repast::AgentId id, std::vector<double> pos,
                bool printInstruction) {
        if (printInstruction) {
            *data << "setpos:";
        }
        *data << id.id() << "|" << id.startingRank() << "|" << id.agentType()
              << "|" << pos[0] << "|" << pos[1];

        if (printInstruction) {
            *data << std::endl;
        } else {
            *data << ",";
        }
    }

    void setState(repast::AgentId id, int state, bool printInstruction) {
        if (printInstruction) {
            *data << "setstate:";
        }
        *data << id.id() << "|" << id.startingRank() << "|" << id.agentType()
              << "|" << state;

        if (printInstruction) {
            *data << std::endl;
        } else {
            *data << ",";
        }
    }

    void writeData() { (*file) << data->rdbuf(); }
};
