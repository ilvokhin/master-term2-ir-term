#ifndef SEARCHER_HPP_
#define SEARCHER_HPP_

#include <vector>
#include <utility>

#include "common.hpp"
#include "indexer.hpp"

namespace ir
{
  namespace searcher
  {

      typedef std::vector<indexer::posting>::const_iterator pos_iter;
      typedef std::pair<pos_iter, pos_iter> pos_range;

      struct merge_iter
      {
          merge_iter(const pos_iter& iter, int from):
            iter(iter), from(from)
          { }

          pos_iter iter;
          int from;
      };

      bool operator < (const merge_iter& x, const merge_iter& y);

      class searcher
      {
        public:

          searcher(const std::string& filename);

          std::vector<indexer::posting>
            handle_query(const std::vector<std::wstring>& terms) const;

        private:
          pos_range calc_postings(const std::wstring& term) const;

          std::vector<indexer::posting>
            intersect_n(const std::vector<pos_range>& ranges) const;

          indexer::indexer indexer_;
      };

  }
}

#endif
