#include "model.hpp"

#include <stdio.h>

#include <boost/mpi.hpp>
#include <ctime>
#include <vector>

#define RANK 1

Model::Model(std::string propsFile, int argc, char** argv,
             boost::mpi::communicator* comm) {
    props = new repast::Properties(propsFile, argc, argv, comm);

    contexts.virus = new repast::SharedContext<Virus>(comm);

    comms.virusProv = new VirusPackageProvider(contexts.virus);
    comms.virusRec = new VirusPackageReceiver(contexts.virus);

    lifetime = stoi(props->getProperty("lifetime"));
    countOfAgents = stoi(props->getProperty("agentCount"));

    double areaSize = 200.0;
    repast::Point<double> origin(-areaSize / 2.0, -areaSize / 2.0);
    repast::Point<double> extent(areaSize, areaSize);

    repast::GridDimensions gd(origin, extent);

    std::vector<int> processDims;
    processDims.push_back(2);
    processDims.push_back(2);

    spaces.virusCont =
        new repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                          repast::SimpleAdder<Virus>>(
            "AgentContinuousSpace", gd, processDims, 0, comm);

    spaces.virusDisc =
        new repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                        repast::SimpleAdder<Virus>>(
            "AgentDiscreteSpace", gd, processDims, 2, comm);

    contexts.virus->addProjection(spaces.virusCont);
    contexts.virus->addProjection(spaces.virusDisc);
}

void Model::init() {
    rank = repast::RepastProcess::instance()->rank();
    worldSize = repast::RepastProcess::instance()->worldSize();
    repast::Random* randNum = repast::Random::instance();
    // randNum->initialize(std::time(NULL));
    randNum->initialize(27);

    // Data collection
    // file to log agent positions to
    char* fileOutputName = (char*)malloc(128 * sizeof(char));
    sprintf(fileOutputName, "output/virus_pos_data_%d.dat", rank);
    virusPosDataFile.open(fileOutputName, std::ios::out | std::ios::trunc);
    

    // Add viruses to model
    double spawnSize = 100.0;

    for (int i = 0; i < countOfAgents; i++) {
        double offsetX = randNum->nextDouble() * spawnSize - spawnSize / 2,
               offsetY = randNum->nextDouble() * spawnSize - spawnSize / 2;
        repast::Point<int> initialLocationDiscrete((int)offsetX, (int)offsetY);
        repast::Point<double> initialLocationContinuous(offsetX, offsetY);
        repast::AgentId id(i, rank, 0);
        id.currentRank(rank);

        Vector vel;
        vel.x = randNum->nextDouble() - 0.5;
        vel.y = randNum->nextDouble() - 0.5;
        Virus* agent = new Virus(id, vel, 0);
        virusPosData << "created:" << id.id() << "|" << id.startingRank()
                     << std::endl;
        contexts.virus->addAgent(agent);
        spaces.virusDisc->moveTo(id, initialLocationDiscrete);
        spaces.virusCont->moveTo(id, initialLocationContinuous);
    }

    std::vector<Virus*> agents;

    // Move randomly places agents into the correct processes
    balanceAgents();
}

void Model::initSchedule(repast::ScheduleRunner& runner) {
    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::move)));

    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::interact)));

    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::collectVirusData)));
    runner.scheduleEvent(
        1, 5,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::writeVirusData)));            

    // End of life events
    runner.scheduleEndEvent(repast::Schedule::FunctorPtr(
        new repast::MethodFunctor<Model>(this, &Model::printAgentCounters)));

    runner.scheduleStop(lifetime);
}

void Model::balanceAgents() {

    // Virus
    spaces.virusDisc->balance();
    repast::RepastProcess::instance()
        ->synchronizeAgentStatus<Virus, VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(
            *contexts.virus, *comms.virusProv, *comms.virusRec, *comms.virusRec);

    repast::RepastProcess::instance()
        ->synchronizeProjectionInfo<Virus, VirusPackage, VirusPackageProvider,
                                    VirusPackageReceiver>(
            *contexts.virus, *comms.virusProv, *comms.virusRec, *comms.virusRec);

    repast::RepastProcess::instance()
        ->synchronizeAgentStates<VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(*comms.virusProv,
                                                       *comms.virusRec);
}

void Model::move() {
    std::vector<Virus*> agents;

    if (contexts.virus->size() == 0) {
        balanceAgents();
        return;
    }

    contexts.virus->selectAgents(repast::SharedContext<Virus>::LOCAL, agents, false);

    std::vector<Virus*>::iterator it = agents.begin();

    it = agents.begin();
    std::vector<double> loc;
    while (it != agents.end()) {
        (*it)->move(spaces.virusDisc, spaces.virusCont);
        it++;
    }

    balanceAgents();
}

void Model::interact() {
    std::vector<Virus*> agents;
    std::vector<double> loc;

    if (contexts.virus->size() == 0) {
        return;
    }

    contexts.virus->selectAgents(repast::SharedContext<Virus>::LOCAL, agents);
    std::vector<Virus*>::iterator it = agents.begin();
    while (it != agents.end()) {
        (*it)->interact(contexts.virus, spaces.virusDisc, spaces.virusCont);
        it++;
    }
}

void Model::collectVirusData() {
    double tick =
    repast::RepastProcess::instance()->getScheduleRunner().currentTick();

    virusPosData << "tick:" << tick << std::endl;

    //  Array of tuples
    //  Tuple is id, start rank, is in this proc, x, y
    std::vector<std::tuple<int, int, double, double>> out;

    if (contexts.virus->size() != 0) {
        std::vector<Virus*> agents;
        contexts.virus->selectAgents(agents, false);
        std::vector<Virus*>::iterator it = agents.begin();
        while (it != agents.end()) {
            Virus* a = (*it);
            std::vector<double> loc;
            spaces.virusCont->getLocation(a->getId(), loc);

            out.push_back(std::make_tuple(
                a->getId().id(), a->getId().startingRank(), loc[0], loc[1]));

            it++;
        }
    }

    virusPosData << "setpos:";
    std::tuple<int, int, double, double> entry;
    for (long unsigned int i = 0; i < out.size(); i++) {
        entry = out[i];
        virusPosData << std::get<0>(entry) << "|" << std::get<1>(entry) << "|"
                     << std::get<2>(entry) << "|" << std::get<3>(entry) << ",";
    }
    virusPosData << std::endl;
}

void Model::writeVirusData(){
    virusPosDataFile << virusPosData.rdbuf();
}

void Model::printAgentCounters() {
    repast::RepastProcess::instance()
        ->synchronizeAgentStates<VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(*comms.virusProv,
                                                       *comms.virusRec);
    if (repast::RepastProcess::instance()->rank() != 0) {
        return;
    }
    std::vector<Virus*> agents;
    contexts.virus->selectAgents(countOfAgents, agents);

    std::vector<Virus*>::iterator it = agents.begin();
    while (it != agents.end()) {
        std::cout << "Agent " << (*it)->getId() << " Value "
                  << (*it)->testCounter << std::endl;
        it++;
    }
}