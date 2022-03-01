#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>

enum AgentType {BaseAgentType, VirusType, CellType};

// Biology Constants
#define CELL_CHANCE_DEATH_OVERCROWD 0.05


// Convert to an int for data logging
int agentTypeToInt(AgentType t);

struct Vector{
  double x, y;
};

#endif