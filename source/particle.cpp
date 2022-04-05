#include "particle.hpp"

#include "cell.hpp"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/Moore2DGridQuery.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/SharedDiscreteSpace.h"

void Particle::interact(
    repast::SharedContext<Particle>* context,
    repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                repast::SimpleAdder<Particle>>*
        partDiscreteSpace,
    repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                  repast::SimpleAdder<Particle>>*
        partContinSpace,
    std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
    std::set<Particle*>* remove) {
    // Inter-virus interaction, not in use (yet)
    /*
    std::vector<Virus*> agentsToPlay;
    std::vector<int> agentLocDiscrete;
    virusDiscreteSpace->getLocation(id, agentLocDiscrete);
    repast::Point<int> center(agentLocDiscrete);
    repast::Moore2DGridQuery<Virus> moore2DQuery(virusDiscreteSpace);
    moore2DQuery.query(center, 0, true, agentsToPlay);

    std::vector<Virus*>::iterator agentToPlay = agentsToPlay.begin();
    while (agentToPlay != agentsToPlay.end()) {}
    */
    // birthTick + lifetime
    if (birthTick + VIRUS_LIFETIME < repast::RepastProcess::instance()
                                         ->getScheduleRunner()
                                         .currentTick() &&
        remove->find(this) == remove->end()) {
        remove->insert(this);
    }
}

void Particle::move(
    repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                repast::SimpleAdder<Particle>>* discreteSpace,
    repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                  repast::SimpleAdder<Particle>>* continSpace) {
    std::vector<double> loc;
    continSpace->getLocation(id, loc);
    loc[0] += vel.x;
    loc[1] += vel.y;

    repast::Point<double> origin = continSpace->bounds().origin();
    repast::Point<double> extent = continSpace->bounds().extents();

    if (loc[0] <= origin.getX() || loc[0] >= extent.getX() + origin.getX()) {
        vel.x = -vel.x;
        loc[0] += vel.x;
    }
    if (loc[1] <= origin.getY() || loc[1] >= extent.getY() + origin.getY()) {
        vel.y = -vel.y;
        loc[1] += vel.y;
    }

    std::vector<int> discLoc;
    discLoc.push_back((int)loc[0]);
    discLoc.push_back((int)loc[1]);

    continSpace->moveTo(id, loc);
    discreteSpace->moveTo(id, discLoc);
}

void InnateImmune::interact(
    repast::SharedContext<Particle>* context,
    repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                repast::SimpleAdder<Particle>>*
        partDiscreteSpace,
    repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                  repast::SimpleAdder<Particle>>*
        partContinSpace,
    std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
    std::set<Particle*>* remove) {
    repast::Random* rand = repast::Random::instance();
    if (birthTick + INNATE_LIFETIME < repast::RepastProcess::instance()
                                          ->getScheduleRunner()
                                          .currentTick() &&
        remove->find(this) == remove->end() && isLocal(this->getId())) {
        remove->insert(this);
        return;
    }

    std::vector<double> loc;
    partContinSpace->getLocation(getId(), loc);

    std::vector<Particle*> agents;

    repast::Moore2DGridQuery<Particle> query(partDiscreteSpace);
    query.query(repast::Point<int>((int)loc[0], (int)loc[1]), INNATE_RANGE,
                true, agents);

    for (std::vector<Particle*>::iterator it = agents.begin();
         it != agents.end(); it++) {
        Particle* a = *it;
        if (a->getAgentType() == VirusType && isLocal(a->getId()) &&
            remove->find(a) == remove->end() &&
            rand->nextDouble() < INNATE_KILL_VIRUS_PROB) {
            if (rand->nextDouble() < INNATE_RECRUIT_PROB) {
                add->push_back(std::tuple<repast::Point<double>, AgentType>(
                    repast::Point<double>(-1, -1), InnateImmuneType));
            }
            remove->insert(a);
            return;
        }
    }
}

void Antibody::interact(
    repast::SharedContext<Particle>* context,
    repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                repast::SimpleAdder<Particle>>*
        partDiscreteSpace,
    repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                  repast::SimpleAdder<Particle>>*
        partContinSpace,
    std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
    std::set<Particle*>* remove) {
    if (birthTick + ANTIBODY_LIFETIME <
        repast::RepastProcess::instance()->getScheduleRunner().currentTick()) {
        if (remove->find(this) == remove->end()) {
            remove->insert(this);
        }
        return;
    }

    std::vector<double> loc;
    partContinSpace->getLocation(getId(), loc);

    std::vector<Particle*> agents;
    repast::Moore2DGridQuery<Particle> query(partDiscreteSpace);
    query.query(repast::Point<int>(loc[0], loc[1]), ANTIBODY_RANGE, true,
                agents);
    // See if theres a virus to "kill" nearby
    for (std::vector<Particle*>::iterator it = agents.begin();
         it != agents.end(); it++) {
        Particle* a = *it;
        if (a->getAgentType() == VirusType && isLocal(a->getId()) &&
            repast::Random::instance()->nextDouble() < ANTIBODY_KILL_PROB &&
            remove->find(a) == remove->end()) {
            // Remove the virus and itselfF
            remove->insert(a);
            if (remove->find(this) == remove->end()) {
                remove->insert(this);
            }
            return;
        }
    }
}

/* Serializable Agent Package Data */

ParticlePackage::ParticlePackage() {}

ParticlePackage::ParticlePackage(int _id, int _rank, int _type,
                                 int _currentRank, int _receptorType,
                                 double _velx, double _vely, double _birthTick,
                                 std::set<int> _attFactors)
    : id(_id),
      rank(_rank),
      type(_type),
      currentRank(_currentRank),
      receptorType(_receptorType),
      velx(_velx),
      vely(_vely),
      birthTick(_birthTick),
      attFactors(_attFactors) {}