#include "agent.hpp"

/* Agent Package Provider */
class AgentPackageProvider {
   private:
    repast::SharedContext<Agent>* agents;

   public:
    AgentPackageProvider(
        repast::SharedContext<Agent>* agentPtr);

    void providePackage(Agent* agent,
                        std::vector<AgentPackage>& out);

    void provideContent(repast::AgentRequest req,
                        std::vector<AgentPackage>& out);
};

/* Agent Package Receiver */
class AgentPackageReceiver {
   private:
    repast::SharedContext<Agent>* agents;

   public:
    AgentPackageReceiver(
        repast::SharedContext<Agent>* agentPtr);

    Agent* createAgent(AgentPackage package);

    void updateAgent(AgentPackage package);
};