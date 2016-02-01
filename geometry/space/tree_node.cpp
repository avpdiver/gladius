//
// Created by alexa on 05.12.2015.
//

#include <algorithm>

#include "../../core/math/box3.h"

#include "tree_node.h"
#include "quad_tree.h"

using namespace gladius::math;

namespace gladius
{
    namespace geometry
    {
        namespace space
        {
            c_tree_node::c_tree_node(c_quad_tree *owner, const c_tree_node *parent, int tx, int ty, double ox, double oy, double l, float zmin, float zmax)
                    : m_parent(parent),
                      m_level(parent == nullptr ? 0 : parent->m_level + 1),
                      tx(tx), ty(ty), ox(ox), oy(oy), l(l), zmin(zmin), zmax(zmax),
                      m_occluded(false), m_drawable(true), m_quad_tree(owner)
            {
            }

            c_tree_node::~c_tree_node()
            {
            }

            c_quad_tree *c_tree_node::get_tree()
            {
                return m_quad_tree;
            }

            bool c_tree_node::is_leaf() const
            {
                return m_children[0] == nullptr;
            }

            int c_tree_node::get_size() const
            {
                int s = 1;
                if (is_leaf())
                {
                    return s;
                }
                else
                {
                    return s + m_children[0]->get_size() + m_children[1]->get_size() + m_children[2]->get_size() + m_children[3]->get_size();
                }
            }

            int c_tree_node::get_depth() const
            {
                if (is_leaf())
                {
                    return m_level;
                }
                else
                {
                    return std::max(std::max(m_children[0]->get_depth(), m_children[1]->get_depth()), std::max(m_children[2]->get_depth(), m_children[3]->get_depth()));
                }
            }

            void c_tree_node::update()
            {
                e_visibility v = m_parent == nullptr ? e_visibility::PARTIALLY_VISIBLE : m_parent->m_visible;
                if (v == e_visibility::PARTIALLY_VISIBLE)
                {
                    box3d localBox(ox, ox + l, oy, oy + l, zmin, zmax);
                    m_visible = m_quad_tree->get_visibility(localBox);
                }
                else
                {
                    m_visible = v;
                }

                // here we reuse the occlusion test from the previous frame:
                // if the quad was found unoccluded in the previous frame, we suppose it is
                // still unoccluded at this frame. If it was found m_occluded, we perform
                // an occlusion test to check if it is still m_occluded.
                if (m_visible != e_visibility::INVISIBLE && m_occluded)
                {
                    m_occluded = m_quad_tree->is_occluded(box3d(ox, ox + l, oy, oy + l, zmin, zmax));
                    if (m_occluded)
                    {
                        m_visible = e_visibility::INVISIBLE;
                    }
                }

                double ground = c_quad_tree::m_ground_height_at_camera;
                float  dist   = m_quad_tree->get_camera_dist(box3d(ox, ox + l, oy, oy + l, std::min(0.0, ground), std::max(0.0, ground)));

                if ((m_quad_tree->m_split_invisible_quads || m_visible != e_visibility::INVISIBLE) && dist < l * m_quad_tree->get_split_distance() && m_level < m_quad_tree->m_max_level)
                {
                    if (is_leaf())
                    {
                        subdivide();
                    }

                    int    order[4];

                    double ox = m_quad_tree->get_local_camera().x;
                    double oy = m_quad_tree->get_local_camera().y;
                    double cx = this->ox + l / 2.0;
                    double cy = this->oy + l / 2.0;

                    if (oy < cy)
                    {
                        if (ox < cx)
                        {
                            order[0] = 0;
                            order[1] = 1;
                            order[2] = 2;
                            order[3] = 3;
                        }
                        else
                        {
                            order[0] = 1;
                            order[1] = 0;
                            order[2] = 3;
                            order[3] = 2;
                        }
                    }
                    else
                    {
                        if (ox < cx)
                        {
                            order[0] = 2;
                            order[1] = 0;
                            order[2] = 3;
                            order[3] = 1;
                        }
                        else
                        {
                            order[0] = 3;
                            order[1] = 1;
                            order[2] = 2;
                            order[3] = 0;
                        }
                    }

                    m_children[order[0]]->update();
                    m_children[order[1]]->update();
                    m_children[order[2]]->update();
                    m_children[order[3]]->update();

                    // we compute a more precise occlusion for the next frame (see above),
                    // by combining the occlusion status of the child nodes
                    m_occluded = m_children[0]->m_occluded && m_children[1]->m_occluded && m_children[2]->m_occluded && m_children[3]->m_occluded;
                }
                else
                {
                    if (m_visible != e_visibility::INVISIBLE)
                    {
                        // we add the bounding box of this quad to the occluders list
                        m_occluded = m_quad_tree->add_occluder(box3d(ox, ox + l, oy, oy + l, zmin, zmax));
                        if (m_occluded)
                        {
                            m_visible = e_visibility::INVISIBLE;
                        }
                    }
                    if (!is_leaf())
                    {
                        m_children[0] = nullptr;
                        m_children[1] = nullptr;
                        m_children[2] = nullptr;
                        m_children[3] = nullptr;
                    }
                }
            }

            void c_tree_node::subdivide()
            {
                float hl = (float) l / 2.0f;
                m_children[0] = new c_tree_node(m_quad_tree, this, 2 * tx, 2 * ty, ox, oy, hl, zmin, zmax);
                m_children[1] = new c_tree_node(m_quad_tree, this, 2 * tx + 1, 2 * ty, ox + hl, oy, hl, zmin, zmax);
                m_children[2] = new c_tree_node(m_quad_tree, this, 2 * tx, 2 * ty + 1, ox, oy + hl, hl, zmin, zmax);
                m_children[3] = new c_tree_node(m_quad_tree, this, 2 * tx + 1, 2 * ty + 1, ox + hl, oy + hl, hl, zmin, zmax);
            }
        }
    }
}