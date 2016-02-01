//
// Created by alexa on 27.12.2015.
//

#ifndef GLADIUS_SPHERICAL_DEFORMATION_H
#define GLADIUS_SPHERICAL_DEFORMATION_H

#include "../deformation.h"

using namespace gladius::math;

namespace gladius
{
    namespace geometry
    {
        namespace space
        {
            class c_spherical_deformation : public c_deformation
            {
            public:
                /**
                 * Creates a new c_spherical_deformation.
                 *
                 * @param R the radius of the sphere into which the plane z=0 must be
                 *      deformed.
                 */
                c_spherical_deformation(float R);

                /**
                 * Deletes this c_spherical_deformation.
                 */
                virtual ~c_spherical_deformation();

            public:

                virtual vec3d localToDeformed(const vec3d &localPt) const;

                virtual mat4d localToDeformedDifferential(const vec3d &localPt, bool clamp = false) const;

                virtual vec3d deformedToLocal(const vec3d &worldPt) const;

                virtual box2f deformedToLocalBounds(const vec3d &deformedCenter, double deformedRadius) const;

                virtual mat4d deformedToTangentFrame(const vec3d &deformedPt) const;

            public:
                /*
                virtual void setUniforms(ptr <SceneNode> context, ptr <TerrainNode> n, ptr <Program> prog) const;
                virtual void setUniforms(ptr <SceneNode> context, ptr <TerrainQuad> q, ptr <Program> prog) const;
                 */
                virtual e_visibility getVisibility(const c_quad_tree *t, const box3d &localBox) const;


            private:
                static e_visibility getVisibility(const vec4d &clip, const vec3d *b, double f);

            protected:
                //virtual void setScreenUniforms(ptr <SceneNode> context, ptr <TerrainQuad> q, ptr <Program> prog) const;

            public:
                /**
                 * The radius of the sphere into which the plane z=0 must be deformed.
                 */
                const float R;

            private:
                /**
                 * The radius of the deformation.
                 */
                //mutable ptr <Uniform1f> radiusU;

                /**
                 * The transformation from local space to world space.
                 */
                //mutable ptr <UniformMatrix3f> localToWorldU;

                /**
                 * The norms of the (x,y,R) vectors corresponding to
                 * the corners of a quad.
                 */
                //mutable ptr <Uniform4f> screenQuadCornerNormsU;

                /**
                 * The transformation from the tangent space at a quad's center to
                 * world space.
                 */
                //mutable ptr <UniformMatrix3f> tangentFrameToWorldU;
            };
        }
    }
}

#endif //GLADIUS_SPHERICAL_DEFORMATION_H
