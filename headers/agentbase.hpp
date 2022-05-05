#ifndef AGENTBASE
#define AGENTBASE

#include <stdlib.h>

#include "globals.hpp"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/SharedDiscreteSpace.h"

class AgentBase {
   protected:
    double birthTick;
    std::set<int> attFactors;  // Attatchment factors
    int receptorType;
    AgentType agentType;

   public:
    Vector vel;
    repast::AgentId id;

    AgentBase() {
        vel.x = 0;
        vel.y = 0;
        agentType = BaseAgentType;
        receptorType = 0;
    }

    AgentBase(repast::AgentId id, Vector vel) {
        this->id = id;
        this->vel = vel;
        agentType = BaseAgentType;
        birthTick = 0;
        receptorType = 0;
    }

    virtual ~AgentBase() {}

    // Getters for the serialisation
    virtual repast::AgentId& getId() { return id; }
    virtual const repast::AgentId& getId() const { return id; }

    // Getter for agent specific attributes
    Vector getVel() { return vel; }
    AgentType getAgentType() { return agentType; }

    void setAgentType(AgentType agentType) { this->agentType = agentType; }

    // Used for updating agents across processes
    void set(repast::AgentId id, AgentType type, Vector vel, int birthTick,
             int receptorType, std::set<int> attFactors) {
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
    inline std::set<int> getAttFactors() { return attFactors; }

    void addAttachFactor(int x) { attFactors.insert(x); }
    void removeAttachFactor(int x) { attFactors.erase(x); }
    bool canAttach(int x) { return attFactors.find(x) != attFactors.end(); }

    // This is where interactions that change the state of agents take place
    virtual void interact(
        repast::SharedContext<AgentBase>* context,
        repast::SharedDiscreteSpace<AgentBase, repast::StrictBorders,
                                    repast::SimpleAdder<AgentBase>>*
            partDiscreteSpace,
        repast::SharedContinuousSpace<AgentBase, repast::StrictBorders,
                                      repast::SimpleAdder<AgentBase>>*
            partContinSpace){};
    // Moves the agent
    virtual void move(
        repast::SharedDiscreteSpace<AgentBase, repast::StrictBorders,
                                    repast::SimpleAdder<AgentBase>>*
            partDiscreteSpace,
        repast::SharedContinuousSpace<AgentBase, repast::StrictBorders,
                                      repast::SimpleAdder<AgentBase>>*
            partContinSpace){};
};

#endif