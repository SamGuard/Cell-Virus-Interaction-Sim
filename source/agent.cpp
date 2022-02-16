#include "agent.hpp"

#include "repast_hpc/Moore2DGridQuery.h"
#include "repast_hpc/Point.h"

void Agent::interact(
    repast::SharedContext<Agent>* context,
    repast::SharedDiscreteSpace<Agent, repast::StrictBorders,
                                repast::SimpleAdder<Agent>>* discreteSpace,
    repast::SharedContinuousSpace<Agent, repast::StrictBorders,
                                  repast::SimpleAdder<Agent>>* continSpace) {
    std::vector<Agent*> agentsToPlay;

    std::vector<int> agentLocDiscrete;
    discreteSpace->getLocation(id, agentLocDiscrete);
    repast::Point<int> center(agentLocDiscrete);
    repast::Moore2DGridQuery<Agent> moore2DQuery(discreteSpace);
    moore2DQuery.query(center, 0, true, agentsToPlay);

    std::vector<Agent*>::iterator agentToPlay = agentsToPlay.begin();
    while (agentToPlay != agentsToPlay.end()) {
        //std::cout << "Hi from " << id.id() << " Agent " << (*agentToPlay)->id.id() << std::endl;
        (*agentToPlay)->increment();
        agentToPlay++;

    }
}

void Agent::move(
    repast::SharedDiscreteSpace<Agent, repast::StrictBorders,
                                repast::SimpleAdder<Agent>>* discreteSpace,
    repast::SharedContinuousSpace<Agent, repast::StrictBorders,
                                  repast::SimpleAdder<Agent>>* continSpace) {
    std::vector<double> loc;
    continSpace->getLocation(id, loc);
    loc[0] += vel.x;
    loc[1] += vel.y;
    continSpace->moveTo(id, loc);
    std::vector<int> discLoc;
    discLoc.push_back((int)loc[0]);
    discLoc.push_back((int)loc[1]);
    discreteSpace->moveTo(id, discLoc);
}

/* Serializable Agent Package Data */

AgentPackage::AgentPackage() {}

AgentPackage::AgentPackage(int _id, int _rank, int _type, int _currentRank, double _velx, double _vely, int _testCounter)
    : id(_id),
      rank(_rank),
      type(_type),
      currentRank(_currentRank),
      testCounter(_testCounter),
      velx(_velx),
      vely(_vely){}