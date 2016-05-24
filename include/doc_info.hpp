#ifndef DOC_INFO_HPP_
#define DOC_INFO_HPP_

#include <vector>

#include <boost/filesystem.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>

namespace ir
{
  namespace doc_info
  {
    namespace bi = boost::interprocess;

    struct doc_info
    {
      doc_info(size_t key = 0, size_t size = 0):
        key(key), size(size)
      { }

      size_t key;
      size_t size;
    };

    bool operator < (const doc_info& x, const doc_info& y);

    typedef bi::managed_mapped_file mapped_file;
    typedef bi::allocator<doc_info,
                          mapped_file::segment_manager> doc_info_alloc;
    typedef std::vector<doc_info, doc_info_alloc> mapped_vector;

    class doc_info_storage
    {
      public:
        void add_record(const doc_info& di);
        void save(const std::string& filename);
        void load(const std::string& filename);

        size_t get_size(size_t key) const;
      private:
        std::vector<doc_info> records_;

        std::unique_ptr<mapped_file> mapped_file_ptr_;
        mapped_vector* mapped_table_ptr_;
    };

  }
}

#endif
