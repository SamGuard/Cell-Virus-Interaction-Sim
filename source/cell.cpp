#include "cell.hpp"

#include "virus.hpp"

void Cell::interact(
    repast::SharedContext<Cell>* cellContext,
    repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                repast::SimpleAdder<Cell>>* cellSpace,
    repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                repast::SimpleAdder<Virus>>* virusDiscSpace,
    std::vector<repast::Point<double>>* add, std::set<Virus*>* remove) {
    hasStateChanged = false;
    repast::Random* rand = repast::Random::instance();
    double currentTick =
        repast::RepastProcess::instance()->getScheduleRunner().currentTick() /
        6;

    if (getState() == Dead) {
        if (getDeathTick() + 200 < currentTick) {
            setNextState(Empty);
        }
        return;
    }

    std::vector<int> loc;
    cellSpace->getLocation(id, loc);

    if (getState() == Empty || getState() == Healthy) {
        repast::VN2DGridQuery<Cell> gridQ(cellSpace);

        repast::Point<int> queryCent(loc);

        std::vector<Cell*> agents;
        gridQ.query(queryCent, 1, false, agents);

        if (getState() == Healthy &&
            rand->nextDouble() < cellDeathChanceOvercrowding) {
            setNextState(Dead);
            return;
        }

        if (getState() == Empty) {
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
        }
    }

    if (getState() == Healthy) {
        std::vector<int> vLoc;  // location in the viruses coordinate system
        {
            repast::Point<int> p(loc[0], loc[1]);
            vLoc = spaceTrans.cellToVirDisc(p).coords();
        }

        repast::VN2DGridQuery<Virus> gridQ(virusDiscSpace);

        std::vector<Virus*> agents;
        gridQ.query(vLoc, spaceTrans.cellSize() / 2.01, true, agents);

        if (rand->nextDouble() < 1.0 - pow(0.80, agents.size())) {
            // A virus cannot infect two cells at once so if the chosen virus is
            // already in the set then try find another
            // As well as checking if the receptors/attatchment factors match
            bool canFindVirus = false;
            std::vector<Virus*>::iterator it = agents.begin();
            while (it != agents.end()) {
                if ((*it)->canAttach(receptorType) &&
                    remove->find(*it) == remove->end()) {
                    canFindVirus = true;
                    remove->insert(*it);
                    break;
                }
                it++;
            }

            if (canFindVirus) {
                setNextState(Infected);
            }
        }
    }

    if (getState() == Infected) {
        if (rand->nextDouble() < 0.1) {
            add->push_back(spaceTrans.cellToVir(loc));
            add->push_back(spaceTrans.cellToVir(loc));
            setNextState(Dead);
            return;
        }
    }
}

CellPackage::CellPackage(int _id, int _rank, int _type, int _currentRank,
                         int _receptorType, CellState _state,
                         CellState _nextState, bool _hasStateChanged,
                         double _deathTick, std::vector<int> _attFactors)
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