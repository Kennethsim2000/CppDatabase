#include <gtest/gtest.h>
#include "page.h"

using namespace db;

TEST(PageTest, PinUnpin)
{
    Page p;

    // Act
    p.pin();
    p.pin();
    p.unpin();

    // Assert
    // pin_count == 1
}

TEST(PageTest, DirtyFlag)
{
    Page p;

    // Act
    p.mark_dirty();

    // Assert
    // is_dirty == true

    // Act
    p.clear_dirty();

    // Assert
    // is_dirty == false
}

TEST(PageTest, Reset)
{
    Page p;

    // Arrange
    // set id, dirty, pin, write data

    // Act
    p.reset();

    // Assert
    // id == INVALID
    // buffer zeroed
    // pin_count == 0
}