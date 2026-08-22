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
    ASSERT_NE(p2, nullptr); // Both fetches should point to the same physical frame
    EXPECT_EQ(p1, p2);
    bpm.unpin_page(page_id);
}

TEST_F(BufferPoolTest, UnpinMakesFrameAvailable)
{
    // Arrange

    // Act
    // fetch page
    // unpin page

    // Assert
    // frame is available again (indirect check via next fetch)
}

TEST_F(BufferPoolTest, EvictionWhenFull)
{
    // Arrange
    // small pool size (e.g. 1)

    // Act
    // fetch page A
    // unpin A
    // fetch page B

    // Assert
    // A should be evicted
}

TEST_F(BufferPoolTest, DirtyPageFlushOnEvict)
{
    // Arrange
    // pool size = 1

    // Act
    // fetch A
    // modify + mark_dirty
    // unpin
    // fetch B (forces eviction)

    // Assert
    // read A from disk → changes persisted
}

TEST_F(BufferPoolTest, PinnedPageNotEvicted)
{
    // Arrange
    // pool size = 1

    // Act
    // fetch A (do NOT unpin)
    // fetch B

    // Assert
    // should return nullptr (no frame available)
}