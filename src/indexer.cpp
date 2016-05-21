#include <iostream>
#include <sstream>
#include <tuple>

#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/serialization/serialization.hpp>

#include "common.hpp"
#include "indexer.hpp"

namespace ir
{
  namespace indexer
  {

    template<class Archive>
    void posting::serialize(Archive & ar, const unsigned int version)
    {
        ar & key;
        ar & group_id;
        ar & post_id;
        ar & pos;
    }

    bool operator < (const posting& x, const posting& y)
    {
      return x.key < y.key;
    }

    bool full_cmp::operator()(const posting& x, const posting& y) const
    {
      return (std::tie(x.key, x.group_id, x.post_id, x.pos) <
              std::tie(y.key, y.group_id, y.post_id, y.pos));
    }

    std::ostream& operator << (std::ostream& os, const posting& p)
    {
      os << boost::format("key = %lu, group_id = %d, post_id = %d, pos = %d")
        % p.key 
        % p.group_id
        % p.post_id
        % p.pos;
      return os;
    }

    // 1. parse json
    // 2. skip trash
    // 3. tokenize text
    // 4. normalize tokens (upper + stem)
    int indexer::add_line(const std::string& raw_line)
    {
      std::vector<std::string> line_parts;
      boost::split(line_parts, raw_line, boost::is_any_of("\t"));
      if(line_parts.size() != 2)
        return 0;

      int group_id = -1 * boost::lexical_cast<int>(line_parts[0]);
      std::wstringstream json;
      json.str(common::bytes_to_wide(line_parts[1]));

      boost::property_tree::wptree tree;
      boost::property_tree::read_json(json, tree);

      int added = 0;

      for(auto& item : tree.get_child(L"response")) {
        boost::optional<std::wstring> type =
          item.second.get_optional<std::wstring>(L"post_type");

        if(!type || *type != L"post")
            continue;

        boost::optional<std::wstring> text =
          item.second.get_optional<std::wstring>(L"text");

        if(!text || (*text).empty())
          continue;

        boost::optional<int> id = item.second.get_optional<int>(L"id");
        if(!id)
            continue;

        std::vector<std::wstring> terms = common::make_terms(*text);
        int n = terms.size();
        for(int i = 0; i < n; i++) {
          size_t term_hash = common::murmur(terms[i]);
          // skip stopwords
          if(std::binary_search(stopwords_.begin(), stopwords_.end(),
                                term_hash))
            continue;

          posting cur_pos(term_hash, group_id, *id, i);
          index_.push_back(cur_pos);
          added++;
        }
      }

      return added;

    }

    void indexer::save(std::ostream& os)
    {
      std::sort(index_.begin(), index_.end(), full_cmp());
      boost::archive::text_oarchive oarch(os);
      oarch << index_;
    }

    void indexer::load(std::istream& is)
    {
      boost::archive::text_iarchive iarch(is);
      iarch >> index_;
    }

    void indexer::load_stopwords(const std::string& stopwords)
    {
      std::fstream sw(stopwords);
      std::string line;
      while(std::getline(sw, line)) {
        std::wstring wide_line = common::bytes_to_wide(line);
        std::vector<std::wstring> terms = common::make_terms(wide_line);
        size_t term_hash = common::murmur(terms[0]);
        stopwords_.push_back(term_hash); 
      }
      std::sort(stopwords_.begin(), stopwords_.end());
    }

  }
}
