#include "data_collectors.hpp"

DataSource_VirusPos::DataSource_VirusPos(
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus> >* _space,
    int _id, int _startingProcess, bool _useX)
    : space(_space), id(_id), startingProcess(_startingProcess), useX(_useX) {}

double DataSource_VirusPos::getData() {
    std::vector<double> loc;
    repast::AgentId fullID = repast::AgentId(id, startingProcess, 0);

    space->getLocation(fullID, loc);

    if(fabs(loc[0]) > 100 || fabs(loc[1]) > 100){
        cout << "OUT OF BOUNDS " << this->id << std::endl;
    }
    if (loc.size() == 0) {
        //cout << "ERROR IN AGENT: " << fullID << std::endl;
        return 0;
    }
    if (useX) {
        return loc[0];
    }
    return loc[1];
}