#pragma once

#include <filesystem>
#include <string>

#include "core/macros.h"

namespace ho {
    class Path {
       public:
        static void Initialize();

        static ALWAYS_INLINE const std::string& ProjectRoot();
        static ALWAYS_INLINE const std::string& AssetRoot();

        static ALWAYS_INLINE std::string ResolveProjectPath(const std::string& relative);
        static ALWAYS_INLINE std::string ResolveAssetPath(const std::string& relative);
        static ALWAYS_INLINE std::string ResolveFileName(const std::string& path);

       private:
        static void FindProjectRoot();
        static void FindAssetRoot();

       private:
        static std::string project_root_;  // root ~ HORenderer3
        static std::string asset_root_;    // root ~ HORenderer3/assets/
        static std::filesystem::path project_root_path_;
        static std::filesystem::path asset_root_path_;
    };

    const std::string& Path::ProjectRoot() { return project_root_; }
    const std::string& Path::AssetRoot() { return asset_root_; }

    std::string Path::ResolveProjectPath(const std::string& relative) {
        std::filesystem::path combined = project_root_path_ / relative;
        return combined.lexically_normal().string();
    }

    std::string Path::ResolveAssetPath(const std::string& relative) {
        std::filesystem::path combined = asset_root_path_ / relative;
        return combined.lexically_normal().string();
    }

    std::string Path::ResolveFileName(const std::string& path) {
        return std::filesystem::path(path).filename().string();
    }
}  // namespace ho
