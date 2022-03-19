#pragma once

#include <algorithm>
#include <cstddef>
#include <list>
#include <new>
#include <ostream>

template <class Key, class KeyProvider, class Allocator>
class Cache
{
public:
    template <class... AllocArgs>
    Cache(const std::size_t cache_size, AllocArgs &&... alloc_args)
        : m_max_size(cache_size)
        , m_alloc(std::forward<AllocArgs>(alloc_args)...)
    {
    }

    std::size_t size() const
    {
        return queue.size();
    }

    bool empty() const
    {
        return queue.empty();
    }

    template <class T>
    T & get(const Key & key);

    std::ostream & print(std::ostream & strm) const;

    friend std::ostream & operator<<(std::ostream & strm, const Cache & cache)
    {
        return cache.print(strm);
    }

private:
    const std::size_t m_max_size;
    Allocator m_alloc;
    std::list<std::pair<KeyProvider *, bool>> queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T & Cache<Key, KeyProvider, Allocator>::get(const Key & key)
{
    auto it = std::find_if(queue.begin(), queue.end(), [&key](const std::pair<KeyProvider *, bool> elem) {
        return *elem.first == key;
    });
    if (it != queue.end()) {
        it->second = true;
        return *(static_cast<T *>(it->first));
    }

    if (m_max_size == queue.size()) {
        while (queue.back().second) {
            const auto t = queue.back().first;
            queue.pop_back();
            queue.push_front({t, false});
        }
        m_alloc.destroy(queue.back().first);
        queue.pop_back();
    }
    queue.push_front({m_alloc.template create<T>(key), false});
    return *(static_cast<T *>(queue.front().first));
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream & Cache<Key, KeyProvider, Allocator>::print(std::ostream & strm) const
{
    for (const auto & x : queue) {
        strm << *x.first << ' ' << x.second << std::endl;
    }
    return strm;
}
