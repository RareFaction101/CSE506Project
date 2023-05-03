#include "Cache.h"

Cache::Cache(){
    this->cacheSize = 16;
    this->blockSize = 4;

    // Resize the outer vector (rows)
    this->caches.resize(4);
    for (auto& cache : caches) {
        cache.resize(6);
    }

    // Loop through each row and resize the inner vector (columns)
    for (size_t i = 0; i < this->caches.size(); ++i) {
        this->caches[i][0] = 'I';
        std::fill(this->caches[i].begin() + 1, this->caches[i].end(), '0');
    }
}