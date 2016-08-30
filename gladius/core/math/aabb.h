//
// Created by pav on 18.02.16.
//

#ifndef GLADIUS_AABB_H
#define GLADIUS_AABB_H

#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/gtx/component_wise.hpp"

namespace gladius {
namespace math {

template<typename T>
class aabb {
public:
    aabb() {
        set_null();
    }

    aabb(const glm::tvec3<T> &center, T radius) {
        set_null();
        extend(center, radius);
    }

    aabb(const glm::tvec3<T> &p1, const glm::tvec3<T> &p2) {
        set_null();
        extend(p1);
        extend(p2);
    }

    aabb(const aabb &aabb) {
        set_null();
        extend(aabb);
    }

public:
    ~aabb() {
    }

public:
    void set_null() {
        m_min = glm::tvec3<T>(1.0);
        m_max = glm::tvec3<T>(-1.0);
    }

    bool is_null() const {
        return m_min.x > m_max.x || m_min.y > m_max.y || m_min.z > m_max.z;
    }

public:
    void extend(T val) {
        if (!is_null()) {
            m_min -= glm::tvec3<T>(val);
            m_max += glm::tvec3<T>(val);
        }
    }

    void extend(const glm::tvec3<T> &p) {
        if (!is_null()) {
            m_min = glm::min(p, m_min);
            m_max = glm::max(p, m_max);
        } else {
            m_min = p;
            m_max = p;
        }
    }

    void extend(const glm::tvec3<T> &p, T radius) {
        glm::tvec3<T> r(radius);
        if (!is_null()) {
            m_min = glm::min(p - r, m_min);
            m_max = glm::max(p + r, m_max);
        } else {
            m_min = p - r;
            m_max = p + r;
        }
    }

    void extend(const aabb &aabb) {
        if (!aabb.is_null()) {
            extend(aabb.m_min);
            extend(aabb.m_max);
        }
    }

public:
    glm::tvec3<T> get_diagonal() const {
        if (!is_null())
            return m_max - m_min;
        else
            return glm::tvec3<T>(0);
    }

    T get_longest_edge() const {
        return glm::compMax(get_diagonal());
    }

    T get_shortest_edge() const {
        return glm::compMin(get_diagonal());
    }

    glm::tvec3<T> get_center() const {
        if (!is_null()) {
            glm::tvec3<T> d = get_diagonal();
            return m_min + (d * T(0.5));
        } else {
            return glm::tvec3<T>(0.0);
        }
    }

public:
    void translate(const glm::tvec3<T> &v) {
        if (!is_null()) {
            m_min += v;
            m_max += v;
        }
    }

    void scale(const glm::tvec3<T> &s, const glm::tvec3<T> &o) {
        if (!is_null()) {
            m_min -= o;
            m_max -= o;

            m_min *= s;
            m_max *= s;

            m_min += o;
            m_max += o;
        }
    }

public:
    bool overlaps(const aabb &bb) const {
        if (is_null() || bb.is_null())
            return false;

        if (bb.m_min.x > m_max.x || bb.m_max.x < m_min.x)
            return false;
        else if (bb.m_min.y > m_max.y || bb.m_max.y < m_min.y)
            return false;
        else if (bb.m_min.z > m_max.z || bb.m_max.z < m_min.z)
            return false;

        return true;
    }

    int intersect(const aabb &b) const {
        if (is_null() || b.is_null())
            return -1;

        if ((m_max.x < b.m_min.x) || (m_min.x > b.m_max.x) ||
            (m_max.y < b.m_min.y) || (m_min.y > b.m_max.y) ||
            (m_max.z < b.m_min.z) || (m_min.z > b.m_max.z)) {
            return -1;
        }

        if ((m_min.x <= b.m_min.x) && (m_max.x >= b.m_max.x) &&
            (m_min.y <= b.m_min.y) && (m_max.y >= b.m_max.y) &&
            (m_min.z <= b.m_min.z) && (m_max.z >= b.m_max.z)) {
            return 1;
        }

        return 0;
    }

    bool is_equals(const aabb &b, T diff) const {
        if (is_null() || b.is_null()) return false;

        glm::tvec3<T> acceptable_diff = ((get_diagonal() + b.get_diagonal()) / T(2.0)) * diff;
        glm::tvec3<T> min_diff(m_min - b.m_min);
        min_diff = glm::tvec3<T>(std::abs(min_diff.x), std::abs(min_diff.y), std::abs(min_diff.z));
        if (min_diff.x > acceptable_diff.x) return false;
        if (min_diff.y > acceptable_diff.y) return false;
        if (min_diff.z > acceptable_diff.z) return false;
        glm::tvec3<T> max_diff(m_max - b.m_max);
        max_diff = glm::tvec3<T>(std::abs(max_diff.x), std::abs(max_diff.y), std::abs(max_diff.z));
        if (max_diff.x > acceptable_diff.x) return false;
        if (max_diff.y > acceptable_diff.y) return false;
        if (max_diff.z > acceptable_diff.z) return false;
        return true;
    }

public:
    glm::tvec3<T> m_min;
    glm::tvec3<T> m_max;
};

}
}

#endif //GLADIUS_AABB_H
