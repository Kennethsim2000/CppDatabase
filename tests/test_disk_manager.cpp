#include <gtest/gtest.h>
#include "disk_manager.h"
#include "page.h"

using namespace db;

TEST(DiskManagerTest, WriteThenRead)
{
    // Arrange
    // create temp file
    // create DiskManager
    // create Page and write some data

    // Act
    // write_page
    // read_page into new Page

    // Assert
    // compare buffers (memcmp or loop)
}

TEST(DiskManagerTest, ReadBeyondEOF)
{
    // Arrange
    // empty file

    // Act
    // read_page for page_id > 0

    // Assert
    // buffer should be all zeros
}

TEST(DiskManagerTest, OverwritePage)
{
    // Arrange
    // write page with data A

    // Act
    // overwrite same page with data B
    // read back

    // Assert
    // data == B
}