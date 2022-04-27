#include "model.hpp"

#include <stdio.h>

#include <boost/mpi.hpp>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

#include "globals.hpp"
#include "parameter_config.hpp"
#include "repast_hpc/AgentRequest.h"
#include "repast_hpc/GridComponents.h"
#include "repast_hpc/Properties.h"
#include "repast_hpc/Random.h"
#include "repast_hpc/SVDataSet.h"
#include "repast_hpc/SVDataSetBuilder.h"
#include "repast_hpc/SVDataSource.h"
#include "repast_hpc/Schedule.h"
#include "repast_hpc/SharedContext.h"
#include "repast_hpc/SharedDiscreteSpace.h"
#include "repast_hpc/TDataSource.h"

unsigned long int particleIdCount;
double SIM_PHYS_SCALE;
double SIM_TIME_SCALE;
int BATCH_NUM;

SpaceTranslator spaceTrans;

Model::Model(std::string propsFile, int argc, char** argv,
             boost::mpi::communicator* comm) {
    props = new repast::Properties(propsFile, argc, argv, comm);

    std::cout << "RANK " << repast::RepastProcess::instance()->rank() << std::endl;

    // Contexts
    contexts.part = new repast::SharedContext<Particle>(comm);
    contexts.cell = new repast::SharedContext<Cell>(comm);

    // Comms
    comms.partProv = new ParticlePackageProvider(contexts.part);
    comms.partRec = new ParticlePackageReceiver(contexts.part);
    comms.cellProv = new CellPackageProvider(contexts.cell);
    comms.cellRec = new CellPackageReceiver(contexts.cell);

    // Define simulation parameters
    lifetime = stoi(props->getProperty("LIFETIME"));
    virusCount = stoi(props->getProperty("VIRUS_COUNT"));
    BATCH_NUM = stoi(props->getProperty("BATCH_NUM"));

    // Physical scale definition
    double areaSize =
        stod(props->getProperty("SIM_SIZE"));  // Size of the area to simulate
    double simSize = 1000;                     // Size of area in the simulation
    SIM_PHYS_SCALE = simSize / areaSize;

    // Time scale definition
    SIM_TIME_SCALE = std::stod(props->getProperty("TIME_PER_TICK"));

    double densityOfCells = stod(props->getProperty("DENSITY_OF_CELLS"));

    paramConfig = ParameterConfig(props);

    std::vector<int> processDims;
    processDims.push_back(std::stoi(props->getProperty("procDimsX")));
    processDims.push_back(std::stoi(props->getProperty("procDimsY")));

    {
        int worldSize = repast::RepastProcess::instance()->worldSize();
        cellCount = sqrt(areaSize * areaSize * densityOfCells);
        cellCount = cellCount + (worldSize - (cellCount % worldSize));
    }

    repast::Point<double> pOrigin(0, 0);
    repast::Point<double> pExtent(simSize, simSize);

    repast::Point<double> cOrigin(0, 0);
    repast::Point<double> cExtent(cellCount, cellCount);

    spaceTrans = SpaceTranslator(pOrigin, pExtent, cOrigin, cExtent, simSize);

    human = HumanResponse(areaSize);

    // Virus spaces
    {
        repast::GridDimensions gd(pOrigin, pExtent);

        spaces.partCont =
            new repast::SharedContinuousSpace<Particle, repast::StrictBorders,
                                              repast::SimpleAdder<Particle>>(
                "AgentContinuousSpace", gd, processDims, 0, comm);

        spaces.partDisc =
            new repast::SharedDiscreteSpace<Particle, repast::StrictBorders,
                                            repast::SimpleAdder<Particle>>(
                "AgentDiscreteSpace", gd, processDims, 1, comm);
        contexts.part->addProjection(spaces.partCont);
        contexts.part->addProjection(spaces.partDisc);
    }

    // Cell Space
    {
        if (cellCount % processDims[0] != 0 ||
            cellCount % processDims[1] != 0) {
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

void Model::initDataLogging() {
    rank = repast::RepastProcess::instance()->rank();

    // file to log agent positions and state to
    if (VIS_DATA_OUTPUT) {
        std::cout << "Logging data for visualation..." << std::endl;
        char* fileOutputName = (char*)malloc(128 * sizeof(char));
        sprintf(fileOutputName, "output/sim_%d_%d.dat", BATCH_NUM, rank);
        simDataFile.open(fileOutputName, std::ios::out | std::ios::trunc);
    }

    dataCol = DataCollector(&simData, &simDataFile);

    // Create the data set builder
    char* totalsOutputName = (char*)malloc(128 * sizeof(char));
    sprintf(totalsOutputName, "output/agent_totals_data_%d.csv", BATCH_NUM);
    repast::SVDataSetBuilder builder = repast::SVDataSetBuilder(
        totalsOutputName, ",",
        repast::RepastProcess::instance()->getScheduleRunner().schedule());

    AgentTotals<Particle>*virus, *ifn, *innate, *antiB;
    AgentTotals<Cell>*cellHealthy, *cellInfected, *cellDead;
    virus = new AgentTotals<Particle>(contexts.part, VirusType);
    ifn = new AgentTotals<Particle>(contexts.part, InterferonType);
    innate = new AgentTotals<Particle>(contexts.part, InnateImmuneType);
    antiB = new AgentTotals<Particle>(contexts.part, AntibodyType);

    cellHealthy = new AgentTotals<Cell>(contexts.cell, CellType, Healthy);
    cellInfected = new AgentTotals<Cell>(contexts.cell, CellType, Infected);
    cellDead = new AgentTotals<Cell>(contexts.cell, CellType, Dead);

    TrueTime* time = new TrueTime();

    builder.addDataSource(
        createSVDataSource("Time", time, std::plus<double>()));
    builder.addDataSource(
        createSVDataSource("Total_Viruses", virus, std::plus<int>()));
    builder.addDataSource(createSVDataSource("Total_Cells_Healthy", cellHealthy,
                                             std::plus<int>()));
    builder.addDataSource(createSVDataSource("Total_Cells_Infected",
                                             cellInfected, std::plus<int>()));
    builder.addDataSource(
        createSVDataSource("Total_Cells_Dead", cellDead, std::plus<int>()));
    builder.addDataSource(
        createSVDataSource("Total_IFNs", ifn, std::plus<int>()));
    builder.addDataSource(createSVDataSource("Total_Innate_Immune_Cell", innate,
                                             std::plus<int>()));
    builder.addDataSource(
        createSVDataSource("Total_Antibodies", antiB, std::plus<int>()));
    builder.addDataSource(
        createSVDataSource("threat_level", &human, std::plus<double>()));

    // Use the builder to create the data set
    agentTotalData = builder.createDataSet();
}

void Model::init() {
    rank = repast::RepastProcess::instance()->rank();
    worldSize = repast::RepastProcess::instance()->worldSize();
    repast::Random* randNum = repast::Random::instance();
    // randNum->initialize(std::time(NULL));
    randNum->initialize(27 + rank);
    // Add viruses to model
    double spawnOriginX = spaces.partCont->dimensions().origin().getX(),
           spawnOriginY = spaces.partCont->dimensions().origin().getY();

    double spawnSizeX = spaces.partCont->dimensions().extents().getX(),
           spawnSizeY = spaces.partCont->dimensions().extents().getY();
    if (repast::RepastProcess::instance()->rank() == worldSize / 2) {
        particleIdCount = 0;
        for (int i = 0; i < virusCount; i++) {
            double offsetX = spawnOriginX + randNum->nextDouble() * spawnSizeX,
                   offsetY = spawnOriginY + randNum->nextDouble() * spawnSizeY;
            addParticle(repast::Point<double>(offsetX, offsetY), VirusType);
        }
    }

    for (int i = 0; i < 1; i++) {
        double offsetX = spawnOriginX + randNum->nextDouble() * spawnSizeX,
               offsetY = spawnOriginY + randNum->nextDouble() * spawnSizeY;
        addParticle(repast::Point<double>(offsetX, offsetY), InnateImmuneType);
    }

    {
        int originX = (int)spaces.cellDisc->dimensions().origin().getX(),
            originY = (int)spaces.cellDisc->dimensions().origin().getY();

        int extentX = (int)spaces.cellDisc->dimensions().extents().getX(),
            extentY = (int)spaces.cellDisc->dimensions().extents().getY();

        for (int x = 0; x < extentX; x++) {
            for (int y = 0; y < extentY; y++) {
                repast::Point<int> pos =
                    repast::Point<int>(originX + x, originY + y);
                repast::AgentId id(x + y * extentX, rank, (int)(CellType));
                Cell* agent = new Cell(id, Healthy);

                contexts.cell->addAgent(agent);
                spaces.cellDisc->moveTo(id, pos);

                repast::Point<double> vPos = spaceTrans.cellToPart(pos);
                dataCol.newAgent(id);
                dataCol.setPos(id, vPos.coords(), true);
                dataCol.setState(id, agent->getState(), true);
            }
        }
    }
    simData << "sortlayers:" << std::endl;

    // Move randomly places agents into the correct processes
    balanceAgents();

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

void Model::initSchedule(repast::ScheduleRunner& runner) {
    // Output tick every tick
    runner.scheduleEvent(
        1.0, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::outputTick)));

    runner.scheduleEvent(
        1.0, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::printTick)));

    runner.scheduleEvent(
        1.1, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::move)));

    runner.scheduleEvent(
        1.2, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::interact)));

    runner.scheduleEvent(
        1.3, 1,
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<Model>(
            this, &Model::collectParticleData)));

    runner.scheduleEvent(
        1.3, 1,
        repast::Schedule::FunctorPtr(
            new repast::MethodFunctor<Model>(this, &Model::collectCellData)));
    runner.scheduleEvent(
        1.3, 1,
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(
            agentTotalData, &repast::DataSet::record)));
    runner.scheduleEvent(
        1.4, 10,
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(
            agentTotalData, &repast::DataSet::write)));

    runner.scheduleEvent(
        1.4, 10,
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<DataCollector>(
            &this->dataCol, &DataCollector::writeData)));

    // End of life events
    runner.scheduleEndEvent(
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<DataCollector>(
            &this->dataCol, &DataCollector::writeData)));
    runner.scheduleEndEvent(
        repast::Schedule::FunctorPtr(new repast::MethodFunctor<repast::DataSet>(
            agentTotalData, &repast::DataSet::write)));

    runner.scheduleStop(lifetime);
}

