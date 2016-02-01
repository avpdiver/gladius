//
// Created by alexa on 27.12.2015.
//

#include "spherical_deformation.h"
#include "../quad_tree.h"

namespace gladius
{
    namespace geometry
    {
        namespace space
        {
            c_spherical_deformation::c_spherical_deformation(float R) : c_deformation(), R(R) //, radiusU(NULL), localToWorldU(NULL), screenQuadCornerNormsU(NULL), tangentFrameToWorldU(NULL)
            {
            }

            c_spherical_deformation::~c_spherical_deformation()
            {
            }

            vec3d c_spherical_deformation::localToDeformed(const vec3d &localPt) const
            {
                return vec3d(localPt.x, localPt.y, R).normalize(localPt.z + R);
            }

            mat4d c_spherical_deformation::localToDeformedDifferential(const vec3d &localPt, bool clamp) const
            {
                if (!std::isfinite(localPt.x) || !std::isfinite(localPt.y) || !std::isfinite(localPt.z))
                {
                    return mat4d::IDENTITY;
                }

                vec3d pt = localPt;
                if (clamp)
                {
                    pt.x = pt.x - floor((pt.x + R) / (2.0 * R)) * 2.0 * R;
                    pt.y = pt.y - floor((pt.y + R) / (2.0 * R)) * 2.0 * R;
                }

                double l  = pt.x * pt.x + pt.y * pt.y + R * R;
                double c0 = 1.0 / sqrt(l);
                double c1 = c0 * R / l;

                return mat4d((pt.y * pt.y + R * R) * c1, -pt.x * pt.y * c1, pt.x * c0, R * pt.x * c0, -pt.x * pt.y * c1, (pt.x * pt.x + R * R) * c1, pt.y * c0, R * pt.y * c0, -pt.x * R * c1, -pt.y * R * c1, R * c0, (R * R) * c0, 0.0, 0.0, 0.0, 1.0);
            }

            vec3d c_spherical_deformation::deformedToLocal(const vec3d &deformedPt) const
            {
                double l = deformedPt.length();
                if (deformedPt.z >= abs(deformedPt.x) && deformedPt.z >= abs(deformedPt.y))
                {
                    return vec3d(deformedPt.x / deformedPt.z * R, deformedPt.y / deformedPt.z * R, l - R);
                }
                if (deformedPt.z <= -abs(deformedPt.x) && deformedPt.z <= -abs(deformedPt.y))
                {
                    return vec3d(INFINITY, INFINITY, INFINITY);
                }
                if (deformedPt.y >= abs(deformedPt.x) && deformedPt.y >= abs(deformedPt.z))
                {
                    return vec3d(deformedPt.x / deformedPt.y * R, (2.0 - deformedPt.z / deformedPt.y) * R, l - R);
                }
                if (deformedPt.y <= -abs(deformedPt.x) && deformedPt.y <= -abs(deformedPt.z))
                {
                    return vec3d(-deformedPt.x / deformedPt.y * R, (-2.0 - deformedPt.z / deformedPt.y) * R, l - R);
                }
                if (deformedPt.x >= abs(deformedPt.y) && deformedPt.x >= abs(deformedPt.z))
                {
                    return vec3d((2.0 - deformedPt.z / deformedPt.x) * R, deformedPt.y / deformedPt.x * R, l - R);
                }
                if (deformedPt.x <= -abs(deformedPt.y) && deformedPt.x <= -abs(deformedPt.z))
                {
                    return vec3d((-2.0 - deformedPt.z / deformedPt.x) * R, -deformedPt.y / deformedPt.x * R, l - R);
                }

                return vec3d::ZERO;
            }

            box2f c_spherical_deformation::deformedToLocalBounds(const vec3d &deformedCenter, double deformedRadius) const
            {
                vec3d  p = deformedToLocal(deformedCenter);
                double r = deformedRadius;

                if (std::isinf(p.x) || std::isinf(p.y))
                {
                    return box2f();
                }

                double k  = (1.0 - r * r / (2.0 * R * R)) * vec3d(p.x, p.y, R).length();
                double A  = k * k - p.x * p.x;
                double B  = k * k - p.y * p.y;
                double C  = -2.0 * p.x * p.y;
                double D  = -2.0 * R * R * p.x;
                double E  = -2.0 * R * R * p.y;
                double F  = R * R * (k * k - R * R);
                double a  = C * C - 4.0 * A * B;
                double b  = 2.0 * C * E - 4.0 * B * D;
                double c  = E * E - 4.0 * B * F;
                double d  = sqrt(b * b - 4.0 * a * c);
                double x1 = (-b - d) / (2.0 * a);
                double x2 = (-b + d) / (2.0 * a);

                b = 2.0 * C * D - 4.0 * A * E;
                c = D * D - 4.0 * A * F;
                d = sqrt(b * b - 4.0 * a * c);

                double y1 = (-b - d) / (2.0 * a);
                double y2 = (-b + d) / (2.0 * a);

                return box2f(vec2f(x1, y1), vec2f(x2, y2));
            }

