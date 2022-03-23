#ifndef AGENTBASE
#define AGENTBASE

#include <stdlib.h>

#include "constants.hpp"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/SharedDiscreteSpace.h"

class AgentBase {
   protected:
    double birthTick;
    std::vector<int> attFactors;  // Attatchment factors
    int receptorType;

   public:
    Vector vel;
    repast::AgentId id;
    AgentType agentType;

    AgentBase() {
        vel.x = 0;
        vel.y = 0;
        agentType = BaseAgentType;
        receptorType = 0;
    }

    AgentBase(repast::AgentId id, Vector vel) {
        this->id = id;
        this->vel = vel;
        this->agentType = BaseAgentType;
        this->birthTick = 0;
    }

    virtual ~AgentBase() {}

    // Getters for the serialisation
    virtual repast::AgentId& getId() { return id; }
    virtual const repast::AgentId& getId() const { return id; }

    // Getter for agent specific attributes
    Vector getVel() { return vel; }

    void set(repast::AgentId id, Vector vel, int birthTick, int receptorType, std::vector<int> attFactors) {
        this->id = id;
        this->vel = vel;
        this->birthTick = birthTick;
        this->receptorType = receptorType;
        this->attFactors = attFactors;
    }

    void setReceptorType(int receptorType) {
        this->receptorType = receptorType;
    }

    int getReceptorType() { return receptorType; }
    inline std::vector<int> getAttFactors() { return attFactors; }

    void addAttatchFactor(int x) { attFactors.push_back(x); }
    bool canAttach(int x) {
        for (std::vector<int>::iterator it = attFactors.begin();
             it != attFactors.end(); it++) {
            if (*it == x) {
                return true;
            }
        }
        return false;
    }

    // This is where interactions that change the state of agents take place
    virtual void interact(
        repast::SharedContext<AgentBase>* context,
        repast::SharedDiscreteSpace<AgentBase, repast::StrictBorders,
                                    repast::SimpleAdder<AgentBase>>*
            virusDiscreteSpace,
        repast::SharedContinuousSpace<AgentBase, repast::StrictBorders,
                                      repast::SimpleAdder<AgentBase>>*
            virusContinSpace){};
    // Moves the agent
    virtual void move(
        repast::SharedDiscreteSpace<AgentBase, repast::StrictBorders,
                                    repast::SimpleAdder<AgentBase>>*
            virusDiscreteSpace,
        repast::SharedContinuousSpace<AgentBase, repast::StrictBorders,
                                      repast::SimpleAdder<AgentBase>>*
            virusContinSpace){};
};

#endif