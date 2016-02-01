//
// Created by alexa on 05.12.2015.
//

#ifndef GLADIUS_QUAD_TREE_H
#define GLADIUS_QUAD_TREE_H

#include "../../core/math/vec4.h"
#include "../../core/math/box3.h"
#include "../../core/math/mat2.h"

#include "../visibility.h"
#include "tree_node.h"
#include "deformation.h"
#include "../transform_node.h"
#include "../../graphics/render_context.h"

using namespace gladius::math;

namespace gladius
{
    namespace geometry
    {
        namespace space
        {
            class c_quad_tree
            {
            public:

                /**
                 * Creates a new c_quad_tree.
                 *
                 * @param m_deform the %terrain deformation.
                 * @param m_root the m_root of the %terrain quadtree.
                 * @param m_split_factor how the %terrain quadtree must be subdivided (see
                 *      #m_split_factor).
                 * @param m_max_level the maximum m_level at which the %terrain quadtree must be
                 *      subdivided (inclusive).
                 */
                c_quad_tree(c_deformation* deform, float splitFactor, int maxLevel);

                /**
                 * Deletes this c_quad_tree.
                 */
                virtual ~c_quad_tree();

            public:
                /**
                 * Returns the current viewer position in the deformed %terrain space
                 * (see #m_deform). This position is updated by the #update method.
                 */
                vec3d get_deformed_camera() const;

                /**
                 * Returns the current viewer frustum planes in the deformed #terrain
                 * space (see #m_deform). These planes are updated by the #update method.
                 */
                const vec4d *get_deformed_frustum_planes() const;

                /**
                 * Returns the current viewer position in the local %terrain space
                 * (see #m_deform). This position is updated by the #update method.
                 */
                vec3d get_local_camera() const;

                /**
                 * Returns the distance between the current viewer position and the
                 * given bounding box. This distance is measured in the local %terrain
                 * space (with Deformation#getLocalDist), with altitudes divided by
                 * #get_dist_factor() to take deformations into account.
                 */
                float get_camera_dist(const box3d &localBox) const;

            public:
                /**
                 * Returns the visibility of the given bounding box from the current
                 * viewer position. This visibility is computed with
                 * Deformation#getVisbility.
                 */
                e_visibility get_visibility(const box3d &localBox) const;

            public:
                /**
                 * Returns the viewer distance at which a quad is subdivided, relatively
                 * to the quad size. This relative distance is equal to #m_split_factor for
                 * a field of view of 80 degrees and a viewport width of 1024 pixels. It
                 * is larger for smaller field of view angles and/or larger viewports.
                 */
                float get_split_distance() const;

                /**
                 * Returns the ratio between local and deformed lengths at #get_local_camera().
                 */
                float get_dist_factor() const;

            public:
                /**
                 * Updates the %terrain quadtree based on the current viewer position.
                 * The viewer position relatively to the local and deformed %terrain
                 * spaces is computed based on the given SceneNode, which represents
                 * the %terrain position in the scene graph (which also contains the
                 * current viewer position).
                 *
                 * @param m_quad_tree the SceneNode representing the terrain position in
                 *      the global scene graph.
                 */
                void update(c_render_context* render_context, c_transform_node* transform_node);

            public:
                /**
                 * Adds the given bounding box as an occluder. <i>The bounding boxes must
                 * be added in front to back order</i>.
                 *
                 * @param occluder a bounding box in local (i.e. non deformed) coordinates.
                 * @return true is the given bounding box is m_occluded by the bounding boxes
                 *      previously added as occluders by this method.
                 */
                bool add_occluder(const box3d &occluder);

                /**
                 * Returns true if the given bounding box is m_occluded by the bounding boxes
                 * previously added by #addOccluder().
                 *
                 * @param box a bounding box in local (i.e. non deformed) coordinates.
                 * @return true is the given bounding box is m_occluded by the bounding boxes
                 *      previously added as occluders by #add_occluder.
                 **/
                bool is_occluded(const box3d &box);
                
            protected:
                /**
                 * Creates an uninitialized c_quad_tree.
                 */
                c_quad_tree();

            protected:
                /**
                 * Initializes this c_quad_tree.
                 *
                 * @param m_deform the %terrain deformation.
                 * @param m_root the m_root of the %terrain quadtree.
                 * @param m_split_factor how the %terrain quadtree must be subdivided (see
                 *      #m_split_factor).
                 * @param m_max_level the maximum m_level at which the %terrain quadtree must be
                 *      subdivided (inclusive).
                 */
                void init(c_deformation* deform, c_tree_node* root, float splitFactor, int maxLevel);

                void swap(c_quad_tree* node);

            public:
                /**
                 * The deformation of this %terrain. In the %terrain <i>local</i> space the
                 * %terrain sea m_level surface is flat. In the %terrain <i>deformed</i> space
                 * the sea m_level surface can be spherical or cylindrical (or flat if the
                 * identity deformation is used).
                 */
                c_deformation* m_deform;

                /**
                 * The m_root of the %terrain quadtree. This quadtree is subdivided based on the
                 * current viewer position by the #update method.
                 */
                c_tree_node* m_root;

                /**
                 * Describes how the %terrain quadtree must be subdivided based on the viewer
                 * distance. For a field of view of 80 degrees, and a viewport width of 1024
                 * pixels, a quad of size L will be subdivided into subquads if the viewer
                 * distance is less than m_split_factor * L. For a smaller field of view and/or
                 * a larger viewport, the quad will be subdivided at a larger distance, so
                 * that its size in pixels stays more or less the same. This number must be
                 * strictly larger than 1.
                 */
                float m_split_factor;

                /**
                 * True to subdivide invisible quads based on distance, like m_visible ones.
                 * The default value of this flag is false.
                 */
                bool m_split_invisible_quads;

                /**
                 * True to perform m_horizon occlusion culling tests.
                 */
                bool m_horizon_culling;

                /**
                 * The maximum m_level at which the %terrain quadtree must be subdivided (inclusive).
                 * The %terrain quadtree will never be subdivided beyond this m_level, even if the
                 * viewer comes very close to the %terrain.
                 */
                int m_max_level;

            public:
                /**
                 * The %terrain elevation below the current viewer position. This field must be
                 * updated manually by users (the TileSamplerZ class can do this for you).
                 * It is used to compute the 3D distance between the viewer and a quad, to decide
                 * whether this quad must be subdivided or not.
                 */
                static float m_ground_height_at_camera;

                /**
                 * The value #m_ground_height_at_camera will have at the next frame.
                 */
                static float m_next_ground_height_at_camera;
                
            private:
                /**
                 * The current viewer position in the deformed %terrain space (see #m_deform).
                 */
                vec3d m_deformed_camera_pos;

                /**
                 * The current viewer frustum planes in the deformed #terrain space (see #m_deform).
                 */
                vec4d m_deformed_frustum_planes[6];

                /**
                 * The current viewer position in the local %terrain space (see #m_deform).
                 */
                vec3d m_local_camera_pos;

                /**
                 * The viewer distance at which a quad is subdivided, relatively to the quad size.
                 */
                float m_split_distance;

                /**
                 * The ratio between local and deformed lengths at #m_local_camera_pos.
                 */
                float m_distance_factor;

                /**
                 * Local reference frame used to compute m_horizon occlusion culling.
                 */
                mat2f m_local_camera_dir;

                /**
                 * Rasterized m_horizon elevation angle for each azimuth angle.
                 */
                float *m_horizon;
            };
        }
    }
}

#endif //GLADIUS_QUAD_TREE_H
