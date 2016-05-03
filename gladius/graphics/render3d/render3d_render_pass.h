//
// Created by alexa on 020 20 03 2016.
//

#ifndef GLADIUS_RENDER3D_RENDER_PASS_H
#define GLADIUS_RENDER3D_RENDER_PASS_H

namespace gladius
{
    namespace graphics
    {
        namespace render3d
        {
            namespace resources
            {
                void get_default_attachment(VkFormat format, VkSampleCountFlagBits samples, VkAttachmentDescription& attachment);

                VkRenderPass create_render_pass(const VkAttachmentDescription* attachments, size_t attachments_count,
                                                const VkSubpassDescription* subpasses, size_t subpasses_count);
            }
        }
    }
}

#endif //GLADIUS_RENDER3D_RENDER_PASS_H