void Model::balanceAgents() {
    // Particle
    spaces.partDisc->balance();
    repast::RepastProcess::instance()
        ->synchronizeAgentStatus<Particle, ParticlePackage,
                                 ParticlePackageProvider,
                                 ParticlePackageReceiver>(
            *contexts.part, *comms.partProv, *comms.partRec, *comms.partRec);

    repast::RepastProcess::instance()
        ->synchronizeProjectionInfo<Particle, ParticlePackage,
                                    ParticlePackageProvider,
                                    ParticlePackageReceiver>(
            *contexts.part, *comms.partProv, *comms.partRec, *comms.partRec);

    repast::RepastProcess::instance()
        ->synchronizeAgentStates<ParticlePackage, ParticlePackageProvider,
                                 ParticlePackageReceiver>(*comms.partProv,
                                                          *comms.partRec);
}

void Model::move() {
    std::vector<Particle*> agents;

    if (contexts.part->size() == 0) {
        balanceAgents();
        return;
    }

    contexts.part->selectAgents(repast::SharedContext<Particle>::LOCAL, agents,
                                false);
    std::vector<Particle*>::iterator it = agents.begin();
    it = agents.begin();
    std::vector<double> loc;
    while (it != agents.end()) {
        (*it)->move(spaces.partDisc, spaces.partCont);
        it++;
    }

    balanceAgents();
}

