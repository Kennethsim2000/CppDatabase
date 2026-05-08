#include <gtest/gtest.h>
#include "disk_manager.h"
#include "page.h"

using namespace db;

TEST(DiskManagerTest, WriteThenRead)
{
    const std::string filename = "test.db";
    DiskManager dm(filename);

    Page page;
    page.setPageId(0);
    page.write<int>(0, 42);
    page.write<double>(8, 3.14159);
    page.write<char>(100, 'A');
    dm.write_page(page);

    Page loaded;
    loaded.setPageId(0);
    dm.read_page(0, loaded);

    ASSERT_EQ(loaded.read<int>(0), 42);
    ASSERT_DOUBLE_EQ(loaded.read<double>(8), 3.14159);
    ASSERT_EQ(loaded.read<char>(100), 'A');

    std::remove(filename.c_str());
}

TEST(DiskManagerTest, ReadBeyondEOF)
{
    const std::string filename = "test.db";
    DiskManager dm(filename);

    Page page;
    page.setPageId(5);

    dm.read_page(5, page);
    for (size_t i = 0; i < PAGE_SIZE; i++)
    {
        ASSERT_EQ(page.data()[i], 0);
    }
    std::remove(filename.c_str());
}

TEST(DiskManagerTest, OverwritePage)
{
    const std::string filename = "test.db";
    DiskManager dm(filename);

    Page page;
    page.setPageId(0);
    page.write<int>(0, 42);
    dm.write_page(page);
    page.write<int>(0, 100);
    dm.write_page(page);

    Page loaded;
    dm.read_page(0, loaded);
    ASSERT_EQ(100, page.read<int>(0));
}