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
    Virus() : AgentBase() { agentType = VirusType; }

    Virus(repast::AgentId id, Vector vel) : AgentBase(id, vel) {
        this->agentType = VirusType;
    }

    Virus(repast::AgentId id, Vector vel, double birthTick) : Virus(id, vel) {
        this->agentType = VirusType;
        this->birthTick = birthTick;
    }

    // Getter for agent specific attributes
    inline Vector getVel() { return vel; }
    inline double getBirthTick() { return birthTick; }

    // This is where interactions that change the state of agents take place
    void interact(repast::SharedContext<Virus>* context,
                  repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                              repast::SimpleAdder<Virus>>*
                      virusDiscreteSpace,
                  repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                                repast::SimpleAdder<Virus>>*
                      virusContinSpace,
                  bool& isAlive);
    // Moves the agent
    void move(repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                          repast::SimpleAdder<Virus>>*
                  virusDiscreteSpace,
              repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                            repast::SimpleAdder<Virus>>*
                  virusContinSpace);
};

/* Serializable Agent Package */
struct VirusPackage {
   public:
    int id, rank, type, currentRank, receptorType;
    double velx, vely, birthTick;
    std::vector<int> attFactors;

    /* Constructors */
    VirusPackage();  // For serialization
    VirusPackage(int _id, int _rank, int _type, int _currentRank,
                 int receptorType, double _velx, double _vely,
                 double _birthTick, std::vector<int> _attFactors);

    /* For archive packaging */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& rank;
        ar& type;
        ar& currentRank;
        ar& receptorType;
        ar& velx;
        ar& vely;
        ar& birthTick;
        ar& attFactors;
    }
};

#endif