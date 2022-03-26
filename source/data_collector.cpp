#include "data_collector.hpp"

void DataCollector::newAgent(repast::AgentId id) {
    *data << "create:" << id.id() << "|" << id.startingRank() << "|"
          << id.agentType() << std::endl;
}

void DataCollector::killAgent(repast::AgentId id) {
    *data << "kill:" << id.id() << "|" << id.startingRank() << "|"
          << id.agentType() << std::endl;
}

void DataCollector::setPos(repast::AgentId id, std::vector<double> pos,
            bool printInstruction) {
    if (printInstruction) {
        *data << "setpos:";
    }
    *data << id.id() << "|" << id.startingRank() << "|" << id.agentType() << "|"
          << pos[0] << "|" << pos[1];

    if (printInstruction) {
        *data << std::endl;
    } else {
        *data << ",";
    }
}

void DataCollector::setState(repast::AgentId id, int state, bool printInstruction) {
    if (printInstruction) {
        *data << "setstate:";
    }
    *data << id.id() << "|" << id.startingRank() << "|" << id.agentType() << "|"
          << state;

    if (printInstruction) {
        *data << std::endl;
    } else {
        *data << ",";
    }
}