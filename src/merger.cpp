#include <utility>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "merger.hpp"
#include "indexer.hpp"
#include "searcher.hpp"

namespace ir
{
  namespace merger
  {

    void merger::add_index(const std::string& filename)
    {
      indexes_.push_back(indexer::indexer());
      indexes_.back().load(filename);

      records_cnt_ += indexes_.back().mapped_index_ptr_->size();
    }

    void merger::merge(const std::string& dst)
    {
      const size_t magic = 8192;
      const size_t record_size = sizeof((*indexes_[0].mapped_index_ptr_)[0]);
      size_t file_size = record_size * records_cnt_ + magic;

      boost::filesystem::remove(dst);
      mapped_file_ptr_.reset(new indexer::mapped_file(bi::open_or_create,
                                                      dst.c_str(), file_size));

      merged_index_ptr_ = mapped_file_ptr_->construct<indexer::mapped_vector>("index")
                            (indexer::posting_alloc(mapped_file_ptr_->get_segment_manager()));
      merged_index_ptr_->reserve(records_cnt_);

      std::vector<searcher::pos_range> ranges;
      ranges.reserve(indexes_.size());

      for(size_t i = 0; i < indexes_.size(); i++)
        ranges.push_back(std::make_pair(indexes_[i].mapped_index_ptr_->begin(),
                                        indexes_[i].mapped_index_ptr_->end()));

      searcher::merge_n_impl(ranges, *merged_index_ptr_);
    }

  }
}
