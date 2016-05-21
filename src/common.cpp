#include <cctype>
#include <vector>
#include <locale>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "common.hpp"

namespace ir
{
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

    // MurmurHash2, 64-bit versions, by Austin Appleby
    // source: https://sites.google.com/site/murmurhash/
    size_t murmur(const void* key, int len, unsigned int seed)
    {
        const size_t m = 0xc6a4a7935bd1e995;
        const int r = 47;

        size_t h = seed ^ (len * m);

        const size_t * data = (const size_t *)key;
        const size_t * end = data + (len/8);

        while(data != end) {
          size_t k = *data++;

          k *= m; 
          k ^= k >> r; 
          k *= m; 
          
          h ^= k;
          h *= m; 
        }

        const unsigned char * data2 = (const unsigned char*)data;

        switch(len & 7) {
          case 7: h ^= size_t(data2[6]) << 48;
          case 6: h ^= size_t(data2[5]) << 40;
          case 5: h ^= size_t(data2[4]) << 32;
          case 4: h ^= size_t(data2[3]) << 24;
          case 3: h ^= size_t(data2[2]) << 16;
          case 2: h ^= size_t(data2[1]) << 8;
          case 1: h ^= size_t(data2[0]);
                  h *= m;
        };
       
        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return h;
      }

      size_t murmur(const std::wstring& wstr)
      {
        const void* ptr = static_cast<const void*>(wstr.c_str());
        const size_t sz = sizeof(wchar_t) * wstr.size();
        return murmur(ptr, sz);
      }

      std::vector<std::wstring> tokenize(const std::wstring& text)
      {
        std::wstring text_wo_p;
        text_wo_p.reserve(text.size());

        for(size_t i = 0; i < text.size(); i++) {
          if(!iswpunct(text[i]))
            text_wo_p.push_back(text[i]);
          else
            text_wo_p.push_back(' ');
        }

        std::vector<std::wstring> line_parts;
        boost::split(line_parts, text_wo_p, boost::is_any_of(" \t"));
    
        return line_parts;
      }

      std::wstring normalize(const std::wstring& token)
      {
        std::wstring term(token);
        boost::algorithm::trim(term);
        std::setlocale(LC_ALL, "en_US.utf8");
        std::transform(term.begin(), term.end(), term.begin(), towupper);
        return stem_word(term);
      }

      std::vector<std::wstring> make_terms(const std::wstring& text)
      {
        std::vector<std::wstring> tokens = tokenize(text);
        std::vector<std::wstring> terms;
        terms.reserve(tokens.size());

        for(size_t i = 0; i < tokens.size(); i++) {
          terms.push_back(common::normalize(tokens[i]));
          // a bit clean up here
          if(terms.back().empty() || terms.back() == L"BR")
            terms.pop_back();
        }

        return terms;
      }

  }
}
