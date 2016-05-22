#ifndef MERGER_HPP_
#define MERGER_HPP_

#include <vector>
#include <string>

#include "indexer.hpp"
#include "searcher.hpp"

namespace ir
{
  namespace merger
  {

    namespace bi = boost::interprocess;

    class merger
    {
      public:
        merger():
          records_cnt_(0)
        { }

        void add_index(const std::string& filename);
        void merge(const std::string& dst);

      private:
        size_t records_cnt_;
        std::vector<indexer::indexer> indexes_;

        std::unique_ptr<indexer::mapped_file> mapped_file_ptr_;
        indexer::mapped_vector* merged_index_ptr_;
    };

  }
}

#endif
