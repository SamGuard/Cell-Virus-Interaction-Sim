#include "agentbase.hpp"
#include "cell.hpp"
#include "repast_hpc/Context.h"
#include "particle.hpp"

/* Agent Package Provider */
class ParticlePackageProvider {
   private:
    repast::SharedContext<Particle>* agents;

   public:
    ParticlePackageProvider(repast::SharedContext<Particle>* agentPtr);

    void providePackage(Particle* agent, std::vector<ParticlePackage>& out);

    void provideContent(repast::AgentRequest req,
                        std::vector<ParticlePackage>& out);
};

/* Agent Package Receiver */
class ParticlePackageReceiver {
   private:
    repast::SharedContext<Particle>* agents;

   public:
    ParticlePackageReceiver(repast::SharedContext<Particle>* agentPtr);

    Particle* createAgent(ParticlePackage package);

    void updateAgent(ParticlePackage package);
};

/* Agent Package Provider */
class CellPackageProvider {
   private:
    repast::SharedContext<Cell>* agents;

   public:
    CellPackageProvider(repast::SharedContext<Cell>* agentPtr);

    void providePackage(Cell* agent, std::vector<CellPackage>& out);

    void provideContent(repast::AgentRequest req,
                        std::vector<CellPackage>& out);
};

/* Agent Package Receiver */
class CellPackageReceiver {
   private:
    repast::SharedContext<Cell>* agents;

   public:
    CellPackageReceiver(repast::SharedContext<Cell>* agentPtr);

    Cell* createAgent(CellPackage package);

    void updateAgent(CellPackage package);
};
