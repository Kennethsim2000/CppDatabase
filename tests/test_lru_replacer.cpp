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