//
// Created by alexa on 05.12.2015.
//

#include "transform_node.h"


namespace gladius
{
    namespace geometry
    {
        c_transform_node::c_transform_node()
        {
            localToParent        = mat4d::IDENTITY;
            localToWorld         = mat4d::IDENTITY;
            worldToLocalUpToDate = false;
            localBounds          = box3d(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
            localToScreen        = mat4d::IDENTITY;
        }

        c_transform_node::~c_transform_node()
        {
        }

        mat4d c_transform_node::getLocalToParent()
        {
            return localToParent;
        }

        void c_transform_node::setLocalToParent(const mat4d &t)
        {
            localToParent = t;
        }

        mat4d c_transform_node::getLocalToWorld()
        {
            return localToWorld;
        }

        mat4d c_transform_node::getWorldToLocal()
        {
            if (!worldToLocalUpToDate)
            {
                worldToLocal         = localToWorld.inverse();
                worldToLocalUpToDate = true;
            }
            return worldToLocal;
        }

        mat4d c_transform_node::getLocalToCamera()
        {
            return localToCamera;
        }

        mat4d c_transform_node::getLocalToScreen()
        {
            return localToScreen;
        }

        box3d c_transform_node::getLocalBounds()
        {
            return localBounds;
        }

        void c_transform_node::setLocalBounds(const box3d &bounds)
        {
            localBounds = bounds;
        }

        box3d c_transform_node::getWorldBounds()
        {
            return worldBounds;
        }

        vec3d c_transform_node::getWorldPos()
        {
            return worldPos;
        }

        size_t c_transform_node::getChildrenCount()
        {
            return children.size();
        }

        c_transform_node *c_transform_node::getChild(size_t index)
        {
            return children[index];
        }

        void c_transform_node::addChild(c_transform_node *child)
        {
            children.push_back(child);
        }

        void c_transform_node::removeChild(size_t index)
        {
            children.erase(children.begin() + index);
        }

        void c_transform_node::swap(c_transform_node *n)
        {
            std::swap(localToParent, n->localToParent);
            std::swap(children, n->children);
        }

        void c_transform_node::updateLocalToWorld(c_transform_node *parent)
        {
            if (parent != nullptr)
            {
                localToWorld = parent->localToWorld * localToParent;
            }

            for (auto i : children)
            {
                i->updateLocalToWorld(this);
            }

            mat4d localToWorld0 = localToWorld;

            worldBounds = localToWorld0 * localBounds;
            worldPos    = localToWorld0 * vec3d::ZERO;

            for (auto i : children)
            {
                worldBounds = worldBounds.enlarge(i->worldBounds);
            }

            worldToLocalUpToDate = false;
        }

        void c_transform_node::updateLocalToCamera(const mat4d &worldToCamera, const mat4d &cameraToScreen)
        {
            localToCamera = worldToCamera * localToWorld;
            localToScreen = cameraToScreen * localToCamera;

            for (auto i : children)
            {
                i->updateLocalToCamera(worldToCamera, cameraToScreen);
            }
        }
    }
}