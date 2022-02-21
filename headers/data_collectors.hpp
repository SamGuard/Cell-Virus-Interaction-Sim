#include "virus.hpp"
#include "repast_hpc/AgentId.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/TDataSource.h"

/* Data Collection */
class DataSource_VirusPos : public repast::TDataSource<double> {
   private:
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus> >* space;
    repast::AgentId id;
    bool useX;  // Use x or y value, true for x

   public:
    DataSource_VirusPos(repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus> >* space, repast::AgentId id,
                        bool _useX);
    double getData();
};