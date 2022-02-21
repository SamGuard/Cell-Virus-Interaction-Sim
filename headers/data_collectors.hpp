#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/TDataSource.h"
#include "repast_hpc/AgentId.h"
#include "agent.hpp"


/* Data Collection */
class DataSource_VirusPos : public repast::TDataSource<double>{
private:
	repast::SharedContext<Virus>* context;
  repast::AgentId id;
  bool useX; // Use x or y value, true for x
    
public:
	DataSource_VirusPos(repast::SharedContext<Virus>* c, repast::AgentId id, bool _useX);
	double getData();
};