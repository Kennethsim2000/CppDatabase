#include <gtest/gtest.h>
#include "disk_manager.h"
#include "page.h"
#include "buffer_pool.h"

using namespace db;

TEST(BufferPoolTest, FetchNewPage)
{
    // Arrange
    // create DiskManager + BufferPoolManager

    // Act
    // fetch_page(some_id)

    // Assert
    // page != nullptr
    // page->getId() == some_id
}

TEST(BufferPoolTest, FetchSamePageTwice)
{
    // Arrange

    // Act
    // Page *p1 = ...;
    // Page *p2 = ...;

    // Assert
    // p1 == p2 (same frame)
}

TEST(BufferPoolTest, UnpinMakesFrameAvailable)
{
    // Arrange

    // Act
    // fetch page
    // unpin page

    // Assert
    // frame is available again (indirect check via next fetch)
}

TEST(BufferPoolTest, EvictionWhenFull)
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

TEST(BufferPoolTest, DirtyPageFlushOnEvict)
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

TEST(BufferPoolTest, PinnedPageNotEvicted)
{
    // Arrange
    // pool size = 1

    // Act
    // fetch A (do NOT unpin)
    // fetch B

    // Assert
    // should return nullptr (no frame available)
}