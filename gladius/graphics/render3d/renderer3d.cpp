//
// Created by pav on 18.02.16.
//

#include <atomic>

#include "vulkan/vulkan_macros.h"
#include "vulkan/vulkan_utils.h"

#include "render3d.h"
#include "renderer3d.h"

#include "vulkan/vulkan_instance.h"
#include "vulkan/vulkan_device.h"
#include "vulkan/vulkan_command_buffer.h"
#include "vulkan/vulkan_surface.h"
#include "vulkan/vulkan_swapchain.h"


namespace gladius {
namespace graphics {
namespace render3d {

c_renderer3d renderer3d;

bool c_renderer3d::init(core::c_window *window) {
    VERIFY(create_instance(this, "appname"));
    VERIFY(create_device(this));
    VERIFY(create_surface(this, window));
    VERIFY(create_swap_chain(this));

    VERIFY(resources::create_command_pool(m_device, m_graphics_queue.index, &m_command_pool));
    VERIFY(resources::create_command_buffer(m_device, true, m_command_pool, m_swapchain.images.size(),
                                            m_swapchain.command_buffers.data()));

    VkSemaphoreCreateInfo semaphore_create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};
    VK_VERIFY(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphores.present_complete));
    VK_VERIFY(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_semaphores.render_complete));

    VkFenceCreateInfo fence_create_info = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr,
        VK_FENCE_CREATE_SIGNALED_BIT
    };
    m_swapchain.fences.resize(m_swapchain.command_buffers.size());
    for (auto &fence : m_swapchain.fences) {
        VK_VERIFY(vkCreateFence(m_device, &fence_create_info, nullptr, &fence));
    }

    m_semaphores.submit_info = {
            VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr,
            1, &m_semaphores.present_complete,
            nullptr,
            0, nullptr,
            1, &m_semaphores.render_complete
    };

    return true;
}

void c_renderer3d::shutdown() {

    if (m_device != nullptr) {
        vkDeviceWaitIdle(m_device);
        shutdown_swap_chain(this);
        delete m_gpu_memory_allocator;
        vkDestroyDevice(m_device, nullptr);
        m_device = nullptr;
    }

    if (m_surface.surface != nullptr) {
        vkDestroySurfaceKHR(m_instance, m_surface.surface, nullptr);
    }

    if (m_instance != nullptr) {
        vkDestroyInstance(m_instance, nullptr);
    }
}

bool c_renderer3d::build_command_buffer() {

    VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
    cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buffer_begin_info.pNext = nullptr;

    // Set clear values for all framebuffer attachments with loadOp set to clear
    // We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
    VkClearValue clearValues[2];
    clearValues[0].color = {{0.0f, 0.0f, 0.2f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.pNext = nullptr;
    //render_pass_begin_info.renderPass = m_forward_pipeline.m_render_pass;
    render_pass_begin_info.renderArea.offset.x = 0;
    render_pass_begin_info.renderArea.offset.y = 0;
    render_pass_begin_info.renderArea.extent.width = m_swapchain.width;
    render_pass_begin_info.renderArea.extent.height = m_swapchain.height;
    render_pass_begin_info.clearValueCount = 2;
    render_pass_begin_info.pClearValues = clearValues;

    //uint32_t i = 0;
    for (auto &command_buffer : m_swapchain.command_buffers) {

        //render_pass_begin_info.framebuffer = m_forward_pipeline.m_framebuffers[i++];

        VK_VERIFY(vkBeginCommandBuffer(command_buffer, &cmd_buffer_begin_info));

        vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = {
                0, 0,
                (float) m_swapchain.width, (float) m_swapchain.height,
                0.0f, 1.0f
        };
        vkCmdSetViewport(command_buffer, 0, 1, &viewport);

        VkRect2D scissor = {
                {
                        (int32_t) m_swapchain.width,
                        (int32_t) m_swapchain.height
                },
                {
                        0,
                        0
                }
        };
        vkCmdSetScissor(command_buffer, 0, 1, &scissor);

        /*
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_forward_pipeline.m_pipeline_layout,
                                0, 1, &m_forward_pipeline.descriptorSet,
                                0, nullptr);
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_forward_pipeline.m_pipeline);
        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(command_buffer, 0, 1, &vertices.buffer, offsets);
        vkCmdBindIndexBuffer(command_buffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(command_buffer, indices.count, 1, 0, 0, 1);
        vkCmdEndRenderPass(command_buffer);
*/
        VK_VERIFY(vkEndCommandBuffer(command_buffer));
    }

    return true;
}

bool c_renderer3d::render() {
    vkDeviceWaitIdle(m_device);

    uint32_t image_index;
    VERIFY(swap_chain_acquire_next_image(this, m_semaphores.present_complete, &image_index));

    VK_VERIFY(vkWaitForFences(m_device, 1, &m_swapchain.fences[image_index], VK_TRUE, UINT64_MAX));
    VK_VERIFY(vkResetFences(m_device, 1, &m_swapchain.fences[image_index]));

    VkPipelineStageFlags wait_dst_stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    m_semaphores.submit_info.pWaitDstStageMask = &wait_dst_stage_mask;
    m_semaphores.submit_info.commandBufferCount = 1;
    m_semaphores.submit_info.pCommandBuffers = &m_swapchain.command_buffers[image_index];

    VK_VERIFY(vkQueueSubmit(m_graphics_queue.handle, 1, &m_semaphores.submit_info, m_swapchain.fences[image_index]));

    VkPresentInfoKHR present_info = {
            VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr,
            1, &m_semaphores.render_complete,
            1, &(m_swapchain.handle),
            &image_index,
            nullptr
    };

    VK_VERIFY(vkQueuePresentKHR(m_graphics_queue.handle, &present_info));

    return true;
}

}
}
}