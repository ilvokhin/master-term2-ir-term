#include <map>
#include <tuple>

#include <boost/lexical_cast.hpp>

#include "ranker.hpp"

namespace ir
{
  namespace searcher
  {

    boost::property_tree::ptree make_json(const doc& doc)
    {
      boost::property_tree::ptree tree;
      tree.put("group_id", boost::lexical_cast<std::string>(doc.group_id));
      tree.put("post_id", boost::lexical_cast<std::string>(doc.post_id));
      tree.put("rank", boost::lexical_cast<std::string>(doc.rank));

      return tree;
    }

    ranker::ranker(const std::string& filename)
    {
      table_.load(filename);
    }

    std::vector<doc>
      ranker::calc_ranked_docs(const std::vector<indexer::posting>& postings) const
    {
      std::map<std::tuple<int, int>, std::vector<indexer::posting> > doc_groups;
      for(size_t i = 0; i < postings.size(); i++)
        doc_groups[indexer::doc_id(postings[i])].push_back(postings[i]);

      std::vector<doc> docs;

      for(auto& kv : doc_groups)
        docs.push_back(calc_score_bm15(kv.second));

      return docs;
    }

    doc ranker::calc_score_bm15(const std::vector<indexer::posting>& postings) const
    {
      static const float k_1 = 2.0f;

      size_t prev_term_id = 0;
      size_t term_cnt = 0;
      doc doc(postings[0].group_id, postings[0].post_id);

      for(size_t i = 0; i < postings.size(); i++) {
        if(postings[i].key != prev_term_id) {
          float term_rank = (term_cnt * (k_1 + 1.0f)) / (term_cnt + k_1);
          doc.rank += table_.get_idf(prev_term_id) * term_rank;
          term_cnt = 0;
        }
        term_cnt++;
        prev_term_id = postings[i].key;
      }

      return doc;
    }

  }
}
