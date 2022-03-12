#include "model.hpp"

#include <stdio.h>

#include <boost/mpi.hpp>
#include <ctime>
#include <vector>

#define RANK 1

Model::Model(std::string propsFile, int argc, char** argv,
             boost::mpi::communicator* comm) {
    props = new repast::Properties(propsFile, argc, argv, comm);

    lifetime = stoi(props->getProperty("lifetime"));
    countOfAgents = stoi(props->getProperty("agentCount"));
    cellCount = stoi(props->getProperty("cellCount"));

    // Contexts
    contexts.virus = new repast::SharedContext<Virus>(comm);
    contexts.cell = new repast::SharedContext<Cell>(comm);

    // Comms
    comms.virusProv = new VirusPackageProvider(contexts.virus);
    comms.virusRec = new VirusPackageReceiver(contexts.virus);
    comms.cellProv = new CellPackageProvider(contexts.cell);
    comms.cellRec = new CellPackageReceiver(contexts.cell);

    std::vector<int> processDims;
    processDims.push_back(2);
    processDims.push_back(2);

    double virusAreaSize = 200;

    repast::Point<double> vOrigin(0, 0);
    repast::Point<double> vExtent(virusAreaSize, virusAreaSize);

    repast::Point<double> cOrigin(0, 0);
    repast::Point<double> cExtent(cellCount, cellCount);

    spaceTrans = SpaceTranslator(vOrigin, vExtent, cOrigin, cExtent);

    // Virus spaces
    {
        repast::GridDimensions gd(vOrigin, vExtent);

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

    // Cell Space
    {
        if ((double)cellCount / processDims[0] !=
                (int)(cellCount / processDims[0]) ||
            (double)cellCount / processDims[1] !=
                (int)(cellCount / processDims[1])) {
            cout << "Cell count must be a multiple of the processor dims"
                 << std::endl;
        }

        repast::GridDimensions gd(cOrigin, cExtent);

        spaces.cellDisc =
            new repast::SharedDiscreteSpace<Cell, repast::StrictBorders,
                                            repast::SimpleAdder<Cell>>(
                "CellDiscreteSpace", gd, processDims, 1, comm);

        contexts.cell->addProjection(spaces.cellDisc);
    }
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
    sprintf(fileOutputName, "output/sim_%d.dat", rank);
    simDataFile.open(fileOutputName, std::ios::out | std::ios::trunc);

    dataCol = DataCollector(&simData, &simDataFile);

    // Add viruses to model
    double spawnSize = 199.0;

    for (int i = 0; i < countOfAgents; i++) {
        double offsetX = randNum->nextDouble() * spawnSize,
               offsetY = randNum->nextDouble() * spawnSize;
        repast::Point<int> initialLocationDiscrete((int)offsetX, (int)offsetY);
        repast::Point<double> initialLocationContinuous(offsetX, offsetY);
        repast::AgentId id(i, rank, agentTypeToInt(VirusType));
        id.currentRank(rank);

        Vector vel;
        vel.x = randNum->nextDouble() - 0.5;
        vel.y = randNum->nextDouble() - 0.5;

        Virus* agent = new Virus(id, vel, 0);

        contexts.virus->addAgent(agent);
        spaces.virusDisc->moveTo(id, initialLocationDiscrete);
        spaces.virusCont->moveTo(id, initialLocationContinuous);

        dataCol.newAgent(id);
        dataCol.setPos(id, initialLocationContinuous.coords(), true);
    }

    for (int i = 0; i < (cellCount * cellCount) / worldSize; i++) {
        int indexOffset = rank * ((cellCount * cellCount) / worldSize);
        repast::Point<int> pos((indexOffset + i) % cellCount,
                               (indexOffset + i) / cellCount);
        repast::AgentId id(i, rank, agentTypeToInt(CellType));
        Cell* agent = new Cell(id, (i == 0) ? Healthy : Empty);
        contexts.cell->addAgent(agent);
        spaces.cellDisc->moveTo(id, pos);

        repast::Point<double> vPos = spaceTrans.cellToVir(pos);
        dataCol.newAgent(id);
        dataCol.setPos(id, vPos.coords(), true);
        dataCol.setState(id, agent->getState(), true);
    }

    simData << "sortlayers:" << std::endl;

    // Move randomly places agents into the correct processes
    balanceAgents();
}

void Model::initSchedule(repast::ScheduleRunner& runner) {
    // Output tick every tick
    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::outputTick)));

    runner.scheduleEvent(
        1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::printTick)));

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
        1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::collectCellData)));

    runner.scheduleEvent(
        1, 5,
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<DataCollector>(
            &this->dataCol, &DataCollector::writeData)));

    // End of life events
    runner.scheduleEndEvent(repast::Schedule::FunctorPtr(
        new repast::MethodFunctor<Model>(this, &Model::printAgentCounters)));

    runner.scheduleEndEvent(
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<DataCollector>(
            &this->dataCol, &DataCollector::writeData)));

    runner.scheduleStop(lifetime);
}

