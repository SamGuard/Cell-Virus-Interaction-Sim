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
    std::cout << "Model created" << std::endl;
    props = new repast::Properties(propsFile, argc, argv, comm);
    initializeRandom(*props, comm);
    provider = new AgentPackageProvider(&context);
    receiver = new AgentPackageReceiver(&context);

    lifetime = stoi(props->getProperty("lifetime"));
    cout << lifetime << std::endl;

    repast::Point<double> origin(-100, -100);
    repast::Point<double> extent(200, 200);

    repast::GridDimensions gd(origin, extent);

    std::vector<int> processDims;
    processDims.push_back(2);
    processDims.push_back(2);

    discreteSpace =
        new repast::SharedDiscreteSpace<Agent, repast::StrictBorders,
                                        repast::SimpleAdder<Agent> >(
            "AgentDiscreteSpace", gd, processDims, 2, comm);
    continSpace =
        new repast::SharedContinuousSpace<Agent, repast::StrictBorders,
                                          repast::SimpleAdder<Agent> >(
            "AgentContinuousSpace", gd, processDims, 0, comm);

    context.addProjection(continSpace);
    context.addProjection(discreteSpace);
}

void Model::init() {
    int rank = repast::RepastProcess::instance()->rank();
    repast::Random* randNum = repast::Random::instance();
    for (int i = 0; i < countOfAgents; i++) {
        double offsetX = randNum->nextDouble() * 10.0 - 5.0,
               offsetY = randNum->nextDouble() * 10.0 - 5.0;
        repast::Point<int> initialLocationDiscrete((int)offsetX, (int)offsetY);
        repast::Point<double> initialLocationContinuous(offsetX, offsetY);
        repast::AgentId id(i, rank, 0);
        id.currentRank(rank);
        Vector vel;
        vel.x = randNum->nextDouble();
        vel.y = randNum->nextDouble();
        Agent* agent = new Agent(id, vel, 0);
        context.addAgent(agent);
        discreteSpace->moveTo(id, initialLocationDiscrete);
        continSpace->moveTo(id, initialLocationContinuous);
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
    std::vector<Agent*> agents;
    context.selectAgents(repast::SharedContext<Agent>::LOCAL, countOfAgents,
                         agents);

    std::vector<Agent*>::iterator it = agents.begin();

    it = agents.begin();
    while (it != agents.end()) {
        (*it)->move(discreteSpace, continSpace);
        it++;
    }
}

void Model::interact() {
    std::vector<Agent*> agents;
    context.selectAgents(repast::SharedContext<Agent>::LOCAL, countOfAgents,
                         agents);

    std::vector<Agent*>::iterator it = agents.begin();
    while (it != agents.end()) {
        (*it)->interact(&context, discreteSpace, continSpace);
        it++;
    }
}

void Model::printAgentCounters() {
    repast::RepastProcess::instance()
        ->synchronizeAgentStates<AgentPackage, AgentPackageProvider,
                                 AgentPackageReceiver>(*provider, *receiver);
    if (repast::RepastProcess::instance()->rank() != 0) {
        return;
    }
    std::vector<Agent*> agents;
    context.selectAgents(countOfAgents, agents);

    std::vector<Agent*>::iterator it = agents.begin();
    while (it != agents.end()) {
        std::cout << "Agent " << (*it)->getId() << " Value "
                  << (*it)->testCounter << std::endl;
        it++;
    }
}