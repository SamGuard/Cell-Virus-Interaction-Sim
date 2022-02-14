#ifndef MODEL
#define MODEL

#include <stdio.h>

#include <boost/mpi.hpp>
#include <iostream>

#include "agent.hpp"
#include "repast_hpc/AgentRequest.h"
#include "repast_hpc/GridComponents.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedDiscreteSpace.h"
#include "repast_hpc/TDataSource.h"

class Model {
   public:
    unsigned int lifetime;
    std::string propsFile;
    repast::Properties* props;

    repast::SharedDiscreteSpace<Agent, repast::WrapAroundBorders,
                                repast::SimpleAdder<Agent> >* discreteSpace;
    repast::SharedContinuousSpace<Agent, repast::WrapAroundBorders,
                                  repast::SimpleAdder<Agent> >* continSpace;

    repast::SharedContext<Agent> context;

    void init(std::string propsFile, int argc, char** argv,
              boost::mpi::communicator* comm);
    void initSchedule();
    Model() { 
      std::cout << "Model Created:)" << std::endl; 

      }
};
#endif