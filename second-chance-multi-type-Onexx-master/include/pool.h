#pragma once

#include <cstddef>
#include <initializer_list>
#include <new>
#include <vector>

class PoolAllocator
{
public:
    PoolAllocator(std::size_t block_size, std::initializer_list<std::size_t> sizes);

    void * allocate(std::size_t size);

    void deallocate(const void * ptr);

private:
    const size_t block_size;

    std::vector<std::size_t> sizes;
    std::vector<std::byte> storage;
    std::vector<std::vector<bool>> used;
};
