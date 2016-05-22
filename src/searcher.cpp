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
      intersect_two_impl(const Iter1& x_begin, const Iter1& x_end,
                         const Iter2& y_begin, const Iter2& y_end,
                         size_t window_size)
    {
      using indexer::doc_id;
      std::vector<indexer::posting> out;

      Iter1 x_cur = x_begin;
      Iter2 y_cur = y_begin;

      while(x_cur != x_end && y_cur != y_end) {
        if(doc_id(*x_cur) == doc_id(*y_cur)) {
          std::vector<indexer::posting> cur_out;
          auto cur_doc_id = doc_id(*x_cur);
          while(doc_id(*x_cur) == cur_doc_id) {
            while(doc_id(*y_cur) == cur_doc_id) {
              int y_cur_pos = (*y_cur).pos;
              if(std::abs(x_cur->pos - y_cur_pos) <= window_size)
                cur_out.push_back(*y_cur);
              else if(y_cur_pos > x_cur->pos)
                break;
              y_cur++;
            }
            while(!cur_out.empty() &&
                  std::abs(x_cur->pos - cur_out.back().pos) > window_size)
              cur_out.pop_back();

              if(!cur_out.empty()) {
                out.push_back(*x_cur);
                for(auto & elem : cur_out)
                  out.push_back(elem);
              }

            x_cur++;
          }
        } else if(doc_id(*x_cur) < doc_id(*y_cur)) {
            x_cur++;
        } else {
            y_cur++;
        }
      }

        return out;
      }

    std::vector<indexer::posting>
      intersect_n(const std::vector<pos_range>& ranges)
    {
      const size_t window_size = 2 * ranges.size();
      std::vector<size_t> order;
      order.reserve(ranges.size());

      for(size_t i = 0; i < ranges.size(); i++)
        if(ranges[i].first != ranges[i].second)
          order.push_back(i);

      if(order.empty())
        std::vector<indexer::posting>();

      std::sort(order.begin(), order.end(),
                [&](const size_t& x, const size_t& y)
                {
                  return std::distance(ranges[x].first, ranges[y].second)
                    < std::distance(ranges[y].first, ranges[y].second);
                });

      std::vector<indexer::posting> out(ranges[order[0]].first,
                                        ranges[order[0]].second);

      for(size_t i = 1; i < order.size(); i++) {
        size_t pos = order[i];
        out = intersect_two_impl(out.begin(), out.end(),
                                 ranges[pos].first, ranges[pos].second,
                                 window_size);
        if(out.empty())
          break;
      }

      return out;
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

      return intersect_n(postings);
    }

    std::string searcher::handle_raw_query(const std::string& raw_query) const
    {
      std::wstring wide_query = common::parse_raw_query(common::bytes_to_wide(raw_query));
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
