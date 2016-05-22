#ifndef INDEXER_HPP_
#define INDEXER_HPP_

#include <iostream>
#include <memory>
#include <vector>
#include <tuple>

#include <boost/interprocess/managed_mapped_file.hpp>

#include "common.hpp"

namespace ir
{
  namespace indexer
  {

    namespace bi = boost::interprocess;

    struct posting
    {
      posting(size_t key = 0, int group_id = 0, int post_id = 0, int pos = 0):
        key(key), group_id(group_id), post_id(post_id), pos(pos)
      { }

      size_t key;
      int group_id;
      int post_id;
      int pos;
    };

    bool operator < (const posting& x, const posting& y);

    std::tuple<int, int> doc_id(const posting& p);

    struct full_cmp
    {
      bool operator()(const posting& x, const posting& y) const;
    };

    std::ostream& operator << (std::ostream& os, const posting& p);

    typedef bi::managed_mapped_file mapped_file;
    typedef bi::allocator<posting,
                          mapped_file::segment_manager> posting_alloc;
    typedef std::vector<posting, posting_alloc> mapped_vector;

    struct indexer
    {
      typedef mapped_vector::const_iterator pos_iter;
      typedef std::pair<pos_iter, pos_iter> pos_range;

      int add_line(const std::string& raw_line);

      void save(const std::string& filename);

      void load(const std::string& filename);

      void load_stopwords(const std::string& stopwords);

      pos_range calc_postings(const posting& posting) const;

      std::vector<posting> index_;
      std::vector<size_t> stopwords_;
      // memory mapped index
      std::unique_ptr<mapped_file> mapped_file_ptr_;
      mapped_vector* mapped_index_ptr_;
    };

  }
}

#endif
