// ================================
// File: include/db/disk_manager.h
// Responsible for reading/writing pages to disk
// ================================
#pragma once

#include <string>
#include "page.h"
#include <fstream>

namespace db
{

    class DiskManager
    {
    public:
        explicit DiskManager(const std::string &db_file);

        // Read a page from disk into memory
        void read_page(PageId page_id, Page &page);

        // Write a page from memory to disk
        void write_page(const Page &page);

    private:
        std::fstream stream_;
    };

}