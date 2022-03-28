#ifndef PARTICLE
#define PARTICLE

#include <stdlib.h>

#include "agentbase.hpp"
#include "cell.hpp"
#include "constants.hpp"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/Moore2DGridQuery.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/SharedDiscreteSpace.h"

class Cell;

class Particle : public AgentBase {
   protected:
    Particle() : AgentBase() {}
    Particle(repast::AgentId id, Vector vel) : AgentBase(id, vel) {}

    Particle(repast::AgentId id, AgentType agentType, Vector vel,
             double birthTick)
        : Particle(id, vel) {
        this->birthTick = birthTick;
        this->agentType = agentType;
    }

   public:
    // Getter for agent specific attributes
    inline Vector getVel() { return vel; }
    inline double getBirthTick() { return birthTick; }

    // This is where interactions that change the state of agents take place
    void interact(
        repast::SharedContext<Particle>* context,
        repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                    repast::SimpleAdder<Particle>>*
            partDiscreteSpace,
        repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                      repast::SimpleAdder<Particle>>*
            partContinSpace,
        std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
        std::set<Particle*>* remove);
    // Moves the agent
    void move(repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                          repast::SimpleAdder<Particle>>*
                  partDiscreteSpace,
              repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                            repast::SimpleAdder<Particle>>*
                  partContinSpace);
};

class Virus : public Particle {
   public:
    Virus(repast::AgentId id, Vector vel, double birthTick)
        : Particle(id, VirusType, vel, birthTick) {}
};

class Interferon : public Particle {
   public:
    Interferon(repast::AgentId id, Vector vel, double birthTick)
        : Particle(id, InterferonType, vel, birthTick) {}
};

class InnateImmune : public Particle {
   public:
    InnateImmune(repast::AgentId id, Vector vel, double birthTick)
        : Particle(id, InnateImmuneType, vel, birthTick) {}
    InnateImmune(Particle p)
        : Particle(p.getId(), InnateImmuneType, p.getVel(), p.getBirthTick()) {}

    void interact(
        repast::SharedContext<Particle>* context,
        repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                    repast::SimpleAdder<Particle>>*
            partDiscreteSpace,
        repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                      repast::SimpleAdder<Particle>>*
            partContinSpace,
        std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
        std::set<Particle*>* remove);
};

class Antigen : public Particle {
   public:
    Antigen(repast::AgentId id, Vector vel, double birthTick)
        : Particle(id, AntigenType, vel, birthTick) {}
    
    void interact(
        repast::SharedContext<Particle>* context,
        repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                    repast::SimpleAdder<Particle>>*
            partDiscreteSpace,
        repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                      repast::SimpleAdder<Particle>>*
            partContinSpace,
        std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
        std::set<Particle*>* remove);
};

/* Serializable Agent Package */
struct ParticlePackage {
   public:
    int id, rank, type, currentRank, receptorType;
    double velx, vely, birthTick;
    std::set<int> attFactors;

    /* Constructors */
    ParticlePackage();  // For serialization
    ParticlePackage(int _id, int _rank, int _type, int _currentRank,
                    int _receptorType, double _velx, double _vely,
                    double _birthTick, std::set<int> _attFactors);

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