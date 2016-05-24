#include "doc_info.hpp"

namespace ir
{
  namespace doc_info
  {

    bool operator < (const doc_info& x, const doc_info& y)
    {
      return x.key < y.key;
    }

    void doc_info_storage::add_record(const doc_info& di)
    {
      records_.push_back(di);
    }

    void doc_info_storage::save(const std::string& filename)
    {
      const size_t magic = 8192;
      size_t file_size = sizeof(records_[0]) * records_.size() + magic;

      boost::filesystem::remove(filename);
      mapped_file_ptr_.reset(new mapped_file(bi::open_or_create,
                                            filename.c_str(), file_size));
      mapped_table_ptr_ = mapped_file_ptr_->construct<mapped_vector>("table")
                          (records_.begin(), records_.end(),
                           doc_info_alloc(mapped_file_ptr_->get_segment_manager()));
    }

    void doc_info_storage::load(const std::string& filename)
    {
      mapped_file_ptr_.reset(new mapped_file(bi::open_read_only, filename.c_str()));
      mapped_table_ptr_ = mapped_file_ptr_->find<mapped_vector>("table").first;
    }
    
    size_t doc_info_storage::get_size(size_t key) const
    {
      doc_info record(key);
      auto range = std::equal_range(mapped_table_ptr_->begin(),
                                    mapped_table_ptr_->end(), record);

      if(range.first == range.second)
        return 0;

      return (*range.first).size;
    }

  }
}
