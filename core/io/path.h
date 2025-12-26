#pragma once

#include <filesystem>
#include <string>

#include "core/macros.h"

namespace ho {
    class Path {
       public:
        Path() = delete;
        Path(const Path& path) = default;
        explicit Path(const std::string& path) : path_(path) {}
        Path(Path&& path) = default;

        ALWAYS_INLINE Path& operator=(const Path& rhs);
        ALWAYS_INLINE Path& operator=(Path&& rhs);

        ALWAYS_INLINE bool operator==(const Path& rhs) const;
        ALWAYS_INLINE bool operator!=(Path&& rhs) const;

        ALWAYS_INLINE Path operator/(const Path& rhs) const;

        ALWAYS_INLINE Path GetParentPath() const;
        ALWAYS_INLINE Path GetFileName() const;

        ALWAYS_INLINE Path& ResolveProjectPath();
        ALWAYS_INLINE Path& ResolveAssetPath();

        ALWAYS_INLINE Path ResolvedProjectPath() const;
        ALWAYS_INLINE Path ResolvedAssetPath() const;

        ALWAYS_INLINE std::string ToString() const;

       private:
        Path(const std::filesystem::path& path) : path_(path) {}

        static std::filesystem::path project_root_path_;  // root ~ HORenderer3
        static std::filesystem::path asset_root_path_;    // root ~ HORenderer3/assets/

        std::filesystem::path path_;
    };

    Path& Path::operator=(const Path& rhs) {
        if (this != &rhs) {
            path_ = rhs.path_;
        }
        return *this;
    }
    Path& Path::operator=(Path&& rhs) {
        if (this != &rhs) {
            path_ = std::move(rhs.path_);
        }
        return *this;
    }

    bool Path::operator==(const Path& rhs) const { return path_ == rhs.path_; }
    bool Path::operator!=(Path&& rhs) const { return !(*this == rhs); }

    Path Path::operator/(const Path& rhs) const { return Path(path_ / rhs.path_); }

    Path Path::GetParentPath() const { return path_.parent_path(); }

    Path Path::GetFileName() const { return Path(path_.filename()); }

    Path Path::ResolvedProjectPath() const {
        Path copy = *this;
        return copy.ResolveProjectPath();
        }

    Path Path::ResolvedAssetPath() const {
        Path copy = *this;
        return copy.ResolveAssetPath();
    }

    Path& Path::ResolveProjectPath() {
        if (!path_.is_absolute()) {
            path_ = (project_root_path_ / path_).lexically_normal();
        }
        return *this;
    }
    Path& Path::ResolveAssetPath() {
        if (!path_.is_absolute()) {
            path_ = (asset_root_path_ / path_).lexically_normal();
        }
        return *this;
    }

    std::string Path::ToString() const { return path_.string(); }

}  // namespace ho
