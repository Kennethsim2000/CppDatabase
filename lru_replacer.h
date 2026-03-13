#include <cstddef>
#include <unordered_map>
#include <optional>

namespace db
{
    struct ListNode
    {
        ListNode *next_;
        ListNode *prev_;
        size_t frame_id_;
    };

    class LRUReplacer // stores frames that are currently evictable(pin count = 0)
    {
    public:
        LRUReplacer();

        ~LRUReplacer();
        // Remove the least recently used frame — return the frame id
        std::optional<size_t> evict();
        // Add a frame to the tracker (it became unpinned)
        void add(size_t frame_id);
        // Remove a frame from the tracker (it got pinned again)
        void remove(size_t frame_id);
        // How many frames are currently evictable?
        size_t size();

    private:
        std::unordered_map<size_t, ListNode *> node_map_;
        ListNode *head;
        ListNode *tail;

        void adjustLinks(ListNode *node);
        void addToTail(ListNode *node);
    };
}
