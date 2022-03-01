#include "agentbase.hpp"
#include "cell.hpp"
#include "repast_hpc/Context.h"
#include "virus.hpp"

/* Agent Package Provider */
class VirusPackageProvider {
   private:
    repast::SharedContext<Virus>* agents;

   public:
    VirusPackageProvider(repast::SharedContext<Virus>* agentPtr);

    void providePackage(Virus* agent, std::vector<VirusPackage>& out);

    void provideContent(repast::AgentRequest req,
                        std::vector<VirusPackage>& out);
};

/* Agent Package Receiver */
class VirusPackageReceiver {
   private:
    repast::SharedContext<Virus>* agents;

   public:
    VirusPackageReceiver(repast::SharedContext<Virus>* agentPtr);

    Virus* createAgent(VirusPackage package);

    void updateAgent(VirusPackage package);
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
