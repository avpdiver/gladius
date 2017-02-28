//
// Created by avpdiver on 31.08.16.
//

#ifndef GLADIUS_GLADIUS_H
#define GLADIUS_GLADIUS_H

#include <string>

#include "core/window/window.h"
#include "graphics/render3d/render3d.h"
#include "core/types.h"

namespace gladius {

struct s_gladius_desc {
public:
    s_gladius_desc& set_name(const char* name_) {
        name = name_;
        return *this;
    }

    s_gladius_desc& logging_enabled(bool logging_) {
        name = logging_;
        return *this;
    }

    s_gladius_desc& set_window_desc(core::s_window_desc& window_desc_) {
        window_desc = window_desc_;
        return *this;
    }

public:
    std::string name = "gladius";
    bool logging = true;

    core::s_window_desc window_desc;
};

class c_gladius {
public:
    c_gladius() {}

private:
    NOT_COPYABLE(c_gladius);
    NOT_MOVEABLE(c_gladius);

public:
    bool start(const s_gladius_desc& desc_);

protected:
    void shutdown();

protected:
    core::c_window m_window;
};

}

#endif //GLADIUS_GLADIUS_H
