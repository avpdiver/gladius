//
// Created by pav on 12.08.15.
//

#include "filesystem.h"
#include "disk_file.h"

namespace gladius
{
    namespace core
    {
        namespace filesystem
        {
            c_file* c_disk_file::create(const char* filename, c_file* fallback)
            {
                c_disk_file* file = new c_disk_file();
                if (file->open (filename, nullptr)) {
                    return file;
                } else {
                    delete file;
                    return nullptr;
                }
            }

            c_disk_file::c_disk_file()
            {

            }

            c_disk_file::~c_disk_file()
            {
                close ();
            }

            bool c_disk_file::open(const char* filename, c_file* fallback)
            {
                m_stream.open (filename, fallback == nullptr ? std::ios::binary : (std::ios::out | std::ios::binary));
                if (!m_stream.is_open ())
                {
                    return false;
                }
                return true;
            }

            void c_disk_file::close()
            {
                m_stream.close ();
            }

            size_t c_disk_file::read(char* buf, size_t bytes)
            {
                return m_stream.readsome (buf, bytes);
            }

            size_t c_disk_file::write(const char* buf, size_t bytes)
            {
                auto before = m_stream.tellp();
                m_stream.write(buf, bytes);
                return m_stream.tellp() - before;
            }
        }
    }
}