#include "path.h"

#include <cassert>

namespace ho {

    std::string Path::project_root_;
    std::string Path::asset_root_;

    std::filesystem::path project_root_path_;
    std::filesystem::path asset_root_path_;

    void Path::Initialize() {
#ifdef HO_PROJECT_ROOT
        project_root_ = HO_PROJECT_ROOT;
        project_root_path_ = std::filesystem::path(project_root_);
#else
        assert(false && "HO_PROJECT_ROOT is not defined!");
#endif

#ifdef HO_ASSET_ROOT
        asset_root_ = HO_ASSET_ROOT;
        asset_root_path_ = std::filesystem::path(asset_root_);
#else
        assert(false && "HO_ASSET_ROOT is not defined!");
#endif
    }

}  // namespace ho
