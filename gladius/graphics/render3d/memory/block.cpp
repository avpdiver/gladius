#include "block.h"

namespace gladius {
namespace graphics {
namespace render3d {
namespace memory {

bool c_block::operator==(c_block const &block) {
    return (m_memory == block.m_memory &&
            m_offset == block.m_offset &&
            m_size == block.m_size &&
            m_free == block.m_free &&
            m_ptr == block.m_ptr);
}

}
}
}
}
