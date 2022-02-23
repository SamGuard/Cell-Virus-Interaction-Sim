#include "repast_hpc/AgentId.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/TDataSource.h"
#include "virus.hpp"

/* Data Collection */
class DataSource_VirusPos : public repast::TDataSource<double> {
   private:
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus> >* space;
    int id, startingProcess;
    bool useX;  // Use x or y value, true for x

   public:
    DataSource_VirusPos(
        repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                      repast::SimpleAdder<Virus> >* space,
        int id, int startingProcess, bool useX);
    double getData();
};