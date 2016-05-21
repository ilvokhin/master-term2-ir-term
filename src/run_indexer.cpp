#include <iostream>

#include "indexer.hpp"

int main()
{
  std::cin.sync_with_stdio(false);
  std::cout.sync_with_stdio(false);

  ir::indexer::indexer indexer;

  indexer.load_stopwords("deps/stopwords.txt");

  size_t added = 0;
  std::string line;

  while(std::getline(std::cin, line)) {
    added += indexer.add_line(line);
  }

  std::cerr << added << std::endl;

  indexer.save(std::cout);

  return 0;
}
