//
// Created by alexa on 05.12.5.
//

#include <algorithm>

#include "quad_tree.h"
#include "../../core/math/common_math.h"
#include "../../core/math/mat4.h"

#define HORIZON_SIZE 256

using namespace gladius::math;

namespace gladius
{
    namespace geometry
    {
        namespace space
        {
            float c_quad_tree::m_ground_height_at_camera      = 0.0f;
            float c_quad_tree::m_next_ground_height_at_camera = 0.0f;

            c_quad_tree::c_quad_tree(c_deformation *deform, float splitFactor, int maxLevel)
            {
                init(deform, nullptr, splitFactor, maxLevel);
            }

            c_quad_tree::c_quad_tree()
            {
            }

            void c_quad_tree::init(c_deformation *deform, c_tree_node *root, float splitFactor, int maxLevel)
            {
                this->m_deform                = deform;
                this->m_root                  = root;
                this->m_split_factor          = splitFactor;
                this->m_split_invisible_quads = false;
                this->m_horizon_culling       = true;
                this->m_split_distance        = 1.1f;
                this->m_max_level             = maxLevel;
                root->m_quad_tree             = this;
                m_horizon = new float[HORIZON_SIZE];
            }

            c_quad_tree::~c_quad_tree()
            {
                delete[] m_horizon;
            }

            vec3d c_quad_tree::get_deformed_camera() const
            {
                return m_deformed_camera_pos;
            }

            const vec4d *c_quad_tree::get_deformed_frustum_planes() const
            {
                return m_deformed_frustum_planes;
            }

            vec3d c_quad_tree::get_local_camera() const
            {
                return m_local_camera_pos;
            }

            float c_quad_tree::get_camera_dist(const box3d &localBox) const
            {
                return (float) std::max(
                        std::abs(m_local_camera_pos.z - localBox.zmax) / m_distance_factor,
                        std::max(
                                std::min(std::abs(m_local_camera_pos.x - localBox.xmin), std::abs(m_local_camera_pos.x - localBox.xmax)),
                                std::min(std::abs(m_local_camera_pos.y - localBox.ymin), std::abs(m_local_camera_pos.y - localBox.ymax))
                        ));
            }

            e_visibility c_quad_tree::get_visibility(const box3d &localBox) const
            {
                return m_deform->getVisibility(this, localBox);
            }

            float c_quad_tree::get_split_distance() const
            {
                return m_split_distance;
            }

            float c_quad_tree::get_dist_factor() const
            {
                return m_distance_factor;
            }

            void c_quad_tree::update(c_render_context* render_context, c_transform_node* transform_node)
            {
                m_deformed_camera_pos = transform_node->getLocalToCamera().inverse() * vec3d::ZERO;

                get_frustum_planes(transform_node->getLocalToScreen(), m_deformed_frustum_planes);

                m_local_camera_pos = m_deform->deformedToLocal(m_deformed_camera_pos);
                mat4d m = m_deform->localToDeformedDifferential(m_local_camera_pos, true);

                m_distance_factor = std::max(vec3d(m[0][0], m[1][0], m[2][0]).length(), vec3d(m[0][1], m[1][1], m[2][1]).length());

                vec3d           left  = m_deformed_frustum_planes[0].xyz().normalize();
                vec3d           right = m_deformed_frustum_planes[1].xyz().normalize();
                float           fov   = (float) safe_acos(-left.dot(right));

                m_split_distance = m_split_factor * render_context->getViewport().z / 1024.0f * std::tan(40.0f / 180.0f * PI) / std::tan(fov / 2.0f);
                if (m_split_distance < 1.1f || !(std::isinf(m_split_distance)))
                {
                    m_split_distance = 1.1f;
                }

                // initializes data structures for m_horizon occlusion culling
                if (m_horizon_culling && m_local_camera_pos.z <= m_root->zmax)
                {
                    vec3d deformedDir = transform_node->getLocalToCamera().inverse() * vec3d::UNIT_Z;
                    vec2d localDir    = (m_deform->deformedToLocal(deformedDir) - m_local_camera_pos).xy().normalize();
                    m_local_camera_dir = mat2f(localDir.y, -localDir.x, -localDir.x, -localDir.y);

                    for (int i = 0; i < HORIZON_SIZE; ++i)
                    {
                        m_horizon[i] = -INFINITY;
                    }
                }

                m_root->update();
            }

