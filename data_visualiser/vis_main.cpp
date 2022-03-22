#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "EasyBMP.hpp"

const int NUM_PROCS = 32;
const int NUM_CELLS = 320;
const int WIDTH = 1280;
const int HEIGHT = 1280;

class Agent{
  double x, y;
  Agent(){
    x = y = 0;
  }

  void draw(){
    
  }
}


void openFiles(std::vector<std::ifstream *> *files) {
    const char baseString[] = "sim_%d.dat";
    char *buff = (char *)malloc(1024 * sizeof(char));
    for (int i = 0; i < NUM_PROCS; i++) {
        sprintf(buff, baseString, i);
        files->push_back(new std::ifstream(buff));
    }
}

void loop() {
    std::vector<std::ifstream *> files;  // Store all the file streams
    openFiles(&files);

    EasyBMP::Image img(WIDTH, HEIGHT);
    int cReader = 0;
    std::string line;
    while (true) {
      getline(*files[cReader], line);
      if(line.length() == 0){
        break;
      }
      int colPos = line.find(':');
      std::string command = line.substr(0, colPos);
      std:: string payload = "";
      if(colPos != line.size() - 1){
        payload = line.substr(colPos + 1, line.size());
      }

      if(command.compare("create") == 0){
        // Add agent to scene
      } else if(command.compare("kill") == 0){
        // Remove agent from scene
      } else if(command.compare("setpos") == 0){
        // Move agent
      } else if(command.compare("setstate") == 0){
        // Change agent state
      }
    }
}

int main() { return 0; }