//
// Created by alexa on 05.12.2015.
//

#include "deformation.h"

#include "quad_tree.h"

namespace gladius
{
    namespace geometry
    {
        namespace space
        {

            c_deformation::c_deformation() //: offsetU(nullptr), cameraU(nullptr), blendingU(nullptr), localToScreenU(nullptr), screenQuadCornersU(nullptr), screenQuadVerticalsU(nullptr)
            {
            }

            c_deformation::~c_deformation()
            {
            }

            vec3d c_deformation::localToDeformed(const vec3d &localPt) const
            {
                return localPt;
            }

            mat4d c_deformation::localToDeformedDifferential(const vec3d &localPt, bool clamp) const
            {
                return mat4d::translate(vec3d(localPt.x, localPt.y, 0.0));
            }

            vec3d c_deformation::deformedToLocal(const vec3d &deformedPt) const
            {
                return deformedPt;
            }

            box2f c_deformation::deformedToLocalBounds(const vec3d &deformedCenter, double deformedRadius) const
            {
                return box2f(deformedCenter.x - deformedRadius, deformedCenter.x + deformedRadius, deformedCenter.y - deformedRadius, deformedCenter.y + deformedRadius);
            }

            mat4d c_deformation::deformedToTangentFrame(const vec3d &deformedPt) const
            {
                return mat4d::translate(vec3d(-deformedPt.x, -deformedPt.y, 0.0));
            }

            float c_deformation::getLocalDist(const vec3d &localPt, const box3d &localBox) const
            {
                return (float) std::max(
                        std::abs(localPt.z - localBox.zmax),
                        std::max(
                                std::min(std::abs(localPt.x - localBox.xmin), std::abs(localPt.x - localBox.xmax)),
                                std::min(std::abs(localPt.y - localBox.ymin), std::abs(localPt.y - localBox.ymax))
                        ));
            }

            e_visibility c_deformation::getVisibility(const c_quad_tree *t, const box3d &localBox) const
            {
                // localBox = deformedBox, so we can compare the deformed frustum with it
                return get_visibility(t->get_deformed_frustum_planes(), localBox);
            }

            /*
void c_deformation::setUniforms(ptr <SceneNode> context, c_quad_tree* n, ptr <Program> prog) const
{
    if (lastNodeProg != prog)
    {
        blendingU      = prog->getUniform2f("deformation.blending");
        localToScreenU = prog->getUniformMatrix4f("deformation.localToScreen");
        tileToTangentU = prog->getUniformMatrix3f("deformation.tileToTangent");
        lastNodeProg   = prog;
    }

    if (blendingU != nullptr)
    {
        float d1 = n->getSplitDistance() + 1.0f;
        float d2 = 2.0f * n->get_split_distance();
        blendingU->set(vec2f(d1, d2 - d1));
    }
    cameraToScreen = context->getOwner()->getCameraToScreen().cast<float>();
    localToScreen  = context->getOwner()->getCameraToScreen() * context->getLocalToCamera();
    if (localToScreenU != nullptr)
    {
        localToScreenU->setMatrix(localToScreen.cast<float>());
    }
    if (tileToTangentU != nullptr)
    {
        vec3d m_local_camera_pos = n->getLocalCamera();
        vec3d worldCamera    = context->getOwner()->getCameraNode()->getWorldPos();
        vec3d deformedCamera = localToDeformed(m_local_camera_pos);
        mat4d A              = localToDeformedDifferential(m_local_camera_pos);
        mat4d B              = deformedToTangentFrame(worldCamera);
        mat4d ltow           = context->getLocalToWorld();
        mat4d ltot           = B * ltow * A;
        localToTangent = mat3f(ltot[0][0], ltot[0][1], ltot[0][3], ltot[1][0], ltot[1][1], ltot[1][3], ltot[3][0], ltot[3][1], ltot[3][3]);
    }
}

void c_deformation::setUniforms(ptr <SceneNode> context, ptr <TerrainQuad> q, ptr <Program> prog) const
{
    if (lastQuadProg != prog)
    {
        offsetU              = prog->getUniform4f("deformation.offset");
        cameraU              = prog->getUniform4f("deformation.camera");
        tileToTangentU       = prog->getUniformMatrix3f("deformation.tileToTangent");
        screenQuadCornersU   = prog->getUniformMatrix4f("deformation.screenQuadCorners");
        screenQuadVerticalsU = prog->getUniformMatrix4f("deformation.screenQuadVerticals");
        lastQuadProg         = prog;
    }

    if (offsetU != nullptr)
    {
        offsetU->set(vec4d(q->ox, q->oy, q->l, q->m_level).cast<float>());
    }
    if (cameraU != nullptr)
    {
        vec3d camera = q->getOwner()->getLocalCamera();
        cameraU->set(vec4f(float((camera.x - q->ox) / q->l), float((camera.y - q->oy) / q->l), float((camera.z - c_quad_tree::m_ground_height_at_camera) / (q->l * q->getOwner()->get_dist_factor())), camera.z));
    }
    if (tileToTangentU != nullptr)
    {
        vec3d c = q->get_tree()->get_local_camera();
        mat3f m = localToTangent * mat3f(q->l, 0.0, q->ox - c.x, 0.0, q->l, q->oy - c.y, 0.0, 0.0, 1.0);
        tileToTangentU->setMatrix(m);
    }

    setScreenUniforms(context, q, prog);
}

void c_deformation::setScreenUniforms(ptr <SceneNode> context, ptr <TerrainQuad> q, ptr <Program> prog) const
{
    vec3d p0 = vec3d(q->ox, q->oy, 0.0);
    vec3d p1 = vec3d(q->ox + q->l, q->oy, 0.0);
    vec3d p2 = vec3d(q->ox, q->oy + q->l, 0.0);
    vec3d p3 = vec3d(q->ox + q->l, q->oy + q->l, 0.0);

    if (screenQuadCornersU != nullptr)
    {
        mat4d corners = mat4d(p0.x, p1.x, p2.x, p3.x, p0.y, p1.y, p2.y, p3.y, p0.z, p1.z, p2.z, p3.z, 1.0, 1.0, 1.0, 1.0);
        screenQuadCornersU->setMatrix((localToScreen * corners).cast<float>());
    }

    if (screenQuadVerticalsU != nullptr)
    {
        mat4d verticals = mat4d(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0);
        screenQuadVerticalsU->setMatrix((localToScreen * verticals).cast<float>());
    }
}
*/
        }
    }
}