#include "cell.hpp"

#include "particle.hpp"

void Cell::interact(
    repast::SharedContext<Cell>* cellContext,
    repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                repast::SimpleAdder<Cell>>* cellSpace,
    repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                repast::SimpleAdder<Particle>>* partDiscSpace,
    std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
    std::set<Particle*>* remove) {
    hasStateChanged = false;
    repast::Random* rand = repast::Random::instance();
    double currentTick =
        repast::RepastProcess::instance()->getScheduleRunner().currentTick();

    std::vector<int> loc;
    cellSpace->getLocation(id, loc);
    switch (getState()) {
        case Dead: {
            if (getDeathTick() + 200 < currentTick) {
                setNextState(Empty);
            }
            break;
        }
        case Empty: {
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

            if (rand->nextDouble() < (double)healthyC * 0.125) {
                setNextState(Healthy);
            }
            break;
        }

        // Falls through to healthy, has a chance of dying but reduces chance of
        // being infected
        case Bystander: {
            double r = rand->nextDouble();
            if (r < 0.001) {
                setNextState(Dead);
                return;
            }else if (r < 0.001 + 0.01) {
                setNextState(Healthy);
                return;
            }
            if (rand->nextDouble() > 0.9) {
                return;
            }
        }
        // The cell will look at nearby viruses and have a chance of becoming
        // infected
        case Healthy: {
            std::vector<int> pLoc;  // location in the viruses coordinate system
            {
                repast::Point<int> p(loc[0], loc[1]);
                pLoc = spaceTrans.cellToPartDisc(p).coords();
            }

            repast::VN2DGridQuery<Particle> gridQ(partDiscSpace);

            std::vector<Particle*> agents;
            gridQ.query(pLoc, spaceTrans.cellSize() / 2.01, true, agents);

            int virusCount = 0;
            int ifnCount = 0;

            for (std::vector<Particle*>::iterator it = agents.begin();
                 it != agents.end(); it++) {
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

            if (rand->nextDouble() < 1.0 - pow(0.80, virusCount)) {
                // A virus cannot infect two cells at once so if the chosen
                // virus is already in the set then try find another As well
                // as checking if the receptors/attatchment factors match
                bool canFind = false;

                for (std::vector<Particle*>::iterator it = agents.begin();
                     it != agents.end(); it++) {
                    // Check the particle returned is a virus
                    Particle* p = *it;
                    if (p->getAgentType() == VirusType &&
                        p->canAttach(receptorType) && isLocal(p->getId()) &&
                        remove->find(p) == remove->end()) {
                        canFind = true;
                        remove->insert(p);
                        break;
                    }
                }
                if (canFind) {
                    setNextState(Infected);
                }
            }
            if (rand->nextDouble() < 1.0 - pow(0.50, ifnCount)) {
                bool canFind = false;

                for (std::vector<Particle*>::iterator it = agents.begin();
                     it != agents.end(); it++) {
                    // Check the particle returned is a virus
                    Particle* p = *it;
                    if (p->getAgentType() == InterferonType &&
                        isLocal(p->getId()) &&
                        remove->find(p) == remove->end()) {
                        canFind = true;
                        remove->insert(p);
                        break;
                    }
                }
                if (canFind) {
                    setNextState(Bystander);
                }
            }
            break;
        }

        case Infected: {
            if (rand->nextDouble() < 0.1) {
                for (int i = 0; i < 2; i++) {
                    add->push_back(std::tuple<repast::Point<double>, AgentType>(
                        spaceTrans.cellToPart(loc), VirusType));
                }
                setNextState(Dead);
                return;
            }
            for (int i = 0; i < 2; i++) {
                add->push_back(std::tuple<repast::Point<double>, AgentType>(
                    spaceTrans.cellToPart(loc), InterferonType));
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