            mat4d c_spherical_deformation::deformedToTangentFrame(const vec3d &deformedPt) const
            {
                vec3d Uz = deformedPt.normalize();
                vec3d Ux = (vec3d::UNIT_Y.cross(Uz)).normalize();
                vec3d Uy = Uz.cross(Ux);
                return mat4d(Ux.x, Ux.y, Ux.z, 0.0, Uy.x, Uy.y, Uy.z, 0.0, Uz.x, Uz.y, Uz.z, -R, 0.0, 0.0, 0.0, 1.0);
            }

            e_visibility c_spherical_deformation::getVisibility(const c_quad_tree *t, const box3d &localBox) const
            {
                vec3d deformedBox[4];

                deformedBox[0] = localToDeformed(vec3d(localBox.xmin, localBox.ymin, localBox.zmin));
                deformedBox[1] = localToDeformed(vec3d(localBox.xmax, localBox.ymin, localBox.zmin));
                deformedBox[2] = localToDeformed(vec3d(localBox.xmax, localBox.ymax, localBox.zmin));
                deformedBox[3] = localToDeformed(vec3d(localBox.xmin, localBox.ymax, localBox.zmin));

                double      a                      = (localBox.zmax + R) / (localBox.zmin + R);
                double      dx                     = (localBox.xmax - localBox.xmin) / 2 * a;
                double      dy                     = (localBox.ymax - localBox.ymin) / 2 * a;
                double      dz                     = localBox.zmax + R;
                double      f                      = sqrt(dx * dx + dy * dy + dz * dz) / (localBox.zmin + R);
                const vec4d *deformedFrustumPlanes = t->get_deformed_frustum_planes();

                e_visibility v0 = getVisibility(deformedFrustumPlanes[0], deformedBox, f);
                if (v0 == e_visibility::INVISIBLE)
                {
                    return e_visibility::INVISIBLE;
                }

                e_visibility v1 = getVisibility(deformedFrustumPlanes[1], deformedBox, f);
                if (v1 == e_visibility::INVISIBLE)
                {
                    return e_visibility::INVISIBLE;
                }

                e_visibility v2 = getVisibility(deformedFrustumPlanes[2], deformedBox, f);
                if (v2 == e_visibility::INVISIBLE)
                {
                    return e_visibility::INVISIBLE;
                }

                e_visibility v3 = getVisibility(deformedFrustumPlanes[3], deformedBox, f);
                if (v3 == e_visibility::INVISIBLE)
                {
                    return e_visibility::INVISIBLE;
                }

                e_visibility v4 = getVisibility(deformedFrustumPlanes[4], deformedBox, f);
                if (v4 == e_visibility::INVISIBLE)
                {
                    return e_visibility::INVISIBLE;
                }

                vec3d  c        = t->get_deformed_camera();
                double lSq      = c.squaredLength();
                double rm       = R + std::min(0.0, localBox.zmin);
                double rM       = R + localBox.zmax;
                double rmSq     = rm * rm;
                double rMSq     = rM * rM;
                vec4d  farPlane = vec4d(c.x, c.y, c.z, sqrt((lSq - rmSq) * (rMSq - rmSq)) - rmSq);

                e_visibility v5 = getVisibility(farPlane, deformedBox, f);
                if (v5 == e_visibility::INVISIBLE)
                {
                    return e_visibility::INVISIBLE;
                }

                if (v0 == e_visibility::VISIBLE && v1 == e_visibility::VISIBLE && v2 == e_visibility::VISIBLE && v3 == e_visibility::VISIBLE && v4 == e_visibility::VISIBLE && v5 == e_visibility::VISIBLE)
                {
                    return e_visibility::VISIBLE;
                }

                return e_visibility::PARTIALLY_VISIBLE;
            }

