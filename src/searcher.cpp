#include <algorithm>
#include <utility>
#include <fstream>
#include <queue>

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

    searcher::searcher(const std::string& filename)
    {
      std::fstream file_stream(filename);
      indexer_.load(file_stream);
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

    pos_range searcher::calc_postings(const std::wstring& term) const
    {
      size_t term_hash = common::murmur(term);
      indexer::posting term_pos(term_hash);

      return std::equal_range(indexer_.index_.begin(),
                              indexer_.index_.end(), term_pos);
    }

    std::vector<indexer::posting>
      searcher::intersect_n(const std::vector<pos_range>& ranges) const
    {
      std::priority_queue<merge_iter> pq;
      std::vector<indexer::posting> out;

      for(size_t i = 0; i < ranges.size(); i++) {
        if(ranges[i].first == ranges[i].second)
          return out;

        pq.push(merge_iter(ranges[i].first, i));
      }

      while(!pq.empty()) {
        auto p_pos = pq.top();
        pq.pop();

        auto& iter = p_pos.iter;
        int from = p_pos.from;

        out.push_back(*iter);
        iter++;
        if(iter != ranges[from].second)
          pq.push(merge_iter(iter, from));
      }

      return out;
    }

  }

}