void Model::balanceAgents() {
    // Virus
    spaces.virusDisc->balance();
    repast::RepastProcess::instance()
        ->synchronizeAgentStatus<Virus, VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(
            *contexts.virus, *comms.virusProv, *comms.virusRec,
            *comms.virusRec);

    repast::RepastProcess::instance()
        ->synchronizeProjectionInfo<Virus, VirusPackage, VirusPackageProvider,
                                    VirusPackageReceiver>(
            *contexts.virus, *comms.virusProv, *comms.virusRec,
            *comms.virusRec);

    repast::RepastProcess::instance()
        ->synchronizeAgentStates<VirusPackage, VirusPackageProvider,
                                 VirusPackageReceiver>(*comms.virusProv,
                                                       *comms.virusRec);

    // Move cells into correct processor
    spaces.cellDisc->balance();
    repast::RepastProcess::instance()
        ->synchronizeAgentStatus<Cell, CellPackage, CellPackageProvider,
                                 CellPackageReceiver>(
            *contexts.cell, *comms.cellProv, *comms.cellRec, *comms.cellRec);

    repast::RepastProcess::instance()
        ->synchronizeProjectionInfo<Cell, CellPackage, CellPackageProvider,
                                    CellPackageReceiver>(
            *contexts.cell, *comms.cellProv, *comms.cellRec, *comms.cellRec);

    repast::RepastProcess::instance()
        ->synchronizeAgentStates<CellPackage, CellPackageProvider,
                                 CellPackageReceiver>(*comms.cellProv,
                                                      *comms.cellRec);
}

void Model::move() {
    std::vector<Virus*> agents;

    if (contexts.virus->size() == 0) {
        balanceAgents();
        return;
    }

    contexts.virus->selectAgents(repast::SharedContext<Virus>::LOCAL, agents,
                                 false);
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
    // Virus
    {
        std::vector<Virus*> agents;
        if (contexts.virus->size() == 0) {
            return;
        }
        contexts.virus->selectAgents(repast::SharedContext<Virus>::LOCAL,
                                     agents);
        std::vector<Virus*>::iterator it = agents.begin();
        while (it != agents.end()) {
            (*it)->interact(contexts.virus, spaces.virusDisc, spaces.virusCont);
            it++;
        }
    }

    // Cell
    {
        std::vector<Cell*> agents;
        if (contexts.virus->size() == 0) {
            return;
        }
        contexts.cell->selectAgents(repast::SharedContext<Cell>::LOCAL, agents);
        std::vector<Cell*>::iterator it = agents.begin();
        while (it != agents.end()) {
            (*it)->interact(contexts.cell, spaces.cellDisc);
            it++;
        }

        it = agents.begin();
        while (it != agents.end()) {
            (*it)->goNextState();
            it++;
        }
    }
}

void Model::collectVirusData() {
    //  Array of tuples
    //  Tuple is id, start rank, is in this proc, x, y and state
    std::vector<std::tuple<repast::AgentId, double, double, int>> out;

    // If there are any viruses to log data for
    if (contexts.virus->size() != 0) {
        std::vector<Virus*> agents;
        contexts.virus->selectAgents(agents, false);
        std::vector<Virus*>::iterator it = agents.begin();
        // Iterate threw and get the location of them all
        while (it != agents.end()) {
            Virus* a = (*it);
            std::vector<double> loc;
            spaces.virusCont->getLocation(a->getId(), loc);

            out.push_back(std::make_tuple(a->getId(), loc[0], loc[1], 1));

            it++;
        }
    }

    simData << "setpos:";
    std::tuple<repast::AgentId, double, double, int> entry;
    for (long unsigned int i = 0; i < out.size(); i++) {
        entry = out[i];
        std::vector<double> loc;
        loc.push_back(std::get<1>(entry));
        loc.push_back(std::get<2>(entry));

        dataCol.setPos(std::get<0>(entry), loc, false);
    }
    simData << std::endl;

    /*
    simData << "setstate:";

    for (long unsigned int i = 0; i < out.size(); i++) {
        entry = out[i];
        dataCol.setState(std::get<0>(entry), std::get<3>(entry), false);
    }

    simData << std::endl;
    */
}

void Model::collectCellData() {
    //  Array of tuples
    //  Tuple is id and agent state
    std::vector<std::tuple<repast::AgentId, int>> out;

    if (contexts.cell->size() != 0) {
        std::vector<Cell*> agents;
        contexts.cell->selectAgents(agents, false);
        std::vector<Cell*>::iterator it = agents.begin();
        // Iterate through and get the location of them all
        while (it != agents.end()) {
            Cell* a = (*it);
            out.push_back(std::make_tuple(a->getId(), a->getState()));
            it++;
        }
    }

    simData << "setstate:";
    std::tuple<repast::AgentId, int> entry;
    for (long unsigned int i = 0; i < out.size(); i++) {
        entry = out[i];
        dataCol.setState(std::get<0>(entry), std::get<1>(entry), false);
    }

    simData << std::endl;
}

void Model::printAgentCounters() {
    if (repast::RepastProcess::instance()->rank() != 0) {
        return;
    }
    std::vector<Virus*> agents;
    contexts.virus->selectAgents(agents, false);

    std::vector<Virus*>::iterator it = agents.begin();
    while (it != agents.end()) {
        std::cout << "Agent " << (*it)->getId() << " Value "
                  << (*it)->testCounter << std::endl;
        it++;
    }
}

void Model::outputTick() {
    simData
        << "tick:"
        << repast::RepastProcess::instance()->getScheduleRunner().currentTick()
        << std::endl;
}

void Model::printTick() {
    repast::RepastProcess* inst = repast::RepastProcess::instance();
    if (inst->rank() != 0) {
        return;
    }
    cout << "tick: " << inst->getScheduleRunner().currentTick() << std::endl;
}