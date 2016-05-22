#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <string>
#include <utility>

#include <boost/locale/encoding_utf.hpp>

#include "stemming/russian_stem.h"

namespace ir
{
  namespace common
  {
    std::wstring stem_word(const std::wstring& raw_word);

    std::wstring bytes_to_wide(const std::string& str);

    std::string wide_to_bytes(const std::wstring& wstr);

    size_t murmur(const void* key, int len, unsigned int seed = 0);

    size_t murmur(const std::wstring& wstr);

    std::vector<std::wstring> tokenize(const std::wstring& text);

    std::wstring normalize(const std::wstring& token);

    std::vector<std::wstring> make_terms(const std::wstring& text);

    std::string parse_raw_query(const std::string& raw_query);
  }
}

#endif
