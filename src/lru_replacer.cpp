#include "lru_replacer.h"

using namespace db;

LRUReplacer::LRUReplacer(size_t capacity)
{
}

// pin a frame, remove from the LRU
void LRUReplacer::remove(size_t frame_id)
{
    auto it = map_.find(frame_id);
    if (it == map_.end())
    {
        return;
    }
    lru_.erase(it->second);
    map_.erase(it);
}

//
void LRUReplacer::add(size_t frame_id)
{
    if (map_.count(frame_id))
    { // frame_id exists in the LRUReplacer
        return;
    }
    lru_.push_back(frame_id);
    auto it = std::prev(lru_.end());
    map_.emplace(frame_id, it);
}

bool LRUReplacer::evict(size_t &frame_id)
{
    if (lru_.empty())
    {
        return false;
    }
    frame_id = lru_.front();
    lru_.pop_front();
    map_.erase(frame_id);
    return true;
}

size_t LRUReplacer::size()
{
    return map_.size();
}

/*

Remove method is meant to remove the ListNode entirely from the LRUReplacer(not just adjusting it to the end).
It is meant for evicting a page(to be used in the page table), so in this case, our ListNode* can be deleted.

Adjust Links need to consider another pattern(if head == tail, meaning one element, we need to set head and tail to nullptr, else if node == head(first element), then we ened to shift the head pointer, else we do the classic assign next to prev and prev to next)

addToTail would just be if head == nullptr, set head and tail to the node, else we will obtain the tail, and assign the next and prev pointer, and update tail

why do we need LRU
our buffer pool has a fixed number of frames:

pages = [ frame0, frame1, frame2, frame3 ]Each frame may contain a page from disk.
If the pool is full and a new page must be loaded:

fetch_page(new_page)you must evict an existing page.

But you cannot evict pages that are pinned.
So the system must track:
which frames are evictable
among them, which one is least recently used

This is what LRUReplacer does.*/