#include <iostream>
#include <string>
#include <vector>

#include "common.hpp"
#include "indexer.hpp"
#include "searcher.hpp"

int main(int argc, char* argv[])
{
  std::cin.sync_with_stdio(false);
  std::cout.sync_with_stdio(false);

  if(argc <= 2) {
    std::cerr << "Where is your index and idf table, dude?" << std::endl;
    return 1;
  }

  ir::searcher::searcher searcher(argv[1], argv[2]);

  std::string line;

  while(std::getline(std::cin, line)) {
    std::string response = searcher.handle_raw_query(line, true);
    std::cout << response << std::endl;
  }

  return 0;
}
