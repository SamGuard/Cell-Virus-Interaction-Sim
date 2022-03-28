#include <vector>

#include "cell.hpp"
#include "constants.hpp"
#include "particle.hpp"
#include "repast_hpc/Point.h"

class HumanResponse {
   public:
   int averageTimeFindingRec = 500;
   bool cureFound;
   int area;
    HumanResponse() {
        cureFound = false;
        area = repast::RepastProcess::instance()->worldSize();
        area *= area;
    }

    void response(
        int innateCellCount, int removeVirusCount,
        std::vector<std::tuple<repast::Point<double>, AgentType>>* partToAdd) {
        // Have a chance of adding more innate cell
        // The chance is increased the less innate cells there are and the more
        // virus cells that have been removed
        double r = repast::Random::instance()->nextDouble();

        if (r < pow(0.8, innateCellCount / area)) {
            partToAdd->push_back(std::tuple<repast::Point<double>, AgentType>(
                repast::Point<double>(-1, -1), InnateImmuneType));
        }

        if(cureFound){
            for(int i = 0; i < 10; i++){
                partToAdd->push_back(std::tuple<repast::Point<double>, AgentType>(
                repast::Point<double>(-1, -1), AntigenType));
            }
        } else {
            double r = repast::Random::instance()->nextDouble();
            if(r < 1 / ((double) averageTimeFindingRec)){
                cureFound = true;
            }
        }
    }
};