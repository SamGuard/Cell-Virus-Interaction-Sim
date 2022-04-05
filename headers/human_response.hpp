#include <vector>

#include "cell.hpp"
#include "constants.hpp"
#include "particle.hpp"
#include "repast_hpc/Point.h"
#include "repast_hpc/TDataSource.h"

class HumanResponse : public repast::TDataSource<double> {
   public:
    double threatLevel;
    double tickToFindCure;
    bool cureFound;
    int area;
    HumanResponse() {}
    HumanResponse(double simSize) {
        cureFound = false;
        area =
            simSize * simSize / repast::RepastProcess::instance()->worldSize();
        double r = repast::Random::instance()->nextDouble();
        tickToFindCure = AVG_TICKS_TO_FIND_ANTIBODY +
                         DEV_TICKS_TO_FIND_ANTIBODY * (r * r - 0.5);
        threatLevel = 0;
    }

    void response(
        int innateCellCount, int removeVirusCount,
        std::vector<std::tuple<repast::Point<double>, AgentType>>* partToAdd) {
        // Average amount of innate cells to add each tick for the area that
        // this process controls
        double innateToAdd = area * PROB_PER_UNIT_TO_SPAWN_INNATE;

        while (repast::Random::instance()->nextDouble() < innateToAdd) {
            partToAdd->push_back(std::tuple<repast::Point<double>, AgentType>(
                repast::Point<double>(-1, -1), InnateImmuneType));
            innateToAdd--;
        }

        {
            threatLevel = (THREAT_LEVEL_SENSITIVITY * threatLevel + 1.0 -
                           1.0 / (1.0 + removeVirusCount)) /
                          (THREAT_LEVEL_SENSITIVITY + 1);
            if (repast::RepastProcess::instance()
                    ->getScheduleRunner()
                    .currentTick() > tickToFindCure) {
                cureFound = true;
                for (int i = 0; i < pow(3, threatLevel) - 1; i++) {
                    partToAdd->push_back(
                        std::tuple<repast::Point<double>, AgentType>(
                            repast::Point<double>(-1, -1), AntibodyType));
                }
            }
        }
    }
    double getData() { return threatLevel; }
};