#ifndef AGENT
#define AGENT

#include <stdlib.h>

#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/SharedDiscreteSpace.h"

#include "constants.hpp"

class Agent {
    Vector vel;
    repast::AgentId id;
    unsigned int testCounter;

   public:
    Agent(){
      vel.x = 0;
      vel.y = 0;
    }

    // Getters for the serialisation
    virtual repast::AgentId& getId(){                   return id;    }
    virtual const repast::AgentId& getId() const {      return id;    }

    // Getter for agent specific attributes
    Vector getVel(){ return vel; }

    Agent(repast::AgentId id, Vector vel){
      this->id = id;
      this->vel = vel;
      testCounter = 0;
    }

    void set(repast::AgentId id, Vector vel){
      this->id = id;
      this->vel = vel;
    }
    // This is where interactions that change the state of agents take place
    void interact(
        repast::SharedContext<Agent>* context,
        repast::SharedDiscreteSpace<Agent, repast::StrictBorders,
                                    repast::SimpleAdder<Agent>>* discreteSpace,
        repast::SharedContinuousSpace<Agent, repast::StrictBorders,
                                      repast::SimpleAdder<Agent>>* continSpace);
    // Moves the agent                                      
    void move(
        repast::SharedDiscreteSpace<Agent, repast::StrictBorders,
                                    repast::SimpleAdder<Agent>>* discreteSpace,
        repast::SharedContinuousSpace<Agent, repast::StrictBorders,
                                      repast::SimpleAdder<Agent>>* continSpace);
};

#endif