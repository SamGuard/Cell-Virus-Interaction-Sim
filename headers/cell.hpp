#ifndef CELL
#define CELL

#include "agentbase.hpp"
#include "repast_hpc/VN2DGridQuery.h"

enum CellState { Dead, Healthy, Infected, Empty };

class Cell : public AgentBase {
    CellState state;
    CellState nextState;

   public:
    bool hasStateChanged;  // Only output setstate if the state has changed

    Cell() : AgentBase() {
        agentType = CellType;
        hasStateChanged = true;
    }
    Cell(repast::AgentId id, CellState state) : AgentBase() {
        agentType = CellType;
        this->state = this->nextState = state;
        this->id = id;
        hasStateChanged = true;
    }

    void set(repast::AgentId id, CellState state, bool hasStateChanged) {
        this->id = id;
        this->state = state;
        this->hasStateChanged = true;
    }

    CellState getState() { return state; }

    void setState(CellState state) { this->state = this->nextState = state; }
    void setNextState(CellState state) {
        this->nextState = state;
        hasStateChanged = true;
    }
    void goNextState() { this->state = this->nextState; }

    void interact(
        repast::SharedContext<Cell>* context,
        repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                    repast::SimpleAdder<Cell>>* space);
};

/* Serializable Agent Package */
struct CellPackage {
   public:
    int id, rank, type, currentRank;
    bool hasStateChanged;
    CellState state;

    /* Constructors */
    CellPackage(){};  // For serialization
    CellPackage(int _id, int _rank, int _type, int _currentRank,
                CellState _state, bool _hasStateChanged);

    /* For archive packaging */
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar& id;
        ar& rank;
        ar& type;
        ar& currentRank;
        ar& state;
        ar& hasStateChanged;
    }
};

#endif