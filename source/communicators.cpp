#include "communicators.hpp"

// Particle --------------------------------------------------------------

ParticlePackageProvider::ParticlePackageProvider(
    repast::SharedContext<Particle>* agentPtr)
    : agents(agentPtr) {}

void ParticlePackageProvider::providePackage(
    Particle* agent, std::vector<ParticlePackage>& out) {
    repast::AgentId id = agent->getId();
    ParticlePackage package(id.id(), id.startingRank(), id.agentType(),
                            id.currentRank(), agent->getReceptorType(),
                            agent->getVel().x, agent->getVel().y,
                            agent->getBirthTick(), agent->getAttFactors());

    out.push_back(package);
}

void ParticlePackageProvider::provideContent(
    repast::AgentRequest req, std::vector<ParticlePackage>& out) {
    std::vector<repast::AgentId> ids = req.requestedAgents();
    for (size_t i = 0; i < ids.size(); i++) {
        providePackage(agents->getAgent(ids[i]), out);
    }
}

ParticlePackageReceiver::ParticlePackageReceiver(
    repast::SharedContext<Particle>* agentPtr)
    : agents(agentPtr) {}

Particle* ParticlePackageReceiver::createAgent(ParticlePackage package) {
    repast::AgentId id(package.id, package.rank, package.type,
                       package.currentRank);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    Particle* agent;
    switch (package.type) {
        case VirusType:
            agent = new Virus(id, v, package.birthTick);
            break;
        case InterferonType:
            agent = new Interferon(id, v, package.birthTick);
            break;
        default:
            std::cout
                << "Invalid agent type in ParticlePacakgeReceiver::createAgent"
                << std::endl;
            return (Particle*)0;
    }
    return agent;
}

void ParticlePackageReceiver::updateAgent(ParticlePackage package) {
    repast::AgentId id(package.id, package.rank, package.type,
                       package.currentRank);
    Particle* agent = agents->getAgent(id);
    Vector v;
    v.x = package.velx;
    v.y = package.vely;
    agent->set(id, (AgentType)package.type, v, package.birthTick,
               package.receptorType, package.attFactors);
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
    out->set(id, package.state, package.nextState, package.hasStateChanged,
             package.deathTick, package.receptorType, package.attFactors);
    out->setDeathTick(package.deathTick);
    return out;
}

void CellPackageReceiver::updateAgent(CellPackage package) {
    repast::AgentId id(package.id, package.rank, package.type);
    Cell* agent = agents->getAgent(id);

    agent->set(id, package.state, package.nextState, package.hasStateChanged,
               package.deathTick, package.receptorType, package.attFactors);
}