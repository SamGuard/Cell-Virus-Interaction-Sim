#include <math.h>

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

const int NUM_PROCS = 4;
const int NUM_CELLS = 32;
const int WIDTH = 1280;
const int HEIGHT = 1280;
const double CELL_SIZE = WIDTH / NUM_CELLS;
const double SIM_EXTENT = 200;
const double SCALE = WIDTH / SIM_EXTENT;
const int MAX_LAYERS = 2;

enum State { Dead, Healthy, Infected, Empty };
enum AgentTypes { BaseAgentType, VirusType, CellType };

void transformPoints(double &x, double &y) {
    x *= SCALE;
    y *= SCALE;
}

class Agent {
   protected:
    sf::Color col;
    int layer;

   public:
    double x, y, size;
    State state;

    Agent() {
        x = y = 0;
        size = 15;
        layer = 0;
        col = sf::Color(255, 255, 255);
        state = Healthy;
    }

    Agent(double x, double y, double size, sf::Color col, State state)
        : x(x), y(y), size(size), state(state) {
        col = sf::Color(255, 255, 255);
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

    int getLayer() { return layer; }

    virtual void update() {}

    virtual void draw(sf::RenderTexture *img) = 0;
};

class Particle : public Agent {
   protected:
    sf::CircleShape shape;

   public:
    Particle() : Agent() { layer = 1; }
    Particle(double x, double y, double size, sf::Color col, State state)
        : Agent(x, y, size, col, state) {
        layer = 1;
        state = Healthy;
        shape = sf::CircleShape(size * SCALE);
    }

    void update() {
        if (state != Healthy) {
            std::cout << "Invalid virus state " << state << std::endl;
        }
        col = sf::Color(255, 0, 0);
        shape.setFillColor(col);
    }

    void draw(sf::RenderTexture *img) {
        double x1, y1;
        x1 = x;
        y1 = y;
        transformPoints(x1, y1);
        shape.setPosition(x1, y1);
        img->draw(shape);
    }
};

class Cell : public Agent {
   protected:
    sf::RectangleShape shape;

   public:
    Cell() : Agent() { layer = 0; }
    Cell(double x, double y, double size, sf::Color col, State state)
        : Agent(x, y, size, col, state) {
        layer = 0;
        sf::Vector2f s(size, size);
        shape = sf::RectangleShape(s);
    }

    void update() {
        switch (state) {
            case Dead:
                col = sf::Color(0, 0, 0);
                break;
            case Healthy:
                col = sf::Color(255, 255, 255);
                break;
            case Infected:
                col = sf::Color(95, 36, 95);
                break;
            case Empty:
                col = sf::Color(64, 64, 64);
                break;
            default:
                std::cout << "Invalid Cell state" << std::endl;
                break;
        }
        shape.setFillColor(col);
    }

    void draw(sf::RenderTexture *img) {
        double x0, y0;
        x0 = x;
        y0 = y;
        transformPoints(x0, y0);
        shape.setPosition(x0 - size / 2, y0 - size / 2);
        img->draw(shape);
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
                agent = new Particle(0, 0, 2, sf::Color(), Healthy);
                agent->update();
                break;
            case CellType:
                agent = new Cell(0, 0, CELL_SIZE, sf::Color(), Healthy);
                agent->update();
                break;
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
        if (vals.size() == 0) {
            return;
        }
        if (vals.size() != 5) {
            std::cout << "Invalid entry for move agent: " << s << " size of "
                      << vals.size() << std::endl;
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
                     std::map<std::string, Agent *> &agents) {
    std::vector<std::string> entries;
    splitString(payload, ',', entries);
    for (std::vector<std::string>::iterator it = entries.begin();
         it != entries.end(); it++) {
        std::string s = *it;
        std::vector<std::string> vals;
        splitString(s, '|', vals);
        if (vals.size() == 0) {
            return;
        }
        if (vals.size() != 4) {
            std::cout << "Invalid entry for set agent state: " << s
                      << " size of " << vals.size() << std::endl;
            return;
        }
        int agentId = std::stoi(vals[0]);
        int sProc = std::stoi(vals[1]);
        int type = std::stoi(vals[2]);
        int state = std::stod(vals[3]);

        std::string id = makeID(agentId, sProc, type);

        Agent *a = agents[id];
        a->setState((State)state);
        a->update();
    }
}

void draw(std::map<std::string, Agent *> agents, sf::RenderTexture *img) {
    for (int layer = 0; layer < MAX_LAYERS; layer++) {
        for (std::map<std::string, Agent *>::iterator it = agents.begin();
             it != agents.end(); it++) {
            if (it->second->getLayer() == layer) {
                it->second->draw(img);
            }
        }
    }
}

void mainLoop() {
    std::vector<std::ifstream *> files;  // Store all the file streams
    openFiles(files);

    std::map<std::string, Agent *> agents;

    sf::RenderTexture *img = new sf::RenderTexture();
    img->create(WIDTH, HEIGHT);
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
            setStateOfAgent(payload, agents);
        } else if (command.compare("tick") == 0) {
            // Go to next file
            cReader = (cReader + 1) % NUM_PROCS;
            if (cReader == 0) {
                draw(agents, img);
                std::string fileName = "./images/tick_";
                fileName.append(std::to_string(currentTick));
                fileName.append(".png");
                img->display();
                img->getTexture().copyToImage().saveToFile(fileName);
                img->clear();
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