void Model::interact() {
    // Synchronise Cells
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

    std::vector<std::tuple<repast::Point<double>, AgentType>> partToAdd;
    std::set<Particle*> partToRemove;
    int innateCount = 0;
    // Particle
    if (contexts.part->size() > 0) {
        {
            std::vector<Particle*> agents;
            contexts.part->selectAgents(repast::SharedContext<Particle>::LOCAL,
                                        agents);
            for (std::vector<Particle*>::iterator it = agents.begin();
                 it != agents.end(); it++) {
                Particle* p = *it;
                switch (p->getAgentType()) {
                    case InnateImmuneType: {
                        InnateImmune* newP = (InnateImmune*)p;
                        newP->interact(contexts.part, spaces.partDisc,
                                       spaces.partCont, &partToAdd,
                                       &partToRemove);
                        innateCount++;
                        break;
                    }
                    case AntibodyType: {
                        Antibody* newP = (Antibody*)p;
                        newP->interact(contexts.part, spaces.partDisc,
                                       spaces.partCont, &partToAdd,
                                       &partToRemove);
                        break;
                    }
                    default:
                        p->interact(contexts.part, spaces.partDisc,
                                    spaces.partCont, &partToAdd, &partToRemove);
                        break;
                }
            }
        }
    }

    // Remove particles that have died of old age
    removeParticles(partToRemove);

    // Cell
    int removeInfectedCellCount = 0;
    for (repast::SharedContext<Cell>::const_local_iterator it =
             contexts.cell->localBegin();
         it != contexts.cell->localEnd(); it++) {
        (*it)->interact(contexts.cell, spaces.cellDisc, spaces.partDisc,
                        &partToAdd, &partToRemove, removeInfectedCellCount);
    }

    {
        int removeVirusCount = 0;
        for (std::set<Particle*>::iterator it = partToRemove.begin();
             it != partToRemove.end(); it++) {
            if ((*it)->getAgentType() == VirusType) {
                removeVirusCount++;
            }
        }
        human.response(innateCount, removeVirusCount + removeInfectedCellCount,
                       &partToAdd);
    }

    removeParticles(partToRemove);

    addParticles(partToAdd);

    // Set cells to their next state
    {
        for (repast::SharedContext<Cell>::const_local_iterator it =
                 contexts.cell->localBegin();
             it != contexts.cell->localEnd(); it++) {
            if ((*it)->hasStateChanged) {
                (*it)->goNextState();
            }
        }
    }

    balanceAgents();
}

