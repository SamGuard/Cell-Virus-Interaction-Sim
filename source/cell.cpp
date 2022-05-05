#include "cell.hpp"

#include "globals.hpp"
#include "particle.hpp"
#include "repast_hpc/Moore2DGridQuery.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/VN2DGridQuery.h"

void Cell::interact(
    repast::SharedContext<Cell>* cellContext,
    repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                repast::SimpleAdder<Cell>>* cellSpace,
    repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                repast::SimpleAdder<Particle>>* partDiscSpace,
    std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
    std::set<Particle*>* remove, int& removeInfectedCellCount) {
    hasStateChanged = false;
    repast::Random* rand = repast::Random::instance();
    double currentTick =
        repast::RepastProcess::instance()->getScheduleRunner().currentTick();

    std::vector<int> loc;
    cellSpace->getLocation(id, loc); // Get the cells location
    switch (getState()) {
        case Dead: {
            // Don't do anything until some time has passed
            if (getDeathTick() + CELL_DEATH_LENGTH < currentTick) {
                setNextState(Empty);
            }
            break;
        }
        case Empty: {
            // See if the cell should be come healthy
            repast::VN2DGridQuery<Cell> gridQ(cellSpace);

            repast::Point<int> queryCent(loc);

            std::vector<Cell*> agents;
            gridQ.query(queryCent, 1, false, agents);

            std::vector<Cell*>::iterator it = agents.begin();
            Cell* c;
            int healthyC = 0;
            while (it != agents.end()) {
                c = (*it);
                if (c->getState() == Healthy) {
                    healthyC++;
                }
                it++;
            }

            if (rand->nextDouble() < (double)healthyC * CELL_DIVIDE_PROB) {
                setNextState(Healthy);
            }
            break;
        }

        // Falls through to healthy, has a chance of dying but reduces chance of
        // being infected
        case Bystander: {
            double r = rand->nextDouble();
            if (r < CELL_BYSTANDER_DEATH_PROB) {
                setNextState(Dead);
                return;
            } else if (r <
                       CELL_BYSTANDER_DEATH_PROB + CELL_REVERT_BYSTANDER_PROB) {
                setNextState(Healthy);
                return;
            }
        }
        // FALL THROUGH
        // The cell will look at nearby viruses and have a chance of becoming
        // infected
        case Healthy: {
            std::vector<int> pLoc;  // location in the viruses coordinate system
            {
                repast::Point<int> p(loc[0], loc[1]);
                pLoc = spaceTrans.cellToPartDisc(p).coords();
            }

            // See if there is a nearby infected cell to infect this cell
            {
                std::vector<Cell*> cells;
                repast::VN2DGridQuery<Cell> gridQ(cellSpace);
                gridQ.query(loc, 1, false, cells);

                int infectedCellCount = 0;
                for (std::vector<Cell*>::iterator it = cells.begin();
                     it != cells.end(); it++) {
                    if ((*it)->getState() == Infected) {
                        infectedCellCount++;
                    }
                }

                if (infectedCellCount != 0 &&
                    rand->nextDouble() > pow(1.0 - CELL_INFECT_NEIGH_CELL_PROB,
                                             infectedCellCount)) {
                    setNextState(Infected);
                    return;
                }
            }

            // See if there are viruses and/or interferons in the nearby area
            {
                repast::Moore2DGridQuery<Particle> gridQ(partDiscSpace);

                std::vector<Particle*> agents;
                gridQ.query(pLoc, spaceTrans.cellSize() / 2.0, true, agents);

                int virusCount = 0;
                int ifnCount = 0;
                // Count the number of types of particles in the area
                for (std::vector<Particle*>::iterator it = agents.begin();
                     it != agents.end(); it++) {
                    if (!isLocal((*it)->getId())) continue;
                    switch ((*it)->getAgentType()) {
                        case VirusType:
                            virusCount++;
                            break;
                        case InterferonType:
                            ifnCount++;
                            break;
                        default:
                            break;
                    }
                }
                // See if cell becomes infected
                if ((getState() == Healthy ||
                     rand->nextDouble() > CELL_BYSTANDER_INFECT_SKIP_PROB) &&
                    rand->nextDouble() >
                        pow(1.0 - VIRUS_INFECT_PROB, virusCount)) {
                    // A virus cannot infect two cells at once so if the chosen
                    // virus is already in the set then try find another As well
                    // as checking if the receptors/attatchment factors match

                    for (std::vector<Particle*>::iterator it = agents.begin();
                         it != agents.end(); it++) {
                        // Check the particle returned is a virus
                        Particle* p = *it;
                        if (p->getAgentType() == VirusType &&
                            p->canAttach(receptorType) && isLocal(p->getId()) &&
                            remove->find(p) == remove->end()) {
                            remove->insert(p);
                            setNextState(Infected);
                            // std::vector<int> loc;
                            // partDiscSpace->getLocation(p->getId(), loc);
                            // std::cout << loc[0] << " " << loc[1] <<
                            // std::endl;
                            break;
                        }
                    }
                }
                
                // See if cell becomes bystander
                if (rand->nextDouble() >
                    pow(1.0 - CELL_TO_BYSTANDER_PROB, ifnCount)) {

                    for (std::vector<Particle*>::iterator it = agents.begin();
                         it != agents.end(); it++) {
                        // Check the particle returned is a virus
                        Particle* p = *it;
                        if (p->getAgentType() == InterferonType &&
                            isLocal(p->getId()) &&
                            remove->find(p) == remove->end()) {
                            remove->insert(p);
                            setNextState(Bystander);
                            break;
                        }
                    }
                }
            }
            break;
        }

        case Infected: {
            // See if infected cell is destoryed by a nearby innate immune cell
            {
                std::vector<int>
                    pLoc;  // location in the particle coordinate system
                {
                    repast::Point<int> p(loc[0], loc[1]);
                    pLoc = spaceTrans.cellToPartDisc(p).coords();
                }

                repast::Moore2DGridQuery<Particle> gridQ(partDiscSpace);

                std::vector<Particle*> agents;
                gridQ.query(pLoc, spaceTrans.cellSize() / 2.0, true, agents);

                int innateCount = 0;
                // Count innate cells in area
                for (std::vector<Particle*>::iterator it = agents.begin();
                     it != agents.end(); it++) {
                    if ((*it)->getAgentType() == InnateImmuneType) {
                        innateCount++;
                    }
                }

                // See if this cell is killed by an innate immune cell
                if (rand->nextDouble() >
                    pow(1.0 - INNATE_KILL_CELL_PROB, innateCount)) {
                    setNextState(Empty);
                    removeInfectedCellCount++;
                    return;
                }
            }

            {
                // See if the cell dies and releases viruses
                if (rand->nextDouble() < CELL_PROB_TO_SPAWN_VIRUS) {
                    for (int i = 0; i < CELL_VIRUS_SPAWN_NUMBER; i++) {
                        add->push_back(
                            std::tuple<repast::Point<double>, AgentType>(
                                spaceTrans.cellToPart(loc), VirusType));
                    }
                    setNextState(Dead);
                    return;
                }
                for (int i = 0; i < CELL_IFN_SPAWN_NUMBER; i++) {
                    add->push_back(std::tuple<repast::Point<double>, AgentType>(
                        spaceTrans.cellToPart(loc), InterferonType));
                }
            }
            break;
        }
    }
}

CellPackage::CellPackage(int _id, int _rank, int _type, int _currentRank,
                         int _receptorType, CellState _state,
                         CellState _nextState, bool _hasStateChanged,
                         double _deathTick, std::set<int> _attFactors)
    : id(_id),
      rank(_rank),
      type(_type),
      currentRank(_currentRank),
      receptorType(_receptorType),
      state(_state),
      nextState(_nextState),
      hasStateChanged(_hasStateChanged),
      deathTick(_deathTick),
      attFactors(_attFactors) {}