#pragma once

#include <memory>
#include <unordered_map>

#include "id.h"

namespace ho {
template <typename ID_type, typename value_type>
class ID_Owner {
 public:
  ID_Owner() : base_id_(), id_to_val_() {}

  // Copy ver (only if value_type is copy constructible)
  template <typename U = value_type,
            typename = std::enable_if_t<std::is_copy_constructible_v<U>>>
  ID_type Enroll(const U& v) {
    base_id_.Increment();
    ID_type id = base_id_;
    id_to_val_.emplace(id, std::make_unique<U>(v));
    return id;
  }

  // Copy ver (only if value_type is move constructible)
  template <typename U = value_type,
            typename = std::enable_if_t<std::is_move_constructible_v<U>>>
  ID_type Enroll(value_type&& v) {
    base_id_.Increment();
    ID_type id = base_id_;
    id_to_val_.emplace(id, std::make_unique<value_type>(std::move(v)));  // move
    return id;
  }

  // In-place construct ver
  template <typename... Args>
  ID_type Emplace(Args&&... args) {
    base_id_.Increment();
    ID_type id = base_id_;
    id_to_val_.emplace(
        id, std::make_unique<value_type>(std::forward<Args>(args)...));
    return id;
  }

  bool Delete(const ID_type& id) {
    if (id.IsNULL()) {
      return false;
    }
    return id_to_val_.erase(id) > 0;
  }

  value_type* Get(const ID_type& id) {
    if (id.IsNULL()) {
      return nullptr;
    }
    auto it = id_to_val_.find(id);
    return (it == id_to_val_.end()) ? nullptr : it->second.get();
  }

  const value_type* Get(const ID_type& id) const {
    if (id.IsNULL()) {
      return nullptr;
    }
    auto it = id_to_val_.find(id);
    return (it == id_to_val_.end()) ? nullptr : it->second.get();
  }

  bool Has(const ID_type& id) const {
    return !id.IsNULL() && id_to_val_.find(id) != id_to_val_.end();
  }

  void Clear() { id_to_val_.clear(); }
  std::size_t Size() const { return id_to_val_.size(); }

 private:
  ID_type base_id_;
  std::unordered_map<ID_type, std::unique_ptr<value_type>> id_to_val_;
};
}  // namespace ho