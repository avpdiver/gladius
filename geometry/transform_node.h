//
// Created by alexa on 05.12.2015.
//

#ifndef GLADIUS_TRANSFORM_NODE_H
#define GLADIUS_TRANSFORM_NODE_H

#include "../core/math/mat4.h"

using namespace gladius::math;

namespace gladius
{
    namespace geometry
    {
        class c_transform_node
        {
        public:
            c_transform_node();
            virtual ~c_transform_node();

        public:
            /**
             * Returns the transformation from this node to its m_parent node.
             */
            mat4d getLocalToParent();

            /**
             * Sets the transformation from this node to its m_parent node.
             *
             * @param t the new localToParent transformation.
             */
            void setLocalToParent(const mat4d &t);

            /**
             * Returns the transformation from this node to the m_root node.
             */
            mat4d getLocalToWorld();

            /**
             * Returns the transformation from the m_root node to this node.
             */
            mat4d getWorldToLocal();

            /**
             * Returns the transformation from this node to the camera node.
             */
            mat4d getLocalToCamera();

            /**
             * Returns the tranformation from this node to the screen. This is the
             * transformation from this node to the camera node, followed by the
             * transformation from the camera space to the screen space (defined by the
             * cameraToScreen mat4 uniform of the camera node).
             */
            mat4d getLocalToScreen();

            /**
             * Returns the bounding box of this node in local coordinates.
             */
            box3d getLocalBounds();

            /**
             * Sets the bounding box of this node in local coordinates.
             */
            void setLocalBounds(const box3d &bounds);

            /**
             * Returns the bounding box of this node in world coordinates.
             */
            box3d getWorldBounds();

            /**
             * Returns the origin of the local reference frame in world coordinates.
             */
            vec3d getWorldPos();

        public:
            /**
             * Returns the number of child node of this node.
             */
            size_t getChildrenCount();

            /**
             * Returns the child node of this node whose index is given.
             *
             * @param index a child node index between 0 and #getChildrenCount - 1.
             */
            c_transform_node* getChild(size_t index);

            /**
             * Adds a child node to this node.
             *
             * @param child a child node.
             */
            void addChild(c_transform_node* child);

            /**
             * Removes a child node from this node.
             *
             * @param index a child node index between 0 and #getChildrenCount - 1.
             */
            void removeChild(size_t index);

        protected:
            /**
             * Swaps this scene node with the given one.
             */
            void swap(c_transform_node* n);

        private:
            /**
             * Updates the #localToWorld transform. This method also updates #worldBounds
             * and #worldPos.
             *
             * @param m_parent the m_parent node of this node.
             */
            void updateLocalToWorld(c_transform_node* parent);

            /**
             * Updates the #localToCamera and the #localToScreen transforms.
             *
             * @param worldToCamera the world to camera transform.
             * @param cameraToScreen the camera to screen transform.
             */
            void updateLocalToCamera(const mat4d &worldToCamera, const mat4d &cameraToScreen);

        public:
            /**
            * True if this scene node is m_visible, false otherwise.
            */
            bool isVisible;

        private:
            /**
             * The transformation from this node to its m_parent node.
             */
            mat4d localToParent;

            /**
             * The transformation from this node to the m_root node.
             */
            mat4d localToWorld;

            /**
             * The transformation from the m_root node to this node.
             */
            mat4d worldToLocal;

            /**
             * The transformation from this node to the camera node.
             */
            mat4d localToCamera;

            /**
             * The transformation from this node to the screen.
             */
            mat4d localToScreen;

            /**
             * The bounding box of this node in local coordinates.
             */
            box3d localBounds;

            /**
             * The bounding box of this node in world coordinates.
             */
            box3d worldBounds;

            /**
             * The origin of the local reference frame of this node in world coordinates.
             */
            vec3d worldPos;

            /**
             * True if the #worldToLocal transform is up to date.
             */
            bool worldToLocalUpToDate;

        private:
            /**
             * The child nodes of this node.
             */
            std::vector<c_transform_node*> children;
        };
    }
}

#endif //GLADIUS_TRANSFORM_NODE_H
