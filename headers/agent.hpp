#ifndef AGENT
#define AGENT

#include <stdlib.h>

#include "constants.hpp"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/SharedDiscreteSpace.h"

class Agent {
    Vector vel;
    repast::AgentId id;
    unsigned int testCounter;

   public:
    Agent() {
        vel.x = 0;
        vel.y = 0;
    }

    // Getters for the serialisation
    virtual repast::AgentId& getId() { return id; }
    virtual const repast::AgentId& getId() const { return id; }

    // Getter for agent specific attributes
    Vector getVel() { return vel; }

    Agent(repast::AgentId id, Vector vel) {
        this->id = id;
        this->vel = vel;
        testCounter = 1;
    }

    void set(repast::AgentId id, Vector vel) {
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

/* Serializable Agent Package */
struct AgentPackage {
   public:
    int id;
    int rank;
    int type;
    int currentRank;
    Vector vel;

    /* Constructors */
    AgentPackage();  // For serialization
    AgentPackage(int _id, int _rank, int _type, int _currentRank, Vector vel);

    /* For archive packaging */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& rank;
        ar& type;
        ar& currentRank;
        ar& vel;
    }
};

#endif