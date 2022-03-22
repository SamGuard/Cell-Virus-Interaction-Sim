#ifndef CELL
#define CELL

#include "agentbase.hpp"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/VN2DGridQuery.h"
#include "virus.hpp"

enum CellState { Dead, Healthy, Infected, Empty };

class Virus;

class Cell : public AgentBase {
   private:
    CellState state;
    int receptorType;
    double deathTick;

   public:
    CellState nextState;
    bool hasStateChanged;  // Only output setstate if the state has changed

    Cell() : AgentBase() {
        agentType = CellType;
        hasStateChanged = false;
        state = nextState = Dead;
        deathTick = -1;
        receptorType = 0;
    }
    Cell(repast::AgentId id, CellState state) : Cell() {
        this->state = this->nextState = state;
        this->id = id;
        hasStateChanged = false;
    }

    Cell(repast::AgentId id, CellState state, CellState nextState,
         bool hasStateChanged, int receptorType)
        : Cell(id, state) {
        this->nextState = nextState;
        this->hasStateChanged = hasStateChanged;
        this->receptorType = receptorType;
    }

    void set(repast::AgentId id, CellState state, CellState nextState,
             bool hasStateChanged, double deathTick, int receptorType) {
        this->id = id;
        this->state = state;
        this->nextState = nextState;
        this->hasStateChanged = hasStateChanged;
        this->deathTick = deathTick;
        this->receptorType = receptorType;
    }

    CellState getState() { return state; }
    CellState getNextState() { return state; }
    int getReceptorType() { return receptorType; }

    void setState(CellState state) { this->state = this->nextState = state; }
    void setNextState(CellState state) {
        if (state == Dead) {
            setDeathTick(repast::RepastProcess::instance()
                             ->getScheduleRunner()
                             .currentTick() /
                         tickCycleLen);
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
        repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                    repast::SimpleAdder<Virus>>* virusDiscSpace,
        std::vector<repast::Point<double>>* add, std::set<Virus*>* remove);
};

/* Serializable Agent Package */
struct CellPackage {
   public:
    int id, rank, type, currentRank, receptorType;
    CellState state, nextState;
    bool hasStateChanged;
    double deathTick;

    /* Constructors */
    CellPackage(){};  // For serialization
    CellPackage(int _id, int _rank, int _type, int _currentRank,
                int _receptorType, CellState _state, CellState nextState,
                bool _hasStateChanged, double _deathTick);

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
    }
};

#endif