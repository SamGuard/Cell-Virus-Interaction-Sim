#include "model.hpp"

#include <stdio.h>
#include <ctime>
#include <boost/mpi.hpp>
#include <vector>

#define RANK 1

Model::Model(std::string propsFile, int argc, char** argv,
             boost::mpi::communicator* comm)
    : context(comm) {
    props = new repast::Properties(propsFile, argc, argv, comm);

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

    // Data collection
    // Create the data set builder
    std::string fileOutputName("./output/agent_pos_data.csv");
    builder = new repast::SVDataSetBuilder(
        fileOutputName.c_str(), ",",
        repast::RepastProcess::instance()->getScheduleRunner().schedule());
}

void Model::init() {
    int rank = repast::RepastProcess::instance()->rank();
    repast::Random* randNum = repast::Random::instance();
    randNum->initialize(std::time(NULL));
    double spawnSize = 100.0;

    char* dataXString = (char*)malloc(128 * sizeof(char));
    char* dataYString = (char*)malloc(128 * sizeof(char));


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
        context.addAgent(agent);
        virusDiscreteSpace->moveTo(id, initialLocationDiscrete);
        virusContinSpace->moveTo(id, initialLocationContinuous);

        sprintf(dataXString, "agent_x_%d", i);
        sprintf(dataYString, "agent_y_%d", i);

        // Loging Agent positions
        this->builder->addDataSource(createSVDataSource(
            dataXString, new DataSource_VirusPos(virusContinSpace, id.id(), rank, true),
            std::plus<double>()));

        this->builder->addDataSource(createSVDataSource(
            dataYString, new DataSource_VirusPos(virusContinSpace, id.id(), rank, false),
            std::plus<double>()));
    }

    // Use the builder to create the data set
    agentsPos = builder->createDataSet();
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
    
    
    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(
            agentsPos, &repast::DataSet::record)));
    /*
    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(
            agentsPos, &repast::DataSet::write)));
    */
    runner.scheduleEndEvent(
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(
            agentsPos, &repast::DataSet::write)));

    // End of life events
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
    
    virusDiscreteSpace->balance();  
    repast::RepastProcess::instance()
        ->synchronizeAgentStatus<Virus, VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(
            context, *virusProvider, *virusReceiver, *virusReceiver);

    repast::RepastProcess::instance()
        ->synchronizeProjectionInfo<Virus, VirusPackage, VirusPackageProvider,
                                    VirusPackageReceiver>(
            context, *virusProvider, *virusReceiver, *virusReceiver);

    repast::RepastProcess::instance()
        ->synchronizeAgentStates<VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(*virusProvider,
                                                       *virusReceiver);
    std::vector<Virus*> test;
    context.selectAgents(test, false);
    if(test.size() != 10){
        cout << "LOST AGENTS " << test.size() << " " << repast::RepastProcess::instance()->rank() << std::endl;
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
        ->synchronizeAgentStates<VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(*virusProvider,
                                                       *virusReceiver);
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