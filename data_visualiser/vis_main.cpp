#include <math.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "EasyBMP.hpp"

const int NUM_PROCS = 32;
const int NUM_CELLS = 320;
const int WIDTH = 1280;
const int HEIGHT = 1280;
const double CELL_SIZE = 1280 / NUM_CELLS;
const double SIM_EXTENT = 200;
const double SCALE = WIDTH / SIM_EXTENT;

enum State { Dead, Healthy, Infected, Empty };
enum AgentTypes { BaseAgentType, VirusType, CellType };

void transformPoints(double &x, double &y) {
    x *= SCALE;
    y *= SCALE;
}

class Agent {
   protected:
    EasyBMP::RGBColor col;

   public:
    double x, y, size;
    State state;

    Agent() {
        x = y = 0;
        size = 3;
        col = EasyBMP::RGBColor(255, 255, 255);
    }

    Agent(double x, double y, double size, EasyBMP::RGBColor col, State state)
        : x(x), y(y), size(size), state(state) {
        col = EasyBMP::RGBColor(255, 255, 255);
        update();
    }

    virtual void move(double x, double y) {
        this->x = x;
        this->y = y;
    }

    virtual void setState(State s) {
        this->state = s;
        this->update();
    }

    virtual void update() {}

    virtual void draw(EasyBMP::Image *img) {
        static const double PI = 3.1415926535;
        double i, angle, x1, y1;
        int rad = size * SCALE;

        for (i = 0; i < 360; i += 0.1) {
            angle = i;
            x1 = x;
            y1 = y;
            transformPoints(x1, y1);
            x1 += rad * cos(angle * PI / 180);
            y1 += rad * sin(angle * PI / 180);
            if(img->isValidCoordinate(x1, y1))
              img->SetPixel(x1, y1, col);
        }
    }
};

class Virus : public Agent {
   public:
    Virus() : Agent() {}
    Virus(double x, double y, double size, EasyBMP::RGBColor col, State state)
        : Agent(x, y, size, col, state) {}

    void update() {
        if (state != Healthy) {
            std::cout << "Invalid virus state" << std::endl;
        }

        this->col = EasyBMP::RGBColor(255, 0, 0);
    }
};

class Cell : public Agent {
   public:
    Cell() : Agent() {}
    Cell(double x, double y, double size, EasyBMP::RGBColor col, State state)
        : Agent(x, y, size, col, state) {}

    void update() {
        switch (state) {
            case Dead:
                col = EasyBMP::RGBColor(0, 0, 0);
            case Healthy:
                col = EasyBMP::RGBColor(255, 255, 255);
                break;
            case Infected:
                col = EasyBMP::RGBColor(95, 36, 95);
                break;
            case Empty:
                col = EasyBMP::RGBColor(64, 64, 64);
                break;
            default:
                std::cout << "Invalid Cell state" << std::endl;
                break;
        }
    }

    void draw(EasyBMP::Image *img) {
        double x1, y1;
        x1 = x;
        y1 = y;
        transformPoints(x1, y1);
        int size = this->size * SCALE;

        for (int i = -size / 2; i < size / 2; i++) {
            for (int j = -size / 2; j < size / 2; j++) {
                if(img->isValidCoordinate(x1 + i, y1 + j))
                  img->SetPixel(x1 + i, y1 + j, col);
            }
        }
    }
};

void openFiles(std::vector<std::ifstream *> &files) {
    const char baseString[] = "../output/sim_%d.dat";
    char *buff = (char *)malloc(1024 * sizeof(char));
    for (int i = 0; i < NUM_PROCS; i++) {
        sprintf(buff, baseString, i);
        files.push_back(new std::ifstream(buff, std::ios::in));
    }
    free(buff);
}

std::string makeID(int id, int sProc, int type) {
    char *buff = (char *)malloc(1024 * sizeof(char));

    sprintf(buff, "%d|%d|%d", id, sProc, type);

    std::string s(buff);
    free(buff);
    return s;
}

void splitString(std::string s, char delim, std::vector<std::string> &out) {
    int pos = 0;
    int barPos = std::min(s.find(delim), s.size());
    if (pos == barPos) {
        return;
    }
    while (true) {
        out.push_back(s.substr(pos, barPos - pos));
        // Next one
        pos = barPos + 1;
        barPos = std::min(s.find(delim, pos), s.size());
        if (pos > s.size()) {
            return;
        }
    }
}

