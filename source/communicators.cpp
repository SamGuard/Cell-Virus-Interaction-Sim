#include "communicators.hpp"


AgentPackageProvider::AgentPackageProvider(repast::SharedContext<Agent>* agentPtr): agents(agentPtr){ }

void AgentPackageProvider::providePackage(Agent * agent, std::vector<AgentPackage>& out){
    repast::AgentId id = agent->getId();
    AgentPackage package(id.id(), id.startingRank(), id.agentType(), id.currentRank(), agent->getVel());
    out.push_back(package);
}

void AgentPackageProvider::provideContent(repast::AgentRequest req, std::vector<AgentPackage>& out){
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for(size_t i = 0; i < ids.size(); i++){
        providePackage(agents->getAgent(ids[i]), out);
    }
}


AgentPackageReceiver::AgentPackageReceiver(repast::SharedContext<Agent>* agentPtr): agents(agentPtr){}

Agent * AgentPackageReceiver::createAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type, package.currentRank);
    return new Agent(id, package.vel);
}

void AgentPackageReceiver::updateAgent(AgentPackage package){
    repast::AgentId id(package.id, package.rank, package.type);
    Agent * agent = agents->getAgent(id);
    agent->set(id, package.vel);
}