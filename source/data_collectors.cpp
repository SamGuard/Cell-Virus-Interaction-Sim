#include "data_collectors.hpp"

DataSource_VirusPos::DataSource_VirusPos(
    repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                  repast::SimpleAdder<Virus> >* _space,
    repast::AgentId _id, bool _useX)
    : space(_space), id(_id), useX(_useX) {}

double DataSource_VirusPos::getData() {
    std::vector<double> loc;
    space->getLocation(id, loc);
    if (useX) {
        return loc[0];
    }
    return loc[1];
}