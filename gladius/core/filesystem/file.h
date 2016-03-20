//
// Created by pav on 12.08.15.
//

#ifndef GLADIUS_FILE_H
#define GLADIUS_FILE_H

#include "../types.h"

namespace gladius
{
    namespace core
    {
        namespace filesystem
        {
            class c_file
            {
            public:
                virtual void close() = 0;

            public:
                virtual size_t read(char* buf, size_t bytes) = 0;
                virtual size_t write(const char* buf, size_t bytes) = 0;

            public:
                template <typename T>
                bool read(T& value)
                {
                    size_t actual = read(&value, sizeof(T));
                    if (actual == sizeof(T)) {
                        return true;
                    }
                    return false;
                }

                template <typename T>
                bool write(const T& value)
                {
                    size_t actual = write (&value, sizeof(T));
                    if (actual == sizeof(T)) {
                        return true;
                    }
                    return false;
                }

            protected:
                c_file* m_fallback_file;
            };
        }
    }
}

#endif //GLADIUS_FILE_H
