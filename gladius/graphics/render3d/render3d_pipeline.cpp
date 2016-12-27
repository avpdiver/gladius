//
// Created by alexa on 028 28 08 2016.
//

#include "../../core/filesystem/filesystem.h"
#include "../../core/filesystem/json_file.h"

#include "render3d_globals.h"
#include "render3d_pipeline.h"
#include "pipeline/pipeline_desc.h"


namespace gladius {
namespace graphics {
namespace render3d {

s_pipelines_json g_pipelines_json;

bool load_pipeline(const char *filename) {
    core::filesystem::c_json_file *file = reinterpret_cast<core::filesystem::c_json_file *>(
            core::filesystem::open("disk:json", filename, core::filesystem::e_file_mode::read));
    file->read(g_pipelines_json);
    core::filesystem::close(file);
    return true;
}

bool create_pipeline() {
    s_pipelines_desc pipelines_desc;
    VERIFY(g_pipelines_json.create(pipelines_desc));
    return pipelines_desc.create();
}

}
}
}