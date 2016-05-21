#include <iostream>

#include "common.hpp"

int main()
{
  std::string raw_word;

  while(std::cin >> raw_word) {
    std::wstring wide_raw_word = common::bytes_to_wide(raw_word);
    std::wstring stemmed = common::stem_word(wide_raw_word);

    std::cout << raw_word << '\t' << common::wide_to_bytes(stemmed) << std::endl;
    std::cout << "OK" << std::endl;
  }

  return 0;
}
