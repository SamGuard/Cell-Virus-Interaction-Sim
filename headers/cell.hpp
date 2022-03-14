#ifndef CELL
#define CELL

#include "agentbase.hpp"
#include "repast_hpc/VN2DGridQuery.h"
#include "virus.hpp"

enum CellState { Dead, Healthy, Infected, Empty };

class Virus;

class Cell : public AgentBase {
    CellState state;
    CellState nextState;

   public:
    bool hasStateChanged;  // Only output setstate if the state has changed

    Cell() : AgentBase() {
        agentType = CellType;
        hasStateChanged = true;
        this->state = this->nextState = Dead;
    }
    Cell(repast::AgentId id, CellState state) : Cell() {
        this->state = this->nextState = state;
        this->id = id;
        hasStateChanged = false;
    }

    void set(repast::AgentId id, CellState state, CellState nextState,
             bool hasStateChanged) {
        this->id = id;
        this->state = state;
        this->nextState = nextState;
        this->hasStateChanged = hasStateChanged;
    }

    CellState getState() { return state; }
    CellState getNextState() { return state; }

    void setState(CellState state) { this->state = this->nextState = state; }
    void setNextState(CellState state) {
        this->nextState = state;
        hasStateChanged = true;
    }
    void goNextState() { this->state = this->nextState; }

    void interact(
        repast::SharedContext<Cell>* cellContext,
        repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                    repast::SimpleAdder<Cell>>* cellSpace,
        repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                    repast::SimpleAdder<Virus>>* virusDiscSpace,
        std::vector<repast::Point<double>> *out);
};

/* Serializable Agent Package */
struct CellPackage {
   public:
    int id, rank, type, currentRank;
    CellState state, nextState;
    bool hasStateChanged;

    /* Constructors */
    CellPackage(){};  // For serialization
    CellPackage(int _id, int _rank, int _type, int _currentRank,
                CellState _state, CellState nextState, bool _hasStateChanged);

    /* For archive packaging */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& rank;
        ar& type;
        ar& currentRank;
        ar& state;
        ar& nextState;
        ar& hasStateChanged;
    }
};

#endif