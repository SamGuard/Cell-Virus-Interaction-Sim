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
    double area;
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

        while (innateToAdd >= 1 ||
               repast::Random::instance()->nextDouble() < innateToAdd) {
            partToAdd->push_back(std::tuple<repast::Point<double>, AgentType>(
                repast::Point<double>(-1, -1), InnateImmuneType));
            innateToAdd--;
        }

        {
            threatLevel =
                (THREAT_LEVEL_SMOOTHING * threatLevel + 1.0 -
                 1.0 / (1.0 + THREAT_LEVEL_SENSITIVITY * removeVirusCount)) /
                (THREAT_LEVEL_SMOOTHING + 1);
            if (repast::RepastProcess::instance()
                    ->getScheduleRunner()
                    .currentTick() > tickToFindCure) {
                cureFound = true;

                double antibodyToAdd =
                    area * PROB_PER_UNIT_TO_SPAWN_ANTIBODY * threatLevel;

                while (antibodyToAdd >= 1 ||
                       repast::Random::instance()->nextDouble() <
                           antibodyToAdd) {
                    partToAdd->push_back(
                        std::tuple<repast::Point<double>, AgentType>(
                            repast::Point<double>(-1, -1), AntibodyType));
                    antibodyToAdd--;
                }
            }
        }
    }
    double getData() { return threatLevel; }
};