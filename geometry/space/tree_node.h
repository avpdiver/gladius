//
// Created by alexa on 05.12.2015.
//

#ifndef GLADIUS_TREE_NODE_H
#define GLADIUS_TREE_NODE_H

#include "../visibility.h"

namespace gladius
{
    namespace geometry
    {
        namespace space
        {
            class c_quad_tree;

            class c_tree_node
            {
            public:

                /**
                 * Creates a new c_tree_node.
                 *
                 * @param m_quad_tree the c_quad_tree to which the %terrain quadtree belongs.
                 * @param m_parent the m_parent quad of this quad.
                 * @param tx the logical x coordinate of this quad.
                 * @param ty the logical y coordinate of this quad.
                 * @param ox the physical x coordinate of the lower left corner of this quad.
                 * @param oy the physical y coordinate of the lower left corner of this quad.
                 * @param l the physical size of this quad.
                 * @param zmin the minimum %terrain elevation inside this quad.
                 * @param zmax the maximum %terrain elevation inside this quad.
                 */
                c_tree_node(c_quad_tree *owner, const c_tree_node *parent, int tx, int ty, double ox, double oy, double l, float zmin, float zmax);

                /**
                 * Deletes this c_tree_node.
                 */
                virtual ~c_tree_node();

            public:
                /**
                 * Returns the c_quad_tree to which the %terrain quadtree belongs.
                 */
                c_quad_tree *get_tree();

                /**
                 * Returns true if this quad is not subdivided.
                 */
                bool is_leaf() const;

                /**
                 * Returns the number of quads in the tree below this quad.
                 */
                int get_size() const;

                /**
                 * Returns the depth of the tree below this quad.
                 */
                int get_depth() const;

                /**
                 * Subdivides or unsubdivides this quad based on the current
                 * viewer distance to this quad, relatively to its size. This
                 * method uses the current viewer position provided by the
                 * c_quad_tree to which this quadtree belongs.
                 */
                void update();

            private:
                /**
                 * Creates the four subquads of this quad.
                 */
                void subdivide();

            public:
                /**
                 * The m_parent quad of this quad.
                 */
                const c_tree_node *m_parent;

                /**
                * The four subquads of this quad. If this quad is not subdivided,
                * the four values are NULL. The subquads are stored in the
                * following order: bottomleft, bottomright, topleft, topright.
                */
                c_tree_node* m_children[4];

                /**
                 * The m_level of this quad in the quadtree (0 for the m_root).
                 */
                const int m_level;

                /**
                 * The visibility of the bounding box of this quad from the current
                 * viewer position. The bounding box is computed using #zmin and
                 * #zmax, which must therefore be up to date to get a correct culling
                 * of quads out of the view frustum. This visibility only takes frustum
                 * culling into account.
                 */
                e_visibility m_visible;

                /**
                 * True if the bounding box of this quad is m_occluded by the bounding
                 * boxes of the quads in front of it.
                 */
                bool m_occluded;

                /**
                 * True if the quad is invisible, or if all its associated tiles are
                 * produced and available in cache (this may not be the case if the
                 * asynchronous mode is used in a TileSampler).
                 */
                bool m_drawable;

            public:
                /**
                 * The logical x coordinate of this quad (between 0 and 2^m_level).
                 */
                const int tx;

                /**
                 * The logical y coordinate of this quad (between 0 and 2^m_level).
                 */
                const int ty;

                /**
                 * The physical x coordinate of the lower left corner of this quad
                 * (in local space).
                 */
                const double ox;

                /**
                 * The physical y coordinate of the lower left corner of this quad
                 * (in local space).
                 */
                const double oy;

                /**
                 * The physical size of this quad (in local space).
                 */
                const double l;

                /**
                 * The minimum %terrain elevation inside this quad. This field must
                 * be updated manually by users (the TileSamplerZ class can
                 * do this for you).
                 */
                float zmin;

                /**
                 * The maximum %terrain elevation inside this quad. This field must
                 * be updated manually by users (the TileSamplerZ class can
                 * do this for you).
                 */
                float zmax;

            private:
                /**
                 * The c_quad_tree to which this %terrain quadtree belongs.
                 */
                c_quad_tree *m_quad_tree;

            private:
                friend class c_quad_tree;
            };
        }
    }
}
#endif //GLADIUS_TREE_NODE_H
