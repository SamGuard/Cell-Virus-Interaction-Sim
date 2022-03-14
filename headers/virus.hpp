#ifndef VIRUS
#define VIRUS

#include <stdlib.h>

#include "agentbase.hpp"
#include "cell.hpp"
#include "constants.hpp"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/SharedDiscreteSpace.h"

class Cell;

class Virus : public AgentBase {
   public:
    unsigned int testCounter;

    Virus() : AgentBase() { agentType = VirusType; }

    Virus(repast::AgentId id, Vector vel, int testCounter) : AgentBase(id, vel) {
        this->testCounter = testCounter;
        this->agentType = VirusType;
    }

    // Getter for agent specific attributes
    Vector getVel() { return vel; }
    int getTestCounter() { return testCounter; }


    void set(repast::AgentId id, Vector vel, int testCounter) {
        this->id = id;
        this->vel = vel;
        this->testCounter = testCounter;
    }

    // This is where interactions that change the state of agents take place
    void interact(
        repast::SharedContext<Virus>* context,
        repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                    repast::SimpleAdder<Virus>>* virusDiscreteSpace,
        repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                      repast::SimpleAdder<Virus>>* virusContinSpace);
    // Moves the agent
    void move(
        repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                    repast::SimpleAdder<Virus>>* virusDiscreteSpace,
        repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                      repast::SimpleAdder<Virus>>* virusContinSpace);

    void increment() { testCounter++; }
};

/* Serializable Agent Package */
struct VirusPackage {
   public:
    int id, rank, type, currentRank, testCounter;
    double velx, vely;

    /* Constructors */
    VirusPackage();  // For serialization
    VirusPackage(int _id, int _rank, int _type, int _currentRank, double _velx,
                 double _vely, int _testCounter);

    /* For archive packaging */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& rank;
        ar& type;
        ar& currentRank;
        ar& velx;
        ar& vely;
        ar& testCounter;
    }
};

#endif