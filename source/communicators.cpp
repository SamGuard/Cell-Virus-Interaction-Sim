#include "communicators.hpp"

AgentPackageProvider::AgentPackageProvider(
    repast::SharedContext<Agent>* agentPtr)
    : agents(agentPtr) {}

void AgentPackageProvider::providePackage(Agent* agent,
                                          std::vector<AgentPackage>& out) {
    repast::AgentId id = agent->getId();
    AgentPackage package(id.id(), id.startingRank(), id.agentType(),
                         id.currentRank(), agent->getVel().x, agent->getVel().y,
                         agent->getTestCounter());
    out.push_back(package);
}

void AgentPackageProvider::provideContent(repast::AgentRequest req,
                                          std::vector<AgentPackage>& out) {
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for (size_t i = 0; i < ids.size(); i++) {
        providePackage(agents->getAgent(ids[i]), out);
    }
}

AgentPackageReceiver::AgentPackageReceiver(
    repast::SharedContext<Agent>* agentPtr)
    : agents(agentPtr) {}

Agent* AgentPackageReceiver::createAgent(AgentPackage package) {
    repast::AgentId id(package.id, package.rank, package.type,
                       package.currentRank);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    return new Agent(id, v, package.testCounter);
}

void AgentPackageReceiver::updateAgent(AgentPackage package) {
    repast::AgentId id(package.id, package.rank, package.type);
    Agent* agent = agents->getAgent(id);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    agent->set(id, v, package.testCounter);
}