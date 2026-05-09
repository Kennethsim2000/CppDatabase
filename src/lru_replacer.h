#include <cstddef>
#include <unordered_map>
#include <optional>
#include <list>

namespace db
{
    class LRUReplacer // stores frames that are currently evictable(pin count = 0)
    {
    public:
        explicit LRUReplacer(size_t capacity);
        // Remove the least recently used frame — return the frame id
        bool evict(size_t &frame_id);

        // Remove frame from LRU-Replacer(when a page gets pinned again)
        void remove(size_t frame_id);

        // When pin count becomes 0, frame becomes evictable, so we are adding to LRUReplacer
        void add(size_t frame_id);

        // How many frames are currently evictable?
        size_t size();

    private:
        std::unordered_map<size_t, std::list<size_t>::iterator> map_;
        std::list<size_t> lru_;
    };
}

/*LRU list: front = oldest, back = newest
frame:3, 7, 1,
3 = evict first
1 = most recently unpinned

pinned pages cannot be evicted, therefore pinned pages should not exist in the LRU

when a page becomes pinned, we should remove frame from replacer, because it is no longer evictable
 */