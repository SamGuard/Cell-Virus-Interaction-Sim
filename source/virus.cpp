#include "virus.hpp"

#include "cell.hpp"
#include "repast_hpc/Moore2DGridQuery.h"
#include "repast_hpc/Point.h"

void Virus::interact(
    repast::SharedContext<Virus>* context,
    repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                repast::SimpleAdder<Virus>>* virusDiscreteSpace,
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus>>*
        virusContinSpace, bool &isAlive) {

    // Inter-virus interaction, not in use yet
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
   isAlive = true;
   if(birthTick + 100 * 6 < repast::RepastProcess::instance()->getScheduleRunner().currentTick()){
       isAlive = false;
   }
}

void Virus::move(
    repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                repast::SimpleAdder<Virus>>* discreteSpace,
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus>>* continSpace) {
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

/* Serializable Agent Package Data */

VirusPackage::VirusPackage() {}

VirusPackage::VirusPackage(int _id, int _rank, int _type, int _currentRank,
                           double _velx, double _vely, double _birthTick)
    : id(_id),
      rank(_rank),
      type(_type),
      currentRank(_currentRank),
      velx(_velx),
      vely(_vely),
      birthTick(_birthTick) {}