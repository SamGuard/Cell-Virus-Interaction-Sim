#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <stdlib.h>

#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/RepastProcess.h"

enum AgentType {
    BaseAgentType,
    VirusType,
    CellType,
    InterferonType,
    InnateImmuneType,
    AntibodyType
};

// Variables needed everywhere
extern unsigned long int particleIdCount;

// Parameters
// Cell
extern double CELL_DEATH_LENGTH;  // How long a dead cell will stay dead before
                                  // it is cleared
extern double CELL_DIVIDE_PROB;   // Probability that an empty cell will become
                                  // healthy multiplied by the amount of healthy
                                  // cells surrounding it
extern double
    CELL_BYSTANDER_DEATH_PROB;  // Probability a bystander cell will die
extern double CELL_REVERT_BYSTANDER_PROB;  // Probability that bystander will
                                           // revert to healthy cell
extern double
    CELL_BYSTANDER_INFECT_SKIP_PROB;  // Probability that the infection step
                                      // will be skipped if the cell is a
                                      // bystander
extern double CELL_INFECT_POW_BASE;  // Base for the prob calculation that there
                                     // is a chance that a cell can be infected
                                     // by one of the nearby viruses
extern double CELL_PROB_TO_BYSTANDER_BASE;  // For each IFN particle around the
                                            // cell this value will be raised to
                                            // the power of that value
extern int CELL_IFN_SPAWN_NUMBER;  // The amount of IFNs to add each tick from a
                                   // bystander cell
extern double
    CELL_CHANCE_TO_SPAWN_VIRUS;  // The chance each tick for a infected cell to
                                 // die and release virons

// Virus
extern double VIRUS_LIFETIME;

// Innate
extern double INNATE_LIFETIME;

// IFN
extern double IFN_LIFETIME;

// Antibody
extern double ANTIBODY_LIFETIME;

// Receptor/Attatchment Types
#define REC_VIRUS 1
#define REC_CELL 2

struct Vector {
    double x, y;
    Vector() : x(0), y(0) {}
};

inline bool isLocal(repast::AgentId id) {
    return id.currentRank() == repast::RepastProcess::instance()->rank();
}

class SpaceTranslator {
    std::vector<double> cellPartOffset;
    std::vector<double> cellPartScale;
    double _cellSize;

   public:
    SpaceTranslator() {
        cellPartOffset.push_back(0);
        cellPartOffset.push_back(0);

        cellPartScale.push_back(0);
        cellPartScale.push_back(0);
        _cellSize = 0;
    }
    SpaceTranslator(repast::Point<double> partOrigin,
                    repast::Point<double> partExtent,
                    repast::Point<double> cellOrigin,
                    repast::Point<double> cellExtent, double areaSize) {
        cellPartOffset = std::vector<double>();
        cellPartScale = std::vector<double>();

        cellPartOffset.push_back(partOrigin[0] - cellOrigin[0]);
        cellPartOffset.push_back(partOrigin[1] - cellOrigin[1]);

        cellPartScale.push_back(partExtent[0] / (double)cellExtent[0]);
        cellPartScale.push_back(partExtent[1] / (double)cellExtent[1]);

        _cellSize = areaSize / cellExtent[0];
    }

    inline double cellSize() { return _cellSize; }

    repast::Point<int> virToCell(repast::Point<double> in) {
        repast::Point<int> out(
            (in[0] - cellPartOffset[0]) / cellPartScale[0] - 0.5,
            (in[1] - cellPartOffset[1]) / cellPartScale[1] - 0.5);

        return out;
    }

    repast::Point<double> cellToPart(repast::Point<int> in) {
        repast::Point<double> out(
            ((double)in[0] + 0.5) * cellPartScale[0] + cellPartOffset[0],
            ((double)in[1] + 0.5) * cellPartScale[1] + cellPartOffset[1]);

        return out;
    }

    repast::Point<int> cellToPartDisc(repast::Point<int> in) {
        repast::Point<double> p = cellToPart(in);

        repast::Point<int> out((int)p[0], (int)p[1]);
        return out;
    }
};

extern SpaceTranslator spaceTrans;

#endif