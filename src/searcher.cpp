#include <algorithm>
#include <utility>
#include <fstream>

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

    pos_range searcher::calc_postings(const std::wstring& term) const
    {
      size_t term_hash = common::murmur(term);
      indexer::posting term_pos(term_hash);

      return indexer_.calc_postings(term_pos);
    }

  }

}
