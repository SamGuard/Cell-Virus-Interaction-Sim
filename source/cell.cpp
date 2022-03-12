#include "cell.hpp"

void Cell::interact(
    repast::SharedContext<Cell>* context,
    repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                repast::SimpleAdder<Cell>>* space) {
    // Cell cannot do anything if dead or empty
    if (getState() == Empty && getState() == Infected) {
        return;
    }

    repast::Random *rand = repast::Random::instance();

    std::vector<int> loc;
    space->getLocation(id, loc);

    repast::VN2DGridQuery<Cell> gridQ(space);

    repast::Point<int> queryCent(loc);

    std::vector<Cell*> agents;
    gridQ.query(queryCent, 1, false, agents);

    // Cell has small chance of dying from overcrowding
    if (getState() == Healthy && rand->nextDouble() >
        pow(1.0 - CELL_CHANCE_DEATH_OVERCROWD, (double)agents.size())) {
        setNextState(Dead);
        return;
    }

    std::vector<Cell*>::iterator it = agents.begin();
    Cell* c;
    int healthyC = 0;
    while (it != agents.end()) {
        c = (*it);
        if (c->getState() == Healthy){
            healthyC++;
        }
        it++;
    }

    if(rand->nextDouble() < healthyC * 0.25){
        setNextState(Healthy);
    }
}

CellPackage::CellPackage(int _id, int _rank, int _type, int _currentRank,
                         CellState _state)
    : id(_id),
      rank(_rank),
      type(_type),
      currentRank(_currentRank),
      state(_state) {}