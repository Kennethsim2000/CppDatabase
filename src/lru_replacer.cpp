#include "lru_replacer.h"

using namespace db;

LRUReplacer::LRUReplacer() : head(nullptr), tail(nullptr)
{
}

LRUReplacer::~LRUReplacer()
{
    ListNode *curr = head;
    while (curr != tail)
    {
        ListNode *nextNode = curr->next_;
        delete curr;
        curr = nextNode;
    }
}

std::optional<size_t> LRUReplacer::evict()
{
    if (head == nullptr) // empty list
    {
        return std::nullopt;
    }
    else
    {
        size_t frame = head->frame_id_;
        remove(frame);
        return frame;
    }
}

// Used to add all the free frames to the LRUReplacer(that we can evict)
void LRUReplacer::add(size_t frame_id)
{
    if (node_map_.find(frame_id) != node_map_.end())
    {
        // node already exist, need to move it to the tail
        ListNode *frameNode = node_map_[frame_id];
        if (frameNode != tail)
        {
            adjustLinks(frameNode);
            addToTail(frameNode);
        }
    }
    else
    {
        // node does not exist, new frameId
        ListNode *node = new ListNode();
        addToTail(node);
    }
}

// This method is mainly for adjusting links, when we remove this node, we need to link
// the previous node to the next node
void LRUReplacer::adjustLinks(ListNode *node)
{
    if (node == head) // single element
    {
        ListNode *nextNode = node->next_;
        node->next_ = nullptr;
        nextNode->prev_ = nullptr;
        head = nextNode;
    }
    else
    {
        ListNode *prevNode = node->prev_;
        ListNode *nextNode = node->next_;
        prevNode->next_ = nextNode;
        nextNode->prev_ = prevNode;
    }
}

void LRUReplacer::addToTail(ListNode *node)
{
}

void LRUReplacer::remove(size_t frame_id)
{
    ListNode *node = node_map_[frame_id];
    node_map_.erase(frame_id);
    adjustLinks(node);
    delete node; // we no longer need this node, safe to free node
}

size_t LRUReplacer::size()
{
    return node_map_.size();
}