#pragma once

#include <cstdint>
#include <functional>
#include <limits>

#include "core/macros.h"

namespace ho {
    template <typename T>
    class ID {
       public:
        constexpr ID() : id_(0) {}

        static const ID NULL_ID;

        constexpr uint64_t id() const { return id_; }

        constexpr bool operator==(const ID<T>& rhs) const { return id_ == rhs.id_; }
        constexpr bool operator!=(const ID<T>& rhs) const { return !(*this == rhs); }

        constexpr bool IsNULL() const { return id_ == 0ull; }
        constexpr bool IsValid() const { return !IsNULL(); }

        constexpr ID& Increment() {
            assert(id_ != std::numeric_limits<uint64_t>::max());
            id_++;
            return *this;
        }

        constexpr ID& Decrement() {
            assert(id_ != 0ull);
            id_--;
            return *this;
        }

       private:
        uint64_t id_;
    };

    template <typename T>
    _INLINE_ constexpr ID<T> ID<T>::NULL_ID{};

    // dummy structs
    struct OwnedByResourceManager {};
    struct OwnedByThreadPool {};

    using ResourceID = ID<OwnedByResourceManager>;
    using ThreadID = ID<OwnedByThreadPool>;
}  // namespace ho

namespace std {
    template <typename T>
    struct hash<ho::ID<T>> {
        std::size_t operator()(const ho::ID<T>& id) const { return std::hash<uint64_t>{}(id.id()); }
    };
}  // namespace std