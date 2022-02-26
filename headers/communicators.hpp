#include "agentbase.hpp"
#include "virus.hpp"
#include "repast_hpc/Context.h"

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
