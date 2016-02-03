//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_DISK_FILE_H
#define GLADIUS_DISK_FILE_H

#include <stddef.h>
#include <fstream>
#include "file.h"

namespace gladius
{
    namespace core
    {
        namespace filesystem
        {
            class c_disk_file : public c_file
            {
            public:
                static constexpr char const* DEVICE = "disk";

            public:
                static c_file* create(const char* filename, c_file* fallback);

            private:
                c_disk_file();

            public:
                ~c_disk_file ();

            private:
                bool open(const char* filename, c_file* fallback);

            public:
                virtual void close();

            public:
                virtual size_t read(char* buf, size_t bytes);
                virtual size_t write(const char* buf, size_t bytes);

            private:
                std::fstream m_stream;
            };
        }
    }
}

#endif //GLADIUS_DISK_FILE_H