            bool c_quad_tree::add_occluder(const box3d &occluder)
            {
                if (!m_horizon_culling || m_local_camera_pos.z > m_root->zmax)
                {
                    return false;
                }

                vec2f corners[4];
                vec2d o = m_local_camera_pos.xy();

                corners[0] = m_local_camera_dir * (vec2d(occluder.xmin, occluder.ymin) - o).cast<float>();
                corners[1] = m_local_camera_dir * (vec2d(occluder.xmin, occluder.ymax) - o).cast<float>();
                corners[2] = m_local_camera_dir * (vec2d(occluder.xmax, occluder.ymin) - o).cast<float>();
                corners[3] = m_local_camera_dir * (vec2d(occluder.xmax, occluder.ymax) - o).cast<float>();

                if (corners[0].y <= 0.0f || corners[1].y <= 0.0f || corners[2].y <= 0.0f || corners[3].y <= 0.0f)
                {
                    // skips bounding boxes that are not fully behind the "near plane"
                    // of the reference frame used for m_horizon occlusion culling
                    return false;
                }

                float    dzmin = float(occluder.zmin - m_local_camera_pos.z);
                float    dzmax = float(occluder.zmax - m_local_camera_pos.z);

                vec3f bounds[4];

                bounds[0] = vec3f(corners[0].x, dzmin, dzmax) / corners[0].y;
                bounds[1] = vec3f(corners[1].x, dzmin, dzmax) / corners[1].y;
                bounds[2] = vec3f(corners[2].x, dzmin, dzmax) / corners[2].y;
                bounds[3] = vec3f(corners[3].x, dzmin, dzmax) / corners[3].y;

                float    xmin     = std::min(std::min(bounds[0].x, bounds[1].x), std::min(bounds[2].x, bounds[3].x)) * 0.33f + 0.5f;
                float    xmax     = std::max(std::max(bounds[0].x, bounds[1].x), std::max(bounds[2].x, bounds[3].x)) * 0.33f + 0.5f;
                float    zmin     = std::min(std::min(bounds[0].y, bounds[1].y), std::min(bounds[2].y, bounds[3].y));
                float    zmax     = std::max(std::max(bounds[0].z, bounds[1].z), std::max(bounds[2].z, bounds[3].z));

                int      imin     = std::max(int(std::floor(xmin * HORIZON_SIZE)), 0);
                int      imax     = std::min(int(std::ceil(xmax * HORIZON_SIZE)), HORIZON_SIZE - 1);

                // first checks if the bounding box projection is below the current m_horizon line
                bool     occluded = imax >= imin;
                for (int i        = imin; i <= imax; ++i)
                {
                    if (zmax > m_horizon[i])
                    {
                        occluded = false;
                        break;
                    }
                }

                if (!occluded)
                {
                    // if it is not, updates the m_horizon line with the projection of this bounding box
                    imin       = std::max(int(std::ceil(xmin * HORIZON_SIZE)), 0);
                    imax       = std::min(int(std::floor(xmax * HORIZON_SIZE)), HORIZON_SIZE - 1);
                    for (int i = imin; i <= imax; ++i)
                    {
                        m_horizon[i] = std::max(m_horizon[i], zmin);
                    }
                }

                return occluded;
            }

            bool c_quad_tree::is_occluded(const box3d &box)
            {
                if (!m_horizon_culling || m_local_camera_pos.z > m_root->zmax)
                {
                    return false;
                }

                vec2f corners[4];
                vec2d o = m_local_camera_pos.xy();

                corners[0] = m_local_camera_dir * (vec2d(box.xmin, box.ymin) - o).cast<float>();
                corners[1] = m_local_camera_dir * (vec2d(box.xmin, box.ymax) - o).cast<float>();
                corners[2] = m_local_camera_dir * (vec2d(box.xmax, box.ymin) - o).cast<float>();
                corners[3] = m_local_camera_dir * (vec2d(box.xmax, box.ymax) - o).cast<float>();

                if (corners[0].y <= 0.0f || corners[1].y <= 0.0f || corners[2].y <= 0.0f || corners[3].y <= 0.0f)
                {
                    return false;
                }

                float dz = float(box.zmax - m_local_camera_pos.z);

                corners[0] = vec2f(corners[0].x, dz) / corners[0].y;
                corners[1] = vec2f(corners[1].x, dz) / corners[1].y;
                corners[2] = vec2f(corners[2].x, dz) / corners[2].y;
                corners[3] = vec2f(corners[3].x, dz) / corners[3].y;

                float    xmin = std::min(std::min(corners[0].x, corners[1].x), std::min(corners[2].x, corners[3].x)) * 0.33f + 0.5f;
                float    xmax = std::max(std::max(corners[0].x, corners[1].x), std::max(corners[2].x, corners[3].x)) * 0.33f + 0.5f;
                float    zmax = std::max(std::max(corners[0].y, corners[1].y), std::max(corners[2].y, corners[3].y));

                int      imin = std::max(int(std::floor(xmin * HORIZON_SIZE)), 0);
                int      imax = std::min(int(std::ceil(xmax * HORIZON_SIZE)), HORIZON_SIZE - 1);

                for (int i    = imin; i <= imax; ++i)
                {
                    if (zmax > m_horizon[i])
                    {
                        return false;
                    }
                }

                return imax >= imin;
            }

            void c_quad_tree::swap(c_quad_tree* t)
            {
                std::swap(m_deform, t->m_deform);
                std::swap(m_root, t->m_root);
                std::swap(m_split_factor, t->m_split_factor);
                std::swap(m_max_level, t->m_max_level);
                std::swap(m_deformed_camera_pos, t->m_deformed_camera_pos);
                std::swap(m_local_camera_pos, t->m_local_camera_pos);
                std::swap(m_split_distance, t->m_split_distance);

                for (int i = 0; i < 6; ++i)
                {
                    std::swap(m_deformed_frustum_planes[i], t->m_deformed_frustum_planes[i]);
                }
            }
        }
    }
}