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

    AgentBase() {
        vel.x = 0;
        vel.y = 0;
    }

    AgentBase(repast::AgentId id, Vector vel) {
        this->id = id;
        this->vel = vel;
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

/* Serializable Agent Package */
struct AgentBasePackage {
   public:
    int id, rank, type, currentRank;
    double velx, vely;

    /* Constructors */
    AgentBasePackage();  // For serialization
    AgentBasePackage(int _id, int _rank, int _type, int _currentRank,
                     double _velx, double _vely);

    /* For archive packaging */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& rank;
        ar& type;
        ar& currentRank;
        ar& velx;
        ar& vely;
    }
};

#endif