#include "agent.hpp"

/* Agent Package Provider */
class VirusPackageProvider {
   private:
    repast::SharedContext<Virus>* agents;

   public:
    VirusPackageProvider(repast::SharedContext<Virus>* agentPtr);

    void providePackage(Virus* agent, std::vector<AgentPackage>& out);

    void provideContent(repast::AgentRequest req,
                        std::vector<AgentPackage>& out);
};

/* Agent Package Receiver */
class VirusPackageReceiver {
   private:
    repast::SharedContext<Virus>* agents;

   public:
    VirusPackageReceiver(repast::SharedContext<Virus>* agentPtr);

    Virus* createAgent(AgentPackage package);

    void updateAgent(AgentPackage package);
};