void Model::addParticle(AgentType t) {
    double spawnOriginX = spaces.partCont->dimensions().origin().getX(),
           spawnOriginY = spaces.partCont->dimensions().origin().getY();

    double spawnSizeX = spaces.partCont->dimensions().extents().getX(),
           spawnSizeY = spaces.partCont->dimensions().extents().getY();
    double offsetX = spawnOriginX +
                     repast::Random::instance()->nextDouble() * spawnSizeX,
           offsetY = spawnOriginY +
                     repast::Random::instance()->nextDouble() * spawnSizeY;
    addParticle(repast::Point<double>(offsetX, offsetY), t);
}

void Model::addParticle(repast::Point<double> loc, AgentType t) {
    if ((int)loc.getX() == -1 && (int)loc.getY() == -1) {
        addParticle(t);
        return;
    }
    int rank = repast::RepastProcess::instance()->rank();
    int tick =
        repast::RepastProcess::instance()->getScheduleRunner().currentTick();
    repast::Random* randNum = repast::Random::instance();
    repast::Point<int> locDisc((int)loc[0], (int)loc[1]);
    repast::AgentId id(particleIdCount, rank, (int)t);
    id.currentRank(rank);

    Vector vel;
    double dir = randNum->nextDouble() * 3.141 * 2.0;
    vel.x = cos(dir);
    vel.y = sin(dir);

    Particle* agent;

    switch (t) {
        case VirusType:
            vel.x *= VIRUS_SPEED;
            vel.y *= VIRUS_SPEED;
            agent = new Virus(id, vel, tick);
            agent->addAttachFactor(REC_CELL);
            break;
        case InterferonType:
            vel.x *= IFN_SPEED;
            vel.y *= IFN_SPEED;
            agent = new Interferon(id, vel, tick);
            agent->addAttachFactor(REC_CELL);
            break;
        case InnateImmuneType:

            vel.x *= INNATE_SPEED;
            vel.y *= INNATE_SPEED;
            agent = new InnateImmune(id, vel, tick);
            break;
        case AntibodyType:
            vel.x *= ANTIBODY_SPEED;
            vel.y *= ANTIBODY_SPEED;
            agent = new Antibody(id, vel, tick);
            agent->addAttachFactor(REC_CELL);
            break;
        default:
            std::cout << "Invalid particle type" << std::endl;
            return;
    }

    contexts.part->addAgent(agent);
    spaces.partCont->moveTo(id, loc);
    spaces.partDisc->moveTo(id, locDisc);

    dataCol.newAgent(id);
    dataCol.setPos(id, loc.coords(), true);
    particleIdCount++;
}

