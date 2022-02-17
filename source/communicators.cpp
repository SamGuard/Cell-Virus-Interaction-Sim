#include "communicators.hpp"

VirusPackageProvider::VirusPackageProvider(
    repast::SharedContext<Virus>* agentPtr)
    : agents(agentPtr) {}

void VirusPackageProvider::providePackage(Virus* agent,
                                          std::vector<AgentPackage>& out) {
    repast::AgentId id = agent->getId();
    AgentPackage package(id.id(), id.startingRank(), id.agentType(),
                         id.currentRank(), agent->getVel().x, agent->getVel().y,
                         agent->getTestCounter());
    out.push_back(package);
}

void VirusPackageProvider::provideContent(repast::AgentRequest req,
                                          std::vector<AgentPackage>& out) {
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for (size_t i = 0; i < ids.size(); i++) {
        providePackage(agents->getAgent(ids[i]), out);
    }
}

VirusPackageReceiver::VirusPackageReceiver(
    repast::SharedContext<Virus>* agentPtr)
    : agents(agentPtr) {}

Virus* VirusPackageReceiver::createAgent(AgentPackage package) {
    repast::AgentId id(package.id, package.rank, package.type,
                       package.currentRank);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    return new Virus(id, v, package.testCounter);
}

void VirusPackageReceiver::updateAgent(AgentPackage package) {
    repast::AgentId id(package.id, package.rank, package.type);
    Virus* agent = agents->getAgent(id);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    agent->set(id, v, package.testCounter);
}