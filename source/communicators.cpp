#include "communicators.hpp"

// Virus --------------------------------------------------------------

VirusPackageProvider::VirusPackageProvider(
    repast::SharedContext<Virus>* agentPtr)
    : agents(agentPtr) {}

void VirusPackageProvider::providePackage(Virus* agent,
                                          std::vector<VirusPackage>& out) {
    repast::AgentId id = agent->getId();
    VirusPackage package(id.id(), id.startingRank(), id.agentType(),
                         id.currentRank(), agent->getReceptorType(),
                         agent->getVel().x, agent->getVel().y,
                         agent->getBirthTick(), agent->getAttFactors());

    out.push_back(package);
}

void VirusPackageProvider::provideContent(repast::AgentRequest req,
                                          std::vector<VirusPackage>& out) {
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for (size_t i = 0; i < ids.size(); i++) {
        providePackage(agents->getAgent(ids[i]), out);
    }
}

VirusPackageReceiver::VirusPackageReceiver(
    repast::SharedContext<Virus>* agentPtr)
    : agents(agentPtr) {}

Virus* VirusPackageReceiver::createAgent(VirusPackage package) {
    repast::AgentId id(package.id, package.rank, package.type,
                       package.currentRank);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    return new Virus(id, v, package.birthTick);
}

void VirusPackageReceiver::updateAgent(VirusPackage package) {
    repast::AgentId id(package.id, package.rank, package.type);
    Virus* agent = agents->getAgent(id);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    agent->set(id, v, package.birthTick, package.receptorType,
               package.attFactors);
}

// Cell ----------------------------------------------------------------

CellPackageProvider::CellPackageProvider(repast::SharedContext<Cell>* agentPtr)
    : agents(agentPtr) {}

void CellPackageProvider::providePackage(Cell* agent,
                                         std::vector<CellPackage>& out) {
    repast::AgentId id = agent->getId();
    CellPackage package(
        id.id(), id.startingRank(), id.agentType(), id.currentRank(),
        agent->getReceptorType(), agent->getState(), agent->getNextState(),
        agent->hasStateChanged, agent->getDeathTick(), agent->getAttFactors());
    out.push_back(package);
}

void CellPackageProvider::provideContent(repast::AgentRequest req,
                                         std::vector<CellPackage>& out) {
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for (size_t i = 0; i < ids.size(); i++) {
        providePackage(agents->getAgent(ids[i]), out);
    }
}

CellPackageReceiver::CellPackageReceiver(repast::SharedContext<Cell>* agentPtr)
    : agents(agentPtr) {}

Cell* CellPackageReceiver::createAgent(CellPackage package) {
    repast::AgentId id(package.id, package.rank, package.type,
                       package.currentRank);

    Cell* out = new Cell();
    out->set(id, package.state, package.nextState,
             package.hasStateChanged, package.deathTick, package.receptorType,
             package.attFactors);
    out->setDeathTick(package.deathTick);
    return out;
}

void CellPackageReceiver::updateAgent(CellPackage package) {
    repast::AgentId id(package.id, package.rank, package.type);
    Cell* agent = agents->getAgent(id);

    agent->set(id, package.state, package.nextState, package.hasStateChanged,
               package.deathTick, package.receptorType, package.attFactors);
}