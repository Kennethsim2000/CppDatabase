#include <gtest/gtest.h>
#include "lru_replacer.h"

using namespace db;

TEST(LRUReplacerTest, AddIncreasesSize)
{
    LRUReplacer lru(3);
    lru.add(1);
    lru.add(2);
    ASSERT_EQ(lru.size(), 2);
}

TEST(LRUReplacerTest, EvictReturnsOldest)
{
    LRUReplacer lru(3);
    lru.add(1);
    lru.add(2);
    lru.add(3);
    size_t frame;

    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 1);

    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 2);

    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 3);

    ASSERT_FALSE(lru.evict(frame));
}

TEST(LRUReplacerTest, RemoveDeletesFrame)
{
    LRUReplacer lru(3);
    lru.add(1);
    lru.add(2);

    lru.remove(1);
    size_t frame;
    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 2);

    ASSERT_EQ(lru.size(), 0);
}

TEST(LRUReplacerTest, DuplicateAddIgnored)
{
    LRUReplacer lru(3);
    lru.add(1);
    lru.add(1);
    lru.add(1);

    ASSERT_EQ(lru.size(), 1);
    size_t frame;
    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 1);

    ASSERT_FALSE(lru.evict(frame));
}

TEST(LRUReplacerTest, MixedOperations)
{
    LRUReplacer lru(5);

    lru.add(1);
    lru.add(2);
    lru.add(3);

    // simulate pin (remove)
    lru.remove(2);

    lru.add(4);

    size_t frame;

    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 1);

    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 3);

    ASSERT_TRUE(lru.evict(frame));
    ASSERT_EQ(frame, 4);

    ASSERT_FALSE(lru.evict(frame));
}