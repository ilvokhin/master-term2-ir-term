#ifndef IDF_TABLE_HPP_
#define IDF_TABLE_HPP_

#include <vector>

#include <boost/filesystem.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>

namespace ir
{

  namespace idf_table
  {
    namespace bi = boost::interprocess;

    struct idf_record
    {
      idf_record(size_t term = 0, float idf = 0.0f):
        term(term), idf(idf)
      { }

      size_t term;
      float idf;
    };

    bool operator < (const idf_record& x, const idf_record& y);

    typedef bi::managed_mapped_file mapped_file;
    typedef bi::allocator<idf_record,
                          mapped_file::segment_manager> idf_record_alloc;
    typedef std::vector<idf_record, idf_record_alloc> mapped_vector;

    class idf_table
    {
      public:
        void add_record(const idf_record& record);
        void save(const std::string& filename);
        void load(const std::string& filename);
        float get_idf(size_t term) const;
      private:
        std::vector<idf_record> records_;

        std::unique_ptr<mapped_file> mapped_file_ptr_;
        mapped_vector* mapped_table_ptr_;
    };

  }

}

#endif
