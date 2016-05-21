#include <string>

#include <boost/locale/encoding_utf.hpp>

#include "stemming/russian_stem.h"

namespace common
{

  std::wstring stem_word(const std::wstring& raw_word);

  std::wstring bytes_to_wide(const std::string& str);

  std::string wide_to_bytes(const std::wstring& wstr);

}
