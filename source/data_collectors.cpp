#include "data_collectors.hpp"

DataSource_VirusPos::DataSource_VirusPos(repast::SharedContext<Virus>* _context,
                                         repast::AgentId _id, bool _useX)
    : context(_context), id(_id), useX(_useX) {}

double DataSource_VirusPos::getData() {
  Vector v = context->getAgent(id)->getVel();
  if(useX){
    return v.x;
  }
  return v.y;
  
}