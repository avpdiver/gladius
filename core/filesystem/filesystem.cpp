//
// Created by pav on 11.08.15.
//

#include <unordered_map>
#include <iterator>
#include "filesystem.h"

namespace gladius
{
    namespace core
    {
        namespace filesystem
        {
            namespace c_filesystem
            {
                std::unordered_map<const char *, file_creator_t> g_devices;

                void add_device (const char *device, file_creator_t creator)
                {
                    g_devices[device] = creator;
                }

                c_file *open (const char *filename, std::initializer_list<const char *> devices)
                {
                    c_file *file = nullptr;
                    c_file * new_file = nullptr;
                    for (const auto device : devices)
                    {
                        new_file = (*g_devices[device]) (filename, file);
                        if (new_file == nullptr)
                        {
                            close (file);
                            return nullptr;
                        }
                        file = new_file;
                    }
                    return file;
                }

                void close (c_file* file)
                {
                    if (file == nullptr) {
                        return;
                    }
                    file->close ();
                    delete file;
                }
            }
        }
    }
}
