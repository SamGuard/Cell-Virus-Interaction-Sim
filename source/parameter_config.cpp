#include "parameter_config.hpp"

#include "constants.hpp"

// Descriptions of each parameter are in constants.hpp
bool VIS_DATA_OUTPUT;

// Cell
double CELL_DEATH_LENGTH, CELL_DIVIDE_PROB, CELL_BYSTANDER_DEATH_PROB,
    CELL_INFECT_POW_BASE, CELL_REVERT_BYSTANDER_PROB,
    CELL_PROB_TO_BYSTANDER_BASE, CELL_BYSTANDER_INFECT_SKIP_PROB,
    CELL_CHANCE_TO_SPAWN_VIRUS;
int CELL_VIRUS_SPAWN_COUNT, CELL_IFN_SPAWN_NUMBER;

// Particle
double VIRUS_LIFETIME, INNATE_LIFETIME, INNATE_KILL_VIRUS_PROB,
    INNATE_KILL_CELL_PROB, INNATE_RECRUIT_PROB, ANTIBODY_LIFETIME,
    ANTIBODY_KILL_PROB, VIRUS_SPEED, IFN_SPEED, INNATE_SPEED, ANTIBODY_SPEED,
    INNATE_RANGE, ANTIBODY_RANGE;

// Human repsonse
double AVG_TICKS_TO_FIND_ANTIBODY, DEV_TICKS_TO_FIND_ANTIBODY,
    PROB_PER_UNIT_TO_SPAWN_INNATE, THREAT_LEVEL_SENSITIVITY,
    THREAT_LEVEL_SMOOTHING, PROB_PER_UNIT_TO_SPAWN_ANTIBODY;

ParameterConfig::ParameterConfig(repast::Properties *props) {
    VIS_DATA_OUTPUT = std::stoi(props->getProperty("VIS_DATA_OUTPUT")) == 1;

    CELL_DEATH_LENGTH = std::stod(props->getProperty("CELL_DEATH_LENGTH"));
    CELL_DIVIDE_PROB = std::stod(props->getProperty("CELL_DIVIDE_PROB"));
    CELL_BYSTANDER_DEATH_PROB =
        std::stod(props->getProperty("CELL_BYSTANDER_DEATH_PROB"));
    CELL_REVERT_BYSTANDER_PROB =
        std::stod(props->getProperty("CELL_REVERT_BYSTANDER_PROB"));
    CELL_BYSTANDER_INFECT_SKIP_PROB =
        std::stod(props->getProperty("CELL_BYSTANDER_INFECT_SKIP_PROB"));
    CELL_INFECT_POW_BASE =
        std::stod(props->getProperty("CELL_INFECT_POW_BASE"));
    CELL_PROB_TO_BYSTANDER_BASE =
        std::stod(props->getProperty("CELL_PROB_TO_BYSTANDER_BASE"));
    CELL_CHANCE_TO_SPAWN_VIRUS =
        std::stod(props->getProperty("CELL_CHANCE_TO_SPAWN_VIRUS"));
    CELL_VIRUS_SPAWN_COUNT =
        std::stoi(props->getProperty("CELL_VIRUS_SPAWN_COUNT"));
    CELL_IFN_SPAWN_NUMBER =
        std::stoi(props->getProperty("CELL_IFN_SPAWN_NUMBER"));

    VIRUS_LIFETIME = std::stod(props->getProperty("VIRUS_LIFETIME"));
    VIRUS_SPEED = std::stod(props->getProperty("VIRUS_SPEED"));
    INNATE_LIFETIME = std::stod(props->getProperty("INNATE_LIFETIME"));
    INNATE_SPEED = std::stod(props->getProperty("INNATE_SPEED"));
    INNATE_RANGE = std::stod(props->getProperty("INNATE_RANGE"));
    INNATE_KILL_VIRUS_PROB =
        std::stod(props->getProperty("INNATE_KILL_VIRUS_PROB"));
    INNATE_KILL_CELL_PROB =
        std::stod(props->getProperty("INNATE_KILL_CELL_PROB"));
    INNATE_RECRUIT_PROB = std::stod(props->getProperty("INNATE_RECRUIT_PROB"));
    ANTIBODY_LIFETIME = std::stod(props->getProperty("ANTIBODY_LIFETIME"));
    ANTIBODY_SPEED = std::stod(props->getProperty("ANTIBODY_SPEED"));
    ANTIBODY_KILL_PROB = std::stod(props->getProperty("ANTIBODY_KILL_PROB"));
    ANTIBODY_RANGE = std::stod(props->getProperty("ANTIBODY_RANGE"));
    IFN_SPEED = std::stod(props->getProperty("IFN_SPEED"));

    AVG_TICKS_TO_FIND_ANTIBODY =
        std::stod(props->getProperty("AVG_TICKS_TO_FIND_ANTIBODY"));
    DEV_TICKS_TO_FIND_ANTIBODY =
        std::stod(props->getProperty("DEV_TICKS_TO_FIND_ANTIBODY"));
    PROB_PER_UNIT_TO_SPAWN_INNATE =
        std::stod(props->getProperty("PROB_PER_UNIT_TO_SPAWN_INNATE"));
    PROB_PER_UNIT_TO_SPAWN_ANTIBODY =
        std::stod(props->getProperty("PROB_PER_UNIT_TO_SPAWN_ANTIBODY"));
    THREAT_LEVEL_SENSITIVITY =
        std::stod(props->getProperty("THREAT_LEVEL_SENSITIVITY"));
    THREAT_LEVEL_SMOOTHING =
        std::stod(props->getProperty("THREAT_LEVEL_SMOOTHING"));

    // Scale any units
    // Physical
    VIRUS_SPEED *= SIM_SCALE;
    IFN_SPEED *= SIM_SCALE;
    INNATE_SPEED *= SIM_SCALE;
    INNATE_RANGE *= SIM_SCALE;
    ANTIBODY_SPEED *= SIM_SCALE;
    ANTIBODY_RANGE *= SIM_SCALE;

    // Temporal
}

ParameterConfig::ParameterConfig() {
    CELL_DEATH_LENGTH = CELL_DIVIDE_PROB = CELL_BYSTANDER_DEATH_PROB =
        CELL_INFECT_POW_BASE = CELL_REVERT_BYSTANDER_PROB =
            CELL_PROB_TO_BYSTANDER_BASE = CELL_BYSTANDER_INFECT_SKIP_PROB = 0.0;
    CELL_IFN_SPAWN_NUMBER = 0;
}