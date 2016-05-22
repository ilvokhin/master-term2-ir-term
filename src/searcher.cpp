#include <algorithm>
#include <utility>
#include <fstream>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "common.hpp"
#include "searcher.hpp"

namespace ir
{

  namespace searcher
  {
    bool operator < (const merge_iter& x, const merge_iter& y)
    {
      return indexer::full_cmp()(*(x.iter), *(y.iter));
    }

    bool operator > (const merge_iter& x, const merge_iter& y)
    {
      return indexer::full_cmp()(*(y.iter), *(x.iter));
    }

    std::vector<indexer::posting>
        merge_n(const std::vector<pos_range>& ranges)
    {
      std::vector<indexer::posting> out;
      merge_n_impl(ranges, out);
      return out;
    }

    std::vector<indexer::posting>
      intersect_two(const pos_range& x_begin,
                    const pos_range& x_end,
                    const pos_range& y_begin,
                    const pos_range& y_end)
    {
      // TODO
      return std::vector<indexer::posting>();
    }

    std::vector<indexer::posting>
      intersect_n(const std::vector<pos_range>& ranges)
    {
      // TODO
      return std::vector<indexer::posting>();
    }

    boost::property_tree::ptree make_json(const indexer::posting& p)
    {
      boost::property_tree::ptree tree;
      tree.put("key", boost::lexical_cast<std::string>(p.key));
      tree.put("group_id", boost::lexical_cast<std::string>(p.group_id));
      tree.put("post_id", boost::lexical_cast<std::string>(p.post_id));
      tree.put("pos", boost::lexical_cast<std::string>(p.pos));
      return tree;
    }

    searcher::searcher(const std::string& filename)
    {
      indexer_.load(filename);
    }

    std::vector<indexer::posting>
        searcher::handle_query(const std::vector<std::wstring>& terms) const
    {
      std::vector<pos_range> postings;
      postings.reserve(terms.size());

      for(size_t i = 0; i < terms.size(); i++)
        postings.push_back(calc_postings(terms[i]));

      return merge_n(postings);
    }

    std::string searcher::handle_raw_query(const std::string& raw_query) const
    {
      std::wstring wide_query = common::bytes_to_wide(common::parse_raw_query(raw_query));
      if(wide_query.empty())
        return "{}";

      std::vector<std::wstring> terms = common::make_terms(wide_query);
      std::vector<indexer::posting> postings = handle_query(terms);

      return serialize_response(postings);
    }

    std::string
      searcher::serialize_response(const std::vector<indexer::posting>& p) const
    {
      boost::property_tree::ptree tree;
      boost::property_tree::ptree postings;

      for(size_t i = 0; i < p.size(); i++) {
        auto single = make_json(p[i]);
        postings.push_back(std::make_pair("", single));
      }
      
      tree.add_child("postings", postings);
      std::stringstream stream;
      boost::property_tree::write_json(stream, tree, false);
      return stream.str();
    }

    pos_range searcher::calc_postings(const std::wstring& term) const
    {
      size_t term_hash = common::murmur(term);
      indexer::posting term_pos(term_hash);

      return indexer_.calc_postings(term_pos);
    }

  }

}
