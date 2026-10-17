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

    static void expect_rids_eq(
        const std::vector<RID> &expected,
        const std::vector<RID> &actual)
    {
        ASSERT_EQ(expected.size(), actual.size());

        for (size_t i = 0; i < expected.size(); ++i)
        {
            expect_rid_eq(expected[i], actual[i]);
        }
    }

    TEST(BPlusTreeTest, SearchEmptyTree)
    {
        BPlusTree tree;
        RID result;
        std::vector<RID> results;

        EXPECT_FALSE(tree.search(10, result));
        EXPECT_FALSE(tree.search_all(10, results));
        EXPECT_TRUE(results.empty());
    }

    TEST(BPlusTreeTest, InsertAndSearchSingleKey)
    {
        BPlusTree tree;

        RID expected = make_rid(10);

        EXPECT_TRUE(tree.insert(10, expected));

        RID result;
        ASSERT_TRUE(tree.search(10, result));
        expect_rid_eq(expected, result);

        std::vector<RID> results;
        ASSERT_TRUE(tree.search_all(10, results));
        expect_rids_eq({expected}, results);
    }

    TEST(BPlusTreeTest, InsertDuplicateKeyStoresMultipleRids)
    {
        BPlusTree tree;

        RID first_rid = make_rid(10);
        RID second_rid = make_rid(20);
        RID third_rid = make_rid(30);

        EXPECT_TRUE(tree.insert(10, first_rid));

        EXPECT_FALSE(tree.insert(10, second_rid));
        EXPECT_FALSE(tree.insert(10, third_rid));

        RID result;
        ASSERT_TRUE(tree.search(10, result));
        expect_rid_eq(first_rid, result);

        std::vector<RID> results;
        ASSERT_TRUE(tree.search_all(10, results));
        expect_rids_eq({first_rid, second_rid, third_rid}, results);
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

        std::vector<RID> results = {make_rid(999)};
        EXPECT_FALSE(tree.search_all(15, results));
        EXPECT_TRUE(results.empty());
    }

    TEST(BPlusTreeTest, SplitLeafNodes)
    {
        BPlusTree tree(3);

        for (int key = 0; key < 20; ++key)
        {
            ASSERT_TRUE(tree.insert(key, make_rid(key)));
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

    TEST(BPlusTreeTest, DuplicateRidsRemainSearchableAfterSplits)
    {
        BPlusTree tree(3);

        // Insert enough distinct keys to trigger both leaf and internal splits.
        for (int key = 0; key < 100; ++key)
        {
            ASSERT_TRUE(tree.insert(key, make_rid(key)));
        }

        const std::vector<RID> duplicates = {
            make_rid(1001),
            make_rid(1002),
            make_rid(1003)};

        // Key 42 already exists. Adding more RIDs must not create a new key.
        for (const RID &rid : duplicates)
        {
            EXPECT_FALSE(tree.insert(42, rid));
        }

        std::vector<RID> results;
        ASSERT_TRUE(tree.search_all(42, results));

        std::vector<RID> expected = {make_rid(42)};
        expected.insert(expected.end(), duplicates.begin(), duplicates.end());
        expect_rids_eq(expected, results);

        for (int key = 0; key < 100; ++key)
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
            ASSERT_TRUE(tree.insert(key, make_rid(key)));
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
            ASSERT_TRUE(tree.insert(key, make_rid(key)));
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
