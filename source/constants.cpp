#include "constants.hpp"

// Convert to an int for data logging
int agentTypeToInt(AgentType t){
  switch(t){
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