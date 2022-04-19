#include "parameter_config.hpp"

#include "globals.hpp"

// Descriptions of each parameter are in constants.hpp
bool VIS_DATA_OUTPUT;

// Cell
double CELL_DEATH_LENGTH, CELL_DIVIDE_PROB, CELL_BYSTANDER_DEATH_PROB,
    VIRUS_INFECT_PROB, CELL_REVERT_BYSTANDER_PROB, CELL_TO_BYSTANDER_PROB,
    CELL_BYSTANDER_INFECT_SKIP_PROB, CELL_PROB_TO_SPAWN_VIRUS,
    CELL_INFECT_NEIGH_CELL_PROB;
int CELL_VIRUS_SPAWN_NUMBER, CELL_IFN_SPAWN_NUMBER;

// Particle
double VIRUS_LIFETIME, INNATE_LIFETIME, INNATE_KILL_VIRUS_PROB,
    INNATE_KILL_CELL_PROB, INNATE_RECRUIT_PROB, ANTIBODY_LIFETIME,
    ANTIBODY_KILL_PROB, VIRUS_SPEED, IFN_SPEED, INNATE_SPEED, ANTIBODY_SPEED,
    INNATE_RANGE, ANTIBODY_RANGE;

// Human repsonse
double AVG_TIME_TO_FIND_ANTIBODY, DEV_TIME_TO_FIND_ANTIBODY,
    PROB_PER_UNIT_TO_SPAWN_INNATE, THREAT_LEVEL_SENSITIVITY,
    THREAT_LEVEL_SMOOTHING, PROB_PER_UNIT_TO_SPAWN_ANTIBODY;

double ParameterConfig::scaleProb(double p, double time) {
    return p == 1.0 ? 1.0 : 1.0 - pow(1.0 - p, time);
}

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
    VIRUS_INFECT_PROB = std::stod(props->getProperty("VIRUS_INFECT_PROB"));
    CELL_TO_BYSTANDER_PROB =
        std::stod(props->getProperty("CELL_TO_BYSTANDER_PROB"));
    CELL_PROB_TO_SPAWN_VIRUS =
        1.0 / std::stod(props->getProperty("CELL_TIME_TO_SPAWN_VIRUS"));
    CELL_INFECT_NEIGH_CELL_PROB =
        std::stod(props->getProperty("CELL_INFECT_NEIGH_CELL_PROB"));
    CELL_VIRUS_SPAWN_NUMBER =
        std::stoi(props->getProperty("CELL_VIRUS_SPAWN_NUMBER"));
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

    AVG_TIME_TO_FIND_ANTIBODY =
        std::stod(props->getProperty("AVG_TIME_TO_FIND_ANTIBODY"));
    DEV_TIME_TO_FIND_ANTIBODY =
        std::stod(props->getProperty("DEV_TIME_TO_FIND_ANTIBODY"));
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
    // Speeds
    VIRUS_SPEED *= SIM_PHYS_SCALE * SIM_TIME_SCALE;
    IFN_SPEED *= SIM_PHYS_SCALE * SIM_TIME_SCALE;
    INNATE_SPEED *= SIM_PHYS_SCALE * SIM_TIME_SCALE;
    ANTIBODY_SPEED *= SIM_PHYS_SCALE * SIM_TIME_SCALE;

    // Distances
    INNATE_RANGE *= SIM_PHYS_SCALE;
    ANTIBODY_RANGE *= SIM_PHYS_SCALE;

    // Scale with time
    // Convert length of time to ticks
    CELL_DEATH_LENGTH /= SIM_TIME_SCALE;
    VIRUS_LIFETIME /= SIM_TIME_SCALE;
    INNATE_LIFETIME /= SIM_TIME_SCALE;
    ANTIBODY_LIFETIME /= SIM_TIME_SCALE;

    // Converts x per min to x per tick
    PROB_PER_UNIT_TO_SPAWN_INNATE *= SIM_TIME_SCALE;
    PROB_PER_UNIT_TO_SPAWN_ANTIBODY *= SIM_TIME_SCALE;

    // Probabilities
    CELL_DIVIDE_PROB = scaleProb(CELL_DIVIDE_PROB, SIM_TIME_SCALE);
    CELL_BYSTANDER_DEATH_PROB =
        scaleProb(CELL_BYSTANDER_DEATH_PROB, SIM_TIME_SCALE);
    CELL_REVERT_BYSTANDER_PROB =
        scaleProb(CELL_REVERT_BYSTANDER_PROB, SIM_TIME_SCALE);
    CELL_BYSTANDER_INFECT_SKIP_PROB =
        scaleProb(CELL_BYSTANDER_INFECT_SKIP_PROB, SIM_TIME_SCALE);
    VIRUS_INFECT_PROB = scaleProb(VIRUS_INFECT_PROB, SIM_TIME_SCALE);
    CELL_TO_BYSTANDER_PROB = scaleProb(CELL_TO_BYSTANDER_PROB, SIM_TIME_SCALE);
    CELL_PROB_TO_SPAWN_VIRUS =
        scaleProb(CELL_PROB_TO_SPAWN_VIRUS, SIM_TIME_SCALE);
    CELL_INFECT_NEIGH_CELL_PROB =
        scaleProb(CELL_INFECT_NEIGH_CELL_PROB, SIM_TIME_SCALE);
    // std::cout << CELL_INFECT_NEIGH_CELL_PROB << std::endl;
    INNATE_KILL_VIRUS_PROB = scaleProb(INNATE_KILL_VIRUS_PROB, SIM_TIME_SCALE);
    INNATE_KILL_CELL_PROB = scaleProb(INNATE_KILL_CELL_PROB, SIM_TIME_SCALE);
    ANTIBODY_KILL_PROB = scaleProb(ANTIBODY_KILL_PROB, SIM_TIME_SCALE);
}

ParameterConfig::ParameterConfig() {
    CELL_DEATH_LENGTH = CELL_DIVIDE_PROB = CELL_BYSTANDER_DEATH_PROB =
        VIRUS_INFECT_PROB = CELL_REVERT_BYSTANDER_PROB =
            CELL_TO_BYSTANDER_PROB = CELL_BYSTANDER_INFECT_SKIP_PROB = 0.0;
    CELL_IFN_SPAWN_NUMBER = 0;
}