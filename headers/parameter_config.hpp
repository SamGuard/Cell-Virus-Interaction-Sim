#ifndef PARAM_CONFIG
#define PARAM_CONFIG

#include "repast_hpc/Properties.h"

class ParameterConfig {
   public:
    ParameterConfig();
    ParameterConfig(repast::Properties* props);
    double scaleProb(double p, double time);
};

#endif