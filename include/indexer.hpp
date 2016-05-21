#ifndef INDEXER_HPP_
#define INDEXER_HPP_

#include <iostream>
#include <vector>

#include "common.hpp"


namespace ir
{
  namespace indexer
  {

    struct posting
    {
      posting(size_t key = 0, int group_id = 0, int post_id = 0, int pos = 0):
        key(key), group_id(group_id), post_id(post_id), pos(pos)
      { }

      template<class Archive>
      void serialize(Archive & ar, const unsigned int version);

      size_t key;
      int group_id;
      int post_id;
      int pos;
    };

    bool operator < (const posting& x, const posting& y);

    std::ostream& operator << (std::ostream& os, const posting& p);

    struct indexer
    {

        int add_line(const std::string& raw_line);

        void save(std::ostream& os);

        void load(std::istream& is);

        void load_stopwords(const std::string& stopwords);

        std::vector<posting> index_;
        std::vector<size_t> stopwords_;
    };

  }
}

#endif
