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
                static c_file* create(const char* filename, c_file* fallback, e_file_mode mode);

            private:
                c_disk_file();

            public:
                ~c_disk_file ();

            public:
                virtual size_t read(char* buf, size_t bytes);
                virtual size_t write(const char* buf, size_t bytes);

            private:
                std::basic_ios<char>* m_stream;
                e_file_mode m_mode;
                c_file* m_fallback_file;
            };
        }
    }
}

#endif //GLADIUS_DISK_FILE_H
