#include "disk_manager.h"
#include "iostream"
#include <cstring>

using namespace db;

DiskManager::DiskManager(const std::string &db_file) : stream_(db_file, std::ios::in | std::ios::out | std::ios::binary)
{
    if (!stream_) // check if opening failed, can mean file does not exist, permission issue or some other I/O errors
    {
        std::fstream create(db_file, std::ios::out | std::ios::binary); // create the file if does not exist
        // this opens the file in write mode only
        create.close(); // immediately close it

        stream_.open(db_file, std::ios::in | std::ios::out | std::ios::binary); // reopen it properly
        if (!stream_)
        {
            std::cerr << "Error opening file!" << std::endl;
        }
    }
};

void DiskManager::read_page(PageId page_id, Page &page)
{
    size_t offset = page_id * PAGE_SIZE;
    stream_.seekg(offset, std::ios::beg);
    // reads the stream raw bytes into the page
    stream_.read(reinterpret_cast<char *>(page.data()), PAGE_SIZE);
    // page.data() returns uint8_t*, read() expects char*

    std::streamsize bytes_read = stream_.gcount();
    if (bytes_read < PAGE_SIZE) // handle partial reads
    {
        std::memset(page.data() + bytes_read, 0, PAGE_SIZE - bytes_read);
        stream_.clear(); // clear EOF flag
        // used to reset the error flags of the stream
        // when an IO operation fails, the stream object sets one or more internal error flags, such as failbit, eofbit, badbit.
    }
}

void DiskManager::write_page(const Page &page)
{
    size_t offset = page.getId() * PAGE_SIZE;
    stream_.seekp(offset, std::ios::beg);
    stream_.write(reinterpret_cast<const char *>(page.data()), PAGE_SIZE);

    if (!stream_)
    {
        std::cerr << "Error writing page to disk" << std::endl;
    }
    stream_.flush(); // forcefully write any data currently held in the stream's internal buffer to its final destination
}