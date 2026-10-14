
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include "b_plus_tree.h"

namespace db
{

    static RID make_rid(int key)
    {
        return RID(
            static_cast<PageId>(key + 1000),
            static_cast<uint16_t>(key + 1000));
    }

    static void expect_rid_eq(const RID &expected, const RID &actual)
    {
        EXPECT_EQ(expected.page_id, actual.page_id);
        EXPECT_EQ(expected.slot_num, actual.slot_num);
    }

    TEST(BPlusTreeTest, SearchEmptyTree)
    {
        BPlusTree tree;
        RID result;

        EXPECT_FALSE(tree.search(10, result));
    }

    TEST(BPlusTreeTest, InsertAndSearchSingleKey)
    {
        BPlusTree tree;

        RID expected = make_rid(10);

        EXPECT_TRUE(tree.insert(10, expected));

        RID result;
        EXPECT_TRUE(tree.search(10, result));

        expect_rid_eq(expected, result);
    }

    TEST(BPlusTreeTest, RejectDuplicateKey)
    {
        BPlusTree tree;

        RID original_rid = make_rid(10);
        RID duplicate_rid = make_rid(20);

        EXPECT_TRUE(tree.insert(10, original_rid));
        EXPECT_FALSE(tree.insert(10, duplicate_rid));

        RID result;
        EXPECT_TRUE(tree.search(10, result));

        expect_rid_eq(original_rid, result);
    }

    TEST(BPlusTreeTest, SearchMissingKey)
    {
        BPlusTree tree;

        tree.insert(10, make_rid(10));
        tree.insert(20, make_rid(20));
        tree.insert(30, make_rid(30));

        RID result;

        EXPECT_FALSE(tree.search(15, result));
        EXPECT_FALSE(tree.search(5, result));
        EXPECT_FALSE(tree.search(40, result));
    }

    TEST(BPlusTreeTest, SplitLeafNodes)
    {
        BPlusTree tree(3);

        for (int key = 0; key < 20; ++key)
        {
            EXPECT_TRUE(tree.insert(key, make_rid(key)));
        }

        for (int key = 0; key < 20; ++key)
        {
            RID result;

            ASSERT_TRUE(tree.search(key, result))
                << "Could not find key " << key;

            expect_rid_eq(make_rid(key), result);
        }
    }

    TEST(BPlusTreeTest, SplitInternalNodes)
    {
        BPlusTree tree(3);

        for (int key = 0; key < 500; ++key)
        {
            ASSERT_TRUE(tree.insert(key, make_rid(key)))
                << "Failed to insert key " << key;
        }

        for (int key = 0; key < 500; ++key)
        {
            RID result;

            ASSERT_TRUE(tree.search(key, result))
                << "Could not find key " << key;

            expect_rid_eq(make_rid(key), result);
        }
    }

    TEST(BPlusTreeTest, InsertDescendingOrder)
    {
        BPlusTree tree(3);

        for (int key = 199; key >= 0; --key)
        {
            EXPECT_TRUE(tree.insert(key, make_rid(key)));
        }

        for (int key = 0; key < 200; ++key)
        {
            RID result;

            ASSERT_TRUE(tree.search(key, result));
            expect_rid_eq(make_rid(key), result);
        }
    }

    TEST(BPlusTreeTest, InsertRandomOrder)
    {
        BPlusTree tree(3);

        std::vector<int> keys(300);
        std::iota(keys.begin(), keys.end(), 0);

        std::mt19937 rng(42);
        std::shuffle(keys.begin(), keys.end(), rng);

        for (int key : keys)
        {
            ASSERT_TRUE(tree.insert(key, make_rid(key)));
        }

        for (int key = 0; key < 300; ++key)
        {
            RID result;

            ASSERT_TRUE(tree.search(key, result))
                << "Could not find key " << key;

            expect_rid_eq(make_rid(key), result);
        }
    }

    TEST(BPlusTreeTest, HandlesNegativeKeys)
    {
        BPlusTree tree(3);

        for (int key = -50; key <= 50; ++key)
        {
            EXPECT_TRUE(tree.insert(key, make_rid(key)));
        }

        for (int key = -50; key <= 50; ++key)
        {
            RID result;

            ASSERT_TRUE(tree.search(key, result));
            expect_rid_eq(make_rid(key), result);
        }

        RID result;
        EXPECT_FALSE(tree.search(-51, result));
        EXPECT_FALSE(tree.search(51, result));
    }

}