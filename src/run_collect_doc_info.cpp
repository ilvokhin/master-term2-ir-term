#include <parallel/algorithm>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <tuple>
#include <map>
#include <set>

#include "common.hpp"
#include "indexer.hpp"
#include "doc_info.hpp"

size_t calc_doc_id_hash(const ir::indexer::posting& x)
{
  std::tuple<int, int> doc_id = ir::indexer::doc_id(x);
  int arr[2] = {std::get<0>(doc_id), std::get<1>(doc_id)};

  return ir::common::murmur((void*) arr, sizeof(arr));
}

struct hashed_doc_id_cmp
{
  hashed_doc_id_cmp(ir::indexer::mapped_vector* mapped_index_ptr_):
    mapped_index_ptr_(mapped_index_ptr_)
  { }

  bool operator()(const size_t& x, const size_t& y) const
  {
    return calc_doc_id_hash((*mapped_index_ptr_)[x]) < calc_doc_id_hash((*mapped_index_ptr_)[y]);
  }

  ir::indexer::mapped_vector* mapped_index_ptr_;
};

int main(int argc, char* argv[])
{
  std::cin.sync_with_stdio(false);
  std::cout.sync_with_stdio(false);

  if(argc == 1) {
    std::cerr << "Where I should save your doc-info, dude?" << std::endl;
    return 1;
  }

  ir::doc_info::doc_info_storage storage;

  std::string line;
  size_t sum_doc_size = 0;
  size_t docs_cnt = 0;
  
  while(std::getline(std::cin, line)) {
    ir::indexer::indexer indexer;
    indexer.load(line);

    std::vector<size_t> order;
    order.reserve(indexer.mapped_index_ptr_->size());

    for(size_t i = 0; i < indexer.mapped_index_ptr_->size(); i++)
      order.push_back(i);
    
    // looks like boost interprocess not thread safe, so may ruin everything
    __gnu_parallel::sort(order.begin(), order.end(), hashed_doc_id_cmp(indexer.mapped_index_ptr_));

    size_t prev_hash = 0;
    size_t doc_size = 0;
    for(size_t i = 0; i < order.size(); i++) {
      auto record = (*indexer.mapped_index_ptr_)[i];
      size_t cur_hash = calc_doc_id_hash(record);
      if(cur_hash != prev_hash) {
        storage.add_record(ir::doc_info::doc_info(cur_hash, doc_size));
        sum_doc_size += doc_size;
        docs_cnt++;
        doc_size = 0;
      }

      doc_size++;
      prev_hash = cur_hash;
    }
    storage.add_record(ir::doc_info::doc_info(prev_hash, doc_size));
  }

  storage.save(argv[1]);

  std::cout << "avg: " << 1.0 * sum_doc_size / docs_cnt << std::endl;

  return 0;
}
