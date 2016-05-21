#include "common.hpp"

namespace common
{

  std::wstring stem_word(const std::wstring& raw_word)
  {
    static stemming::russian_stem<> ru_stem;
    std::wstring word(raw_word);
    ru_stem(word);
    return word; 
  }

  std::wstring bytes_to_wide(const std::string& str)
  {
    return boost::locale::conv::utf_to_utf<wchar_t>(str.c_str(),
                                                    str.c_str() + str.size());
  }

  std::string wide_to_bytes(const std::wstring& wstr)
  {
    return boost::locale::conv::utf_to_utf<char>(wstr.c_str(),
                                                 wstr.c_str() + wstr.size());
  }

}
