#include "pool.h"

#include <algorithm>
#include <assert.h>

PoolAllocator::PoolAllocator(const std::size_t block_size, std::initializer_list<std::size_t> sizes)
    : block_size(block_size)
    , sizes(sizes)
    , storage(block_size * sizes.size())
    , used(sizes.size())
{
    size_t index = 0;
    std::sort(this->sizes.begin(), this->sizes.end());
    for (const auto elem_size : sizes) {
        assert(elem_size != 0);
        used[index++].resize(block_size / elem_size);
    }
}

void * PoolAllocator::allocate(const std::size_t size)
{
    auto lower = std::lower_bound(sizes.begin(), sizes.end(), size);
    if (lower == sizes.end() || *lower != size) {
        throw std::bad_alloc{};
    }
    size_t block = lower - sizes.begin();
    const auto it = std::find(used[block].begin(), used[block].end(), 0);
    if (it != used[block].end()) {
        const size_t pos = it - used[block].begin();
        used[block][pos] = true;
        return &storage[block * block_size + pos * size];
    }
    throw std::bad_alloc{};
}

void PoolAllocator::deallocate(const void * ptr)
{
    const auto b_ptr = static_cast<const std::byte *>(ptr);
    const auto begin = &storage[0];
    if (b_ptr >= begin) {
        const size_t block = (b_ptr - begin) / block_size;
        const size_t obj_size = sizes[block];
        const size_t slab_offset = (b_ptr - begin - block * block_size);
        const size_t offset = slab_offset / obj_size;
        assert((slab_offset % obj_size) == 0);
        if (offset < used[block].size()) {
            used[block][offset] = false;
        }
    }
}
