#include <gtest/gtest.h>
#include "page.h"

using namespace db;

TEST(PageTest, PinUnpin)
{
    Page p;
    p.pin();
    p.pin();
    p.unpin();
    ASSERT_EQ(1, p.pin_count());
}

TEST(PageTest, DirtyFlag)
{
    Page p;
    p.mark_dirty();
    ASSERT_TRUE(p.is_dirty());
    p.clear_dirty();
    ASSERT_FALSE(p.is_dirty());
}

TEST(PageTest, Reset)
{
    Page p;
    p.setPageId(42);
    p.mark_dirty();
    p.write<int>(0, 123);
    p.reset();

    // Assert
    ASSERT_EQ(p.getId(), INVALID_PAGE_ID);
    ASSERT_EQ(p.pin_count(), 0);
    ASSERT_FALSE(p.is_dirty());
    for (size_t i = 0; i < PAGE_SIZE; i++)
    {
        ASSERT_EQ(p.data()[i], 0);
    }
}