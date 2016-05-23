#include <iostream>
#include <cmath>
#include <tuple>
#include <map>
#include <set>

#include "common.hpp"
#include "indexer.hpp"
#include "idf_table.hpp"


int main(int argc, char* argv[])
{
  std::cin.sync_with_stdio(false);
  std::cout.sync_with_stdio(false);

  if(argc == 1) {
    std::cerr << "Where I should save your idf-table, dude?" << std::endl;
    return 1;
  }

  std::map<size_t, size_t> in_doc_cnt;
  std::set<size_t> docs;

  std::string line;
  while(std::getline(std::cin, line)) {
    ir::indexer::indexer indexer;
    indexer.load(line);

    size_t prev_term = 0;
    size_t doc_freq = 0;
    std::tuple<int, int> prev_doc_id = std::tuple<int, int>(-1, -1);


    for(size_t i = 0; i < indexer.mapped_index_ptr_->size(); i++) {
      auto& p = (*indexer.mapped_index_ptr_)[i];
      if(p.key != prev_term) {
        in_doc_cnt[prev_term] += doc_freq;
        doc_freq = 0;
      }

      if(prev_doc_id != ir::indexer::doc_id(p)) {
        doc_freq++;
        auto cur_doc_id = ir::indexer::doc_id(p);
        int arr[2] = {std::get<0>(cur_doc_id), std::get<1>(cur_doc_id)};
        size_t cur_hash = ir::common::murmur((void*) arr, sizeof(arr));
        docs.insert(cur_hash);
      }

      prev_term = p.key;
      prev_doc_id = ir::indexer::doc_id(p);
    }

    size_t docs_cnt = docs.size();

    std::cerr << "docs: " << docs_cnt << std::endl;
    std::cerr << "terms: " << in_doc_cnt.size() << std::endl;

    // save idf table
    ir::idf_table::idf_table table;
    for(auto& kv : in_doc_cnt) {
      ir::idf_table::idf_record r(kv.first);
      size_t freq = kv.second;
      r.idf = std::log2((docs_cnt - freq + 0.5f) / (freq + 0.5f));
      table.add_record(r);
    }

    table.save(argv[1]);
  }

  return 0;
}
