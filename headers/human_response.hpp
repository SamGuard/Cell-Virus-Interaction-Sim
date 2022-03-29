#include <vector>

#include "cell.hpp"
#include "constants.hpp"
#include "particle.hpp"
#include "repast_hpc/Point.h"

class HumanResponse {
   public:
    int averageTimeFindingRec = 700;
    double threatLevel;
    double tickToFindCure;
    bool cureFound;
    int area;
    HumanResponse() {
        cureFound = false;
        area = repast::RepastProcess::instance()->worldSize();
        area *= area;
        double r = repast::Random::instance()->nextDouble();
        tickToFindCure = averageTimeFindingRec + 100.0 * (r * r - 0.5);
        threatLevel = 0;
    }

    void response(
        int innateCellCount, int removeVirusCount,
        std::vector<std::tuple<repast::Point<double>, AgentType>>* partToAdd) {
        if (innateCellCount < 1) {
            partToAdd->push_back(std::tuple<repast::Point<double>, AgentType>(
                repast::Point<double>(-1, -1), InnateImmuneType));
        }

        {
            threatLevel = 1 - 1 / (1 + removeVirusCount);
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
};