#include "model.hpp"

#include <stdio.h>

#include <boost/mpi.hpp>
#include <vector>

#include "repast_hpc/AgentId.h"
#include "repast_hpc/Point.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/RepastProcess.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "repast_hpc/Utilities.h"
#include "repast_hpc/initialize_random.h"

Model::Model(std::string propsFile, int argc, char** argv,
             boost::mpi::communicator* comm)
    : context(comm) {
    props = new repast::Properties(propsFile, argc, argv, comm);
    initializeRandom(*props, comm);
    virusProvider = new VirusPackageProvider(&context);
    virusReceiver = new VirusPackageReceiver(&context);

    lifetime = stoi(props->getProperty("lifetime"));
    countOfAgents = stoi(props->getProperty("agentCount"));
    cout << lifetime << std::endl;

    double areaSize = 200.0;
    repast::Point<double> origin(-areaSize / 2.0, -areaSize / 2.0);
    repast::Point<double> extent(areaSize, areaSize);

    repast::GridDimensions gd(origin, extent);

    std::vector<int> processDims;
    processDims.push_back(2);
    processDims.push_back(2);

    virusDiscreteSpace =
        new repast::SharedDiscreteSpace<Virus, repast::StrictBorders,
                                        repast::SimpleAdder<Virus> >(
            "AgentDiscreteSpace", gd, processDims, 2, comm);
    virusContinSpace =
        new repast::SharedContinuousSpace<Virus, repast::StrictBorders,
                                          repast::SimpleAdder<Virus> >(
            "AgentContinuousSpace", gd, processDims, 0, comm);

    context.addProjection(virusContinSpace);
    context.addProjection(virusDiscreteSpace);
}

void Model::init() {
    int rank = repast::RepastProcess::instance()->rank();
    repast::Random* randNum = repast::Random::instance();
    double spawnSize = 100.0;
    for (int i = 0; i < countOfAgents; i++) {
        double offsetX = randNum->nextDouble() * spawnSize - spawnSize / 2,
               offsetY = randNum->nextDouble() * spawnSize - spawnSize / 2;
        repast::Point<int> initialLocationDiscrete((int)offsetX, (int)offsetY);
        repast::Point<double> initialLocationContinuous(offsetX, offsetY);
        repast::AgentId id(i, rank, 0);
        id.currentRank(rank);
        Vector vel;
        vel.x = randNum->nextDouble();
        vel.y = randNum->nextDouble();
        Virus* agent = new Virus(id, vel, 0);
        context.addAgent(agent);
        virusDiscreteSpace->moveTo(id, initialLocationDiscrete);
        virusContinSpace->moveTo(id, initialLocationContinuous);
    }
}

void Model::initSchedule(repast::ScheduleRunner& runner) {
    runner.scheduleEvent(
        2, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::move)));

    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::interact)));

    runner.scheduleEndEvent(repast::Schedule::FunctorPtr(
        new repast::MethodFunctor<Model>(this, &Model::printAgentCounters)));
    runner.scheduleStop(lifetime);
}

void Model::move() {
    std::vector<Virus*> agents;
    context.selectAgents(repast::SharedContext<Virus>::LOCAL, countOfAgents,
                         agents);

    std::vector<Virus*>::iterator it = agents.begin();

    it = agents.begin();
    while (it != agents.end()) {
        (*it)->move(virusDiscreteSpace, virusContinSpace);
        it++;
    }
}

void Model::interact() {
    std::vector<Virus*> agents;
    context.selectAgents(repast::SharedContext<Virus>::LOCAL, countOfAgents,
                         agents);

    std::vector<Virus*>::iterator it = agents.begin();
    while (it != agents.end()) {
        (*it)->interact(&context, virusDiscreteSpace, virusContinSpace);
        it++;
    }
}

void Model::printAgentCounters() {
    repast::RepastProcess::instance()
        ->synchronizeAgentStates<AgentPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(*virusProvider, *virusReceiver);
    if (repast::RepastProcess::instance()->rank() != 0) {
        return;
    }
    std::vector<Virus*> agents;
    context.selectAgents(countOfAgents, agents);

    std::vector<Virus*>::iterator it = agents.begin();
    while (it != agents.end()) {
        std::cout << "Agent " << (*it)->getId() << " Value "
                  << (*it)->testCounter << std::endl;
        it++;
    }
}