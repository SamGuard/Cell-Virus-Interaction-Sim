#include "parameter_config.hpp"

#include "constants.hpp"

// Descriptions of each parameter are in constants.hpp

// Cell
double CELL_DEATH_LENGTH, CELL_DIVIDE_PROB, CELL_BYSTANDER_DEATH_PROB,
    CELL_INFECT_POW_BASE, CELL_REVERT_BYSTANDER_PROB,
    CELL_PROB_TO_BYSTANDER_BASE, CELL_BYSTANDER_INFECT_SKIP_PROB,
    CELL_CHANCE_TO_SPAWN_VIRUS;
int CELL_IFN_SPAWN_NUMBER;

ParameterConfig::ParameterConfig(repast::Properties *props) {
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
    CELL_IFN_SPAWN_NUMBER =
        std::stoi(props->getProperty("CELL_IFN_SPAWN_NUMBER"));
}

ParameterConfig::ParameterConfig() {
    CELL_DEATH_LENGTH = CELL_DIVIDE_PROB = CELL_BYSTANDER_DEATH_PROB =
        CELL_INFECT_POW_BASE = CELL_REVERT_BYSTANDER_PROB =
            CELL_PROB_TO_BYSTANDER_BASE = CELL_BYSTANDER_INFECT_SKIP_PROB = 0.0;
    CELL_IFN_SPAWN_NUMBER = 0;
}