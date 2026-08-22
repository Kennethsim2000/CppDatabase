#include <gtest/gtest.h>
#include "disk_manager.h"
#include "page.h"
#include "buffer_pool.h"

using namespace db;

class BufferPoolTest : public ::testing::Test
{
protected:
    std::string filename = "test_buffer_pool.db";

    void SetUp() override
    { // Remove old test database before each test
        std::filesystem::remove(filename);
    }
    void TearDown() override
    {
        std::filesystem::remove(filename);
    }
};

TEST_F(BufferPoolTest, FetchNewPage)
{
    DiskManager disk(filename);
    BufferPoolManager bpm(disk, 2);

    PageId page_id = bpm.fetch_next_page();
    Page *page = bpm.fetch_page(page_id);
    ASSERT_NE(page, nullptr);
    EXPECT_EQ(page->getId(), page_id);
    EXPECT_EQ(page->pin_count(), 1);
    bpm.unpin_page(page_id);
}

TEST_F(BufferPoolTest, FetchSamePageTwice)
{
    DiskManager disk(filename);
    BufferPoolManager bpm(disk, 2);

    PageId page_id = bpm.fetch_next_page();
    Page *p1 = bpm.fetch_page(page_id);
    ASSERT_NE(p1, nullptr);
    bpm.unpin_page(page_id);

    Page *p2 = bpm.fetch_page(page_id);
    ASSERT_NE(p2, nullptr);

    // Both fetches should point to the same physical frame
    EXPECT_EQ(p1, p2);
    bpm.unpin_page(page_id);
}

TEST_F(BufferPoolTest, UnpinMakesFrameAvailable)
{
    DiskManager disk(filename);
    BufferPoolManager bpm(disk, 1);
    PageId page_a = bpm.fetch_next_page();
    PageId page_b = bpm.fetch_next_page();
    Page *a = bpm.fetch_page(page_a);
    ASSERT_NE(a, nullptr);

    // A is now pinned, so unpin it to make it evictable
    bpm.unpin_page(page_a);

    // Since pool size is 1, B can only be loaded if A became available
    Page *b = bpm.fetch_page(page_b);
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->getId(), page_b);
}

TEST_F(BufferPoolTest, EvictionWhenFull)
{
    DiskManager disk(filename);
    BufferPoolManager bpm(disk, 1);
    PageId page_a = bpm.fetch_next_page();
    PageId page_b = bpm.fetch_next_page();

    Page *a = bpm.fetch_page(page_a);
    ASSERT_NE(a, nullptr);
    bpm.unpin_page(page_a);

    // Pool is full, so A must be evicted
    Page *b = bpm.fetch_page(page_b);
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->getId(), page_b);
    bpm.unpin_page(page_b);

    // A should no longer be in memory.
    // Fetching it again should load it into the same frame.
    Page *a_again = bpm.fetch_page(page_a);
    ASSERT_NE(a_again, nullptr);
    EXPECT_EQ(a_again->getId(), page_a);
}

TEST_F(BufferPoolTest, DirtyPageFlushOnEvict)
{
    DiskManager disk(filename);
    BufferPoolManager bpm(disk, 1);

    PageId page_a = bpm.fetch_next_page();
    PageId page_b = bpm.fetch_next_page();
    Page *a = bpm.fetch_page(page_a);
    ASSERT_NE(a, nullptr);

    // Modify A
    a->data()[0] = 'X';
    a->mark_dirty();
    bpm.unpin_page(page_a);

    // This forces A to be evicted
    Page *b = bpm.fetch_page(page_b);
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->getId(), page_b);
    bpm.unpin_page(page_b);

    // Fetch A again.
    // If eviction correctly flushed A, its data should still be there.
    Page *a_again = bpm.fetch_page(page_a);
    ASSERT_NE(a_again, nullptr);
    EXPECT_EQ(a_again->data()[0], 'X');
    bpm.unpin_page(page_a);
}

TEST_F(BufferPoolTest, PinnedPageNotEvicted)
{

    DiskManager disk(filename);
    BufferPoolManager bpm(disk, 1);
    PageId page_a = bpm.fetch_next_page();
    PageId page_b = bpm.fetch_next_page();

    Page *a = bpm.fetch_page(page_a);
    ASSERT_NE(a, nullptr);

    // No free frame and A cannot be evicted
    Page *b = bpm.fetch_page(page_b);
    EXPECT_EQ(b, nullptr);
    bpm.unpin_page(page_a);
}

TEST_F(BufferPoolTest, FlushPageWritesDirtyData)
{
    DiskManager disk(filename);
    BufferPoolManager bpm(disk, 1);
    PageId page_id = bpm.fetch_next_page();
    Page *page = bpm.fetch_page(page_id);
    ASSERT_NE(page, nullptr);

    page->data()[0] = 'A';
    page->mark_dirty();
    bpm.flush_page(page_id);

    // Dirty flag should be cleared after flushing
    EXPECT_FALSE(page->is_dirty());
    bpm.unpin_page(page_id);

    // Fetch again and verify the data remains
    Page *page_again = bpm.fetch_page(page_id);
    ASSERT_NE(page_again, nullptr);
    EXPECT_EQ(page_again->data()[0], 'A');
    bpm.unpin_page(page_id);
}