void createAgent(std::string payload, std::map<std::string, Agent *> &agents) {
    std::vector<std::string> entries;
    splitString(payload, ',', entries);
    for (std::vector<std::string>::iterator it = entries.begin();
         it != entries.end(); it++) {
        std::string s = *it;
        std::vector<std::string> vals;
        splitString(s, '|', vals);
        if (vals.size() != 3) {
            std::cout << "Invalid entry for create agent: " << s << " size of "
                      << vals.size() << std::endl;
            return;
        }
        int agentId = std::stoi(vals[0]);
        int sProc = std::stoi(vals[1]);
        int type = std::stoi(vals[2]);

        std::string id = makeID(agentId, sProc, type);
        Agent *agent;

        switch (type) {
            case BaseAgentType:
                std::cout << "Cannot add base agent as its a template class"
                          << std::endl;
                return;
            case VirusType:
                agent = new Virus();
                agent->size = 4;
            case CellType:
                agent = new Cell();
                agent->size = CELL_SIZE;
                agent->state = Infected;
                agent->update();
            default:
                break;
        }
        agents[id] = agent;
    }
}

void killAgent(std::string payload, std::map<std::string, Agent *> &agents) {
    std::vector<std::string> entries;
    splitString(payload, ',', entries);
    for (std::vector<std::string>::iterator it = entries.begin();
         it != entries.end(); it++) {
        agents.erase(*it);
    }
}
void moveAgent(std::string payload, std::map<std::string, Agent *> &agents) {
    std::vector<std::string> entries;
    splitString(payload, ',', entries);
    for (std::vector<std::string>::iterator it = entries.begin();
         it != entries.end(); it++) {
        std::string s = *it;
        std::vector<std::string> vals;
        splitString(s, '|', vals);
        if (vals.size() != 5) {
            std::cout << "Invalid entry for createmove agent: " << s
                      << " size of " << vals.size() << std::endl;
            return;
        }
        int agentId = std::stoi(vals[0]);
        int sProc = std::stoi(vals[1]);
        int type = std::stoi(vals[2]);
        double x = std::stod(vals[3]);
        double y = std::stod(vals[4]);

        std::string id = makeID(agentId, sProc, type);

        agents[id]->move(x, y);
    }
}
void setStateOfAgent(std::string payload,
                     std::map<std::string, Agent *> &agents) {}

void draw(std::map<std::string, Agent *> agents, EasyBMP::Image *img) {
    for (std::map<std::string, Agent *>::iterator it = agents.begin();
         it != agents.end(); it++) {
        it->second->draw(img);
    }
}

void mainLoop() {
    std::vector<std::ifstream *> files;  // Store all the file streams
    openFiles(files);

    std::map<std::string, Agent *> agents;

    EasyBMP::Image *img = new EasyBMP::Image(WIDTH, HEIGHT);
    int cReader = 0;
    int currentTick = 0;
    std::string line;
    while (true) {
        getline(*(files[cReader]), line);
        if (line.length() == 0) {
            std::cout << "end" << std::endl;
            break;
        }
        int colPos = line.find(':');
        std::string command = line.substr(0, colPos);
        std::string payload = "";
        if (colPos != line.size() - 1) {
            payload = line.substr(colPos + 1, line.size());
        }

        if (command.compare("create") == 0) {
            // Add agent to scene
            createAgent(payload, agents);
        } else if (command.compare("kill") == 0) {
            killAgent(payload, agents);
        } else if (command.compare("setpos") == 0) {
            // Move agent
            moveAgent(payload, agents);
        } else if (command.compare("setstate") == 0) {
            // Change agent state
        } else if (command.compare("tick") == 0) {
            // Go to next file
            cReader = (cReader + 1) % NUM_PROCS;
            if (cReader == 0) {
                draw(agents, img);
                std::string fileName = "./images/tick_";
                fileName.append(std::to_string(currentTick));
                fileName.append(".bmp");
                img->Write(fileName);
                delete img;
                img = new EasyBMP::Image(WIDTH, HEIGHT);
                currentTick++;
                std::cout << "Tick: " << currentTick << std::endl;
            }
        }
    }
}

int main() {
    mainLoop();
    return 0;
}