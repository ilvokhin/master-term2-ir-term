#include <algorithm>

#include "idf_table.hpp"

namespace ir
{

  namespace idf_table
  {

    bool operator < (const idf_record& x, const idf_record& y)
    {
      return x.term < y.term;
    }

    void idf_table::add_record(const idf_record& record)
    {
      records_.push_back(record);
    }

    void idf_table::save(const std::string& filename)
    {
      const size_t magic = 8192;
      size_t file_size = sizeof(records_[0]) * records_.size() + magic;

      boost::filesystem::remove(filename);
      mapped_file_ptr_.reset(new mapped_file(bi::open_or_create,
                                            filename.c_str(), file_size));
      mapped_table_ptr_ = mapped_file_ptr_->construct<mapped_vector>("table")
                          (records_.begin(), records_.end(),
                           idf_record_alloc(mapped_file_ptr_->get_segment_manager()));
    }

    void idf_table::load(const std::string& filename)
    {
      mapped_file_ptr_.reset(new mapped_file(bi::open_read_only, filename.c_str()));
      mapped_table_ptr_ = mapped_file_ptr_->find<mapped_vector>("table").first;
    }

    float idf_table::get_idf(size_t term) const
    {
      idf_record record(term);
      auto range = std::equal_range(mapped_table_ptr_->begin(),
                                    mapped_table_ptr_->end(), record);

      if(range.first == range.second)
        return 0.0;

      return (*range.first).idf;
    }

  }

}