            e_visibility c_spherical_deformation::getVisibility(const vec4d &clip, const vec3d *b, double f)
            {
                double o = b[0].x * clip.x + b[0].y * clip.y + b[0].z * clip.z;
                bool   p = o + clip.w > 0.0;
                if ((o * f + clip.w > 0.0) == p)
                {
                    o = b[1].x * clip.x + b[1].y * clip.y + b[1].z * clip.z;
                    if ((o + clip.w > 0.0) == p && (o * f + clip.w > 0.0) == p)
                    {
                        o = b[2].x * clip.x + b[2].y * clip.y + b[2].z * clip.z;
                        if ((o + clip.w > 0.0) == p && (o * f + clip.w > 0.0) == p)
                        {
                            o = b[3].x * clip.x + b[3].y * clip.y + b[3].z * clip.z;
                            return (o + clip.w > 0.0) == p && (o * f + clip.w > 0.0) == p ? (p ? e_visibility::VISIBLE : e_visibility::INVISIBLE) : e_visibility::PARTIALLY_VISIBLE;
                        }
                    }
                }
                return e_visibility::PARTIALLY_VISIBLE;
            }

            /*                          
            void c_spherical_deformation::setUniforms(ptr <SceneNode> context, ptr <c_quad_tree> n, ptr <Program> prog) const
            {
                if (lastNodeProg != prog)
                {
                    radiusU       = prog->getUniform1f("deformation.radius");
                    localToWorldU = prog->getUniformMatrix3f("deformation.localToWorld");
                }
                Deformation::setUniforms(context, n, prog);
                if (localToWorldU != NULL)
                {
                    mat4d ltow = context->getLocalToWorld();
                    localToWorldU->setMatrix(mat3d(ltow[0][0], ltow[0][1], ltow[0][2], ltow[1][0], ltow[1][1], ltow[1][2], ltow[2][0], ltow[2][1], ltow[2][2]).cast<float>());
                }
                if (radiusU != NULL)
                {
                    radiusU->set(R);
                }
            }

            void c_spherical_deformation::setUniforms(ptr <SceneNode> context, ptr <TerrainQuad> q, ptr <Program> prog) const
            {
                if (lastQuadProg != prog)
                {
                    screenQuadCornerNormsU = prog->getUniform4f("deformation.screenQuadCornerNorms");
                    tangentFrameToWorldU   = prog->getUniformMatrix3f("deformation.tangentFrameToWorld");
                }
                Deformation::setUniforms(context, q, prog);
            }

            void c_spherical_deformation::setScreenUniforms(ptr <SceneNode> context, ptr <TerrainQuad> q, ptr <Program> prog) const
            {
                vec3d  p0 = vec3d(q->ox, q->oy, R);
                vec3d  p1 = vec3d(q->ox + q->l, q->oy, R);
                vec3d  p2 = vec3d(q->ox, q->oy + q->l, R);
                vec3d  p3 = vec3d(q->ox + q->l, q->oy + q->l, R);
                vec3d  pc = (p0 + p3) * 0.5;
                double l0, l1, l2, l3;
                vec3d  v0 = p0.normalize(&l0);
                vec3d  v1 = p1.normalize(&l1);
                vec3d  v2 = p2.normalize(&l2);
                vec3d  v3 = p3.normalize(&l3);
                if (screenQuadCornersU != NULL)
                {
                    mat4d deformedCorners = mat4d(v0.x * R, v1.x * R, v2.x * R, v3.x * R, v0.y * R, v1.y * R, v2.y * R, v3.y * R, v0.z * R, v1.z * R, v2.z * R, v3.z * R, 1.0, 1.0, 1.0, 1.0);
                    screenQuadCornersU->setMatrix((localToScreen * deformedCorners).cast<float>());
                }
                if (screenQuadVerticalsU != NULL)
                {
                    mat4d deformedVerticals = mat4d(v0.x, v1.x, v2.x, v3.x, v0.y, v1.y, v2.y, v3.y, v0.z, v1.z, v2.z, v3.z, 0.0, 0.0, 0.0, 0.0);
                    screenQuadVerticalsU->setMatrix((localToScreen * deformedVerticals).cast<float>());
                }
                if (screenQuadCornerNormsU != NULL)
                {
                    screenQuadCornerNormsU->set(vec4d(l0, l1, l2, l3).cast<float>());
                }
                if (tangentFrameToWorldU != NULL)
                {
                    vec3d uz                  = pc.normalize();
                    vec3d ux                  = vec3d::UNIT_Y.cross(uz).normalize();
                    vec3d uy                  = uz.cross(ux);
                    mat4d ltow                = context->getLocalToWorld();
                    mat3d tangentFrameToWorld = mat3d(ltow[0][0], ltow[0][1], ltow[0][2], ltow[1][0], ltow[1][1], ltow[1][2], ltow[2][0], ltow[2][1], ltow[2][2]) * mat3d(ux.x, uy.x, uz.x, ux.y, uy.y, uz.y, ux.z, uy.z, uz.z);
                    tangentFrameToWorldU->setMatrix(tangentFrameToWorld.cast<float>());
                }
            }
             
             */
        }
    }
}