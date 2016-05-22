#include <iostream>

#include "indexer.hpp"

int main(int argc, char* argv[])
{
  std::cin.sync_with_stdio(false);
  std::cout.sync_with_stdio(false);

  if(argc == 1) {
    std::cerr << "Where I should save your index, dude?" << std::endl;
    return 1;
  }

  ir::indexer::indexer indexer;

  indexer.load_stopwords("deps/stopwords.txt");

  size_t added = 0;
  std::string line;

  while(std::getline(std::cin, line)) {
    added += indexer.add_line(line);
  }

  std::cerr << added << std::endl;

  indexer.save(argv[1]);

  return 0;
}
