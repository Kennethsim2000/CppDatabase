#include <gtest/gtest.h>

#include "page.h"
#include "tuple.h"
#include "rid.h"
#include "table_page.h"

using namespace db;

// ================================
// RID Tests
// ================================

TEST(RIDTest, DefaultConstructor)
{
    RID rid;

    EXPECT_EQ(rid.page_id, INVALID_PAGE_ID);
    EXPECT_EQ(rid.slot_num, 0);
}

TEST(RIDTest, ParameterizedConstructor)
{
    RID rid(42, 5);

    EXPECT_EQ(rid.page_id, 42);
    EXPECT_EQ(rid.slot_num, 5);
}

// ================================
// TablePage Tests
// ================================

class TablePageTest : public ::testing::Test
{
protected:
    Page page;
    TablePage table_page{&page};

    void SetUp() override
    {
        page.setPageId(1);
        table_page.init();
    }
};

TEST_F(TablePageTest, InitializesCorrectly)
{
    EXPECT_EQ(page.getId(), 1);
    EXPECT_EQ(page.pin_count(), 0);
    EXPECT_FALSE(page.is_dirty());

    EXPECT_TRUE(table_page.has_space(100));
}