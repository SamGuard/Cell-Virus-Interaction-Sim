#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <stdlib.h>

#include "repast_hpc/Point.h"
#include "repast_hpc/Properties.h"

enum AgentType { BaseAgentType, VirusType, CellType };

extern double cellDeathChanceOvercrowding;
extern unsigned long int virusIdCount;

struct Vector {
    double x, y;
};

// Convert to an int for data logging
inline int agentTypeToInt(AgentType t) {
    switch (t) {
        default:
            printf("ERROR: BAD AGENT TYPE\n");
            return -1;
        case BaseAgentType:
            return 0;
        case VirusType:
            return 1;
        case CellType:
            return 2;
    }
}

class SpaceTranslator {
    std::vector<double> cellVirOffset;
    std::vector<double> cellVirScale;

   public:
    SpaceTranslator() {
        cellVirOffset.push_back(0);
        cellVirOffset.push_back(0);

        cellVirScale.push_back(0);
        cellVirScale.push_back(0);
    }
    SpaceTranslator(repast::Point<double> virOrigin,
                    repast::Point<double> virExtent,
                    repast::Point<double> cellOrigin,
                    repast::Point<double> cellExtent) {
        cellVirOffset = std::vector<double>();
        cellVirScale = std::vector<double>();

        cellVirOffset.push_back(virOrigin[0] - cellOrigin[0]);
        cellVirOffset.push_back(virOrigin[1] - cellOrigin[1]);

        cellVirScale.push_back(virExtent[0] / (double)cellExtent[0]);
        cellVirScale.push_back(virExtent[1] / (double)cellExtent[1]);
    }

    repast::Point<int> virToCell(repast::Point<double> in) {
        repast::Point<int> out(
            (in[0] - cellVirOffset[0]) / cellVirScale[0] - 0.5,
            (in[1] - cellVirOffset[1]) / cellVirScale[1] - 0.5);

        return out;
    }

    repast::Point<double> cellToVir(repast::Point<int> in) {
        repast::Point<double> out(
            ((double)in[0] + 0.5) * cellVirScale[0] + cellVirOffset[0],
            ((double)in[1] + 0.5) * cellVirScale[1] + cellVirOffset[1]);

        return out;
    }

    repast::Point<int> cellToVirDisc(repast::Point<int> in){
        repast::Point<double> p = cellToVir(in);

        repast::Point<int> out((int)p[0], (int) p[1]);
        return out;
    }
};

extern SpaceTranslator spaceTrans;

#endif