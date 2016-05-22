#include <iostream>
#include <string>

#include "common.hpp"
#include "idf_table.hpp"

int main(int argc, char* argv[])
{
  std::cin.sync_with_stdio(false);
  std::cout.sync_with_stdio(false);

  if(argc == 1) {
    std::cerr << "Where is your index, dude?" << std::endl;
    return 1;
  }

  ir::idf_table::idf_table table;
  table.load(argv[1]);

  std::string line;
  while(std::getline(std::cin, line)) {
    std::wstring wide_line = ir::common::bytes_to_wide(line);
    std::vector<std::wstring> terms = ir::common::make_terms(wide_line);

    for(size_t i = 0; i < terms.size(); i++) {
      std::cout << ir::common::wide_to_bytes(terms[i])
        << "\t"
        << table.get_idf(ir::common::murmur(terms[i]))
        << std::endl;
    }

  }

  return 0;
}
