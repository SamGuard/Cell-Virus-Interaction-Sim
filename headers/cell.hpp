#ifndef CELL
#define CELL

#include "agentbase.hpp"

enum CellState { Dead, Healthy, Infected, Empty, Bystander };

class Particle;

class Cell : public AgentBase {
   private:
    CellState state;
    double deathTick;

   public:
    CellState nextState;
    bool hasStateChanged;  // Only output setstate if the state has changed

    Cell() : AgentBase() {
        agentType = CellType;
        hasStateChanged = false;
        state = nextState = Dead;
        deathTick = -1;
        receptorType = REC_CELL;
    }
    Cell(repast::AgentId id, CellState state) : Cell() {
        this->state = this->nextState = state;
        this->id = id;
        hasStateChanged = false;
    }

    Cell(repast::AgentId id, CellState state, CellState nextState,
         bool hasStateChanged)
        : Cell(id, state) {
        this->nextState = nextState;
        this->hasStateChanged = hasStateChanged;
    }

    void set(repast::AgentId id, CellState state, CellState nextState,
             bool hasStateChanged, double deathTick, int receptorType,
             std::set<int> attFactors) {
        AgentBase::set(id, CellType, Vector(), 0, receptorType, attFactors);
        this->state = state;
        this->nextState = nextState;
        this->hasStateChanged = hasStateChanged;
    }

    CellState getState() { return state; }
    CellState getNextState() { return state; }

    void setState(CellState state) { this->state = this->nextState = state; }
    void setNextState(CellState state) {
        if (state == Dead) {
            setDeathTick(repast::RepastProcess::instance()
                             ->getScheduleRunner()
                             .currentTick());
        } else if (state == Empty) {
            setDeathTick(-1);
        }

        this->nextState = state;
        hasStateChanged = true;
    }
    void goNextState() { this->state = this->nextState; }

    double getDeathTick() { return deathTick; }
    void setDeathTick(double tick) { deathTick = tick; }

    void interact(
        repast::SharedContext<Cell>* cellContext,
        repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                    repast::SimpleAdder<Cell>>* cellSpace,
        repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                    repast::SimpleAdder<Particle>>*
            partDiscSpace,
        std::vector<std::tuple<repast::Point<double>, AgentType>>* add,
        std::set<Particle*>* remove, int &removeInfectedCellCount);
};

/* Serializable Agent Package */
struct CellPackage {
   public:
    int id, rank, type, currentRank, receptorType;
    CellState state, nextState;
    bool hasStateChanged;
    double deathTick;
    std::set<int> attFactors;

    /* Constructors */
    CellPackage(){};  // For serialization
    CellPackage(int _id, int _rank, int _type, int _currentRank,
                int _receptorType, CellState _state, CellState nextState,
                bool _hasStateChanged, double _deathTick,
                std::set<int> _attFactors);

    /* For archive packaging */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& rank;
        ar& type;
        ar& currentRank;
        ar& receptorType;
        ar& state;
        ar& nextState;
        ar& hasStateChanged;
        ar& deathTick;
        ar& attFactors;
    }
};

#endif