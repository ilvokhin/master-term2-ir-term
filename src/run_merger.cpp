#include <iostream>

#include "merger.hpp"

int main(int argc, char* argv[])
{
  std::cin.sync_with_stdio(false);
  std::cout.sync_with_stdio(false);

  if(argc == 1) {
    std::cerr << "Where I should save your index, dude?" << std::endl;
    return 1;
  }

  ir::merger::merger merger;

  std::string line;

  while(std::getline(std::cin, line)) {
    merger.add_index(line);
    std::cerr << "loaded: " << line << std::endl;   
  }

  std::cerr << "start merging" << std::endl;

  merger.merge(argv[1]);

  std::cerr << "all done" << std::endl;

  return 0;
}
