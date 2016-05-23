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

  if(argc == 2) {
    std::cerr << "Where is your index and idf table, dude?" << std::endl;
    return 1;
  }

  ir::searcher::searcher searcher(argv[1], argv[2]);

  std::string line;

  while(std::getline(std::cin, line)) {
    std::wstring wide_line = ir::common::bytes_to_wide(line);
    std::vector<std::wstring> terms = ir::common::make_terms(wide_line);

    for(size_t i = 0; i < terms.size(); i++)
      std::cout << ir::common::wide_to_bytes(terms[i]) << std::endl;

    std::vector<ir::indexer::posting> postings = searcher.handle_query(terms);
  
    for(auto& posting : postings) {
      std::cout << posting << std::endl;
    }
  }

  return 0;
}
