#include "vis_main.hpp"

const int NUM_PROCS = 32;
const int NUM_CELLS = 320;
const int WIDTH = 1280;
const int HEIGHT = 1280;

void openFiles(std::vector<std::ifstream> *files) {
    const char baseString[] = "sim_%d.dat";
    char *buff = (char *)malloc(1024 * sizeof(char));
    for (int i = 0; i < NUM_PROCS; i++) {
      sprintf(buff, baseString, i);
      std::ifstream f(buff, std::ios::in);
      files->push_back(f);
    }
}

int main() { 
  std::vector<std::ifstream> files;
  openFiles(&files);

  return 0; 
  }