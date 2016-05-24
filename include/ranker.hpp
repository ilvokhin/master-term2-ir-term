#ifndef RANKER_HPP_
#define RANKER_HPP_

#include <vector>

#include "indexer.hpp"
#include "idf_table.hpp"

#include <boost/property_tree/ptree.hpp>

namespace ir
{
  namespace searcher
  {
    struct doc
    {
      doc(int group_id = 0, int post_id = 0, float rank = 0.0f):
        group_id(group_id), post_id(post_id), rank(rank)
      { }

      int group_id;
      int post_id;
      float rank;
    };

    bool operator < (const doc& x, const doc& y);

    boost::property_tree::ptree  make_json(const doc& doc);

    class ranker
    {
      public:
        ranker(const std::string& filename);

        std::vector<doc>
          calc_ranked_docs(const std::vector<indexer::posting>& postings) const;

      private:
        doc calc_score_bm25(const std::vector<indexer::posting>& postings) const;

        idf_table::idf_table table_;
    };
  }
}

#endif