void Model::addParticles(
    std::vector<std::tuple<repast::Point<double>, AgentType>>& p) {
    for (std::vector<std::tuple<repast::Point<double>, AgentType>>::iterator
             it = p.begin();
         it != p.end(); it++) {
        addParticle(std::get<0>(*it), std::get<1>(*it));
    }
}

void Model::removeParticle(Particle* v) {
    // For debugging
    /*
    std::cout
        << "REMOVING " << v->getId() << " ON TICK "
        << repast::RepastProcess::instance()->getScheduleRunner().currentTick();
    */
    repast::AgentId id = v->getId();

    dataCol.killAgent(id);
    contexts.part->removeAgent(v);
    repast::RepastProcess::instance()->agentRemoved(id);
}

void Model::removeParticles(std::set<Particle*>& v) {
    for (std::set<Particle*>::iterator it = v.begin(); it != v.end(); it++) {
        removeParticle(*it);
    }
    repast::RepastProcess::instance()
        ->synchronizeAgentStatus<Particle, ParticlePackage,
                                 ParticlePackageProvider,
                                 ParticlePackageReceiver>(
            *contexts.part, *comms.partProv, *comms.partRec, *comms.partRec);
    v.clear();
}

void Model::collectParticleData() {
    if (!VIS_DATA_OUTPUT) return;
    //  Array of tuples
    //  Tuple is id, start rank, is in this proc, x, y and state
    std::vector<std::tuple<repast::AgentId, double, double, int>> out;

    // If there are any viruses to log data for
    if (contexts.part->size() != 0) {
        std::vector<Particle*> agents;
        contexts.part->selectAgents(repast::SharedContext<Particle>::LOCAL,
                                    agents);
        std::vector<Particle*>::iterator it = agents.begin();
        // Iterate threw and get the location of them all
        while (it != agents.end()) {
            Particle* a = (*it);
            std::vector<double> loc;
            spaces.partCont->getLocation(a->getId(), loc);

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
    if (!VIS_DATA_OUTPUT) return;
    //  Array of tuples
    //  Tuple is id and agent state
    std::vector<std::tuple<repast::AgentId, int>> out;

    if (contexts.cell->size() != 0) {
        std::vector<Cell*> agents;
        contexts.cell->selectAgents(repast::SharedContext<Cell>::LOCAL, agents);
        std::vector<Cell*>::iterator it = agents.begin();
        // Iterate through and get the location of them all
        while (it != agents.end()) {
            Cell* a = (*it);
            if (a->hasStateChanged) {
                out.push_back(std::make_tuple(a->getId(), a->getState()));
            }
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
    std::vector<Particle*> agents;
    contexts.part->selectAgents(agents, false);

    std::vector<Particle*>::iterator it = agents.begin();
    while (it != agents.end()) {
        std::cout << "Agent " << (*it)->getId() << " Value " << std::endl;
        it++;
    }
}

void Model::outputTick() {
    simData << "tick:"
            << (repast::RepastProcess::instance()
                    ->getScheduleRunner()
                    .currentTick())
            << std::endl;
}

void Model::printTick() {
    repast::RepastProcess* inst = repast::RepastProcess::instance();
    if (inst->rank() != 0) {
        return;
    }
    cout << "tick: " << (inst->getScheduleRunner().currentTick()) << std::endl;
}