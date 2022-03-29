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

extern double cellDeathChanceOvercrowding;
extern unsigned long int particleIdCount;
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