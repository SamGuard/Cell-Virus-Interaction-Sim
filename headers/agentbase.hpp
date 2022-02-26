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
   public:
    Vector vel;
    repast::AgentId id;
    AgentType agentType;

    AgentBase() {
        this->vel.x = 0;
        this->vel.y = 0;
        this->agentType = BaseAgentType;
    }

    AgentBase(repast::AgentId id, Vector vel) {
        this->id = id;
        this->vel = vel;
        this->agentType = BaseAgentType;
    }

    virtual ~AgentBase() {}

    // Getters for the serialisation
    virtual repast::AgentId& getId() { return id; }
    virtual const repast::AgentId& getId() const { return id; }

    // Getter for agent specific attributes
    Vector getVel() { return vel; }

    virtual void set(repast::AgentId id, Vector vel, int testCounter) {
        this->id = id;
        this->vel = vel;
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