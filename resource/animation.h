#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/math/quaternion.h"
#include "core/math/transform_3d.h"
#include "core/math/vector3.h"

namespace ho {

struct Animation {
  enum ExtrapolationMode : uint8_t {
    // Outside the animation time range, fall back to the node's
    // original (bind/default) local transform.
    EXTRAP_MODE_DEFAULT = 0,

    // Outside the animation time range, keep using the closest
    // animation key value without any interpolation.
    EXTRAP_MODE_CONSTANT = 1,

    // Outside the animation time range, estimate the value by
    // extending the curve linearly using the two nearest keys.
    EXTRAP_MODE_LINEAR = 2,

    // Loop the animation time back into the valid key range and
    // evaluate the animation as if it were repeating endlessly.
    EXTRAP_MODE_REPEAT = 3
  };

  enum InterpolationMode : uint8_t {
    // The value stays constant until the next keyframe is reached.
    // No interpolation is performed between keys.
    INTERP_MODE_STEP = 0,

    // Interpolate linearly between two neighboring key values.
    // Used for translations, scales, and any linear quantity.
    INTERP_MODE_LINEAR = 1,

    // Interpolate rotations using spherical linear interpolation (slerp).
    // Ensures constant angular velocity and avoids artifacts of linear
    // interpolation.
    INTERP_MODE_SPHERICAL_LINEAR = 2,

    // Interpolate using cubic splines defined by in/out tangents.
    // Produces smooth C1-continuous curves between keyframes.
    INTERP_MODE_CUBIC_SPLINE = 3
  };

  struct TranslationKey {
    real time;
    Vector3 translation;
  };

  struct RotationKey {
    real time;
    Quaternion rotation;
  };

  struct ScalingKey {
    real time;
    Vector3 scale;
  };

  struct MorphingKey {
    MorphingKey() = default;
    MorphingKey(real p_time, std::vector<uint32_t>&& p_target_indices,
                std::vector<real>&& p_weights) noexcept
        : time(p_time),
          target_indices(std::move(p_target_indices)),
          weights(std::move(p_weights)) {}

    MorphingKey(MorphingKey&& rhs) noexcept = default;
    MorphingKey& operator=(MorphingKey&& rhs) noexcept = default;

    MorphingKey(const MorphingKey& rhs) = delete;
    MorphingKey& operator=(const MorphingKey& rhs) = delete;

    real time;
    std::vector<uint32_t> target_indices;
    std::vector<real> weights;
  };

  struct SkeletalTrack {
    SkeletalTrack() = default;
    SkeletalTrack(uint32_t p_bone_index,
                  std::vector<TranslationKey>&& p_translation_key_sequence,
                  InterpolationMode p_translation_interp_mode,
                  std::vector<RotationKey>&& p_rotation_key_sequence,
                  InterpolationMode p_rotation_interp_mode,
                  std::vector<ScalingKey>&& p_scaling_key_sequence,
                  InterpolationMode p_scaling_interp_mode,
                  ExtrapolationMode p_pre_mode,
                  ExtrapolationMode p_post_mode) noexcept
        : bone_index(p_bone_index),
          translation_key_sequence(std::move(p_translation_key_sequence)),
          translation_interp_mode(p_translation_interp_mode),
          rotation_key_sequence(std::move(p_rotation_key_sequence)),
          rotation_interp_mode(p_rotation_interp_mode),
          scaling_key_sequence(std::move(p_scaling_key_sequence)),
          scaling_interp_mode(p_scaling_interp_mode),
          pre_mode(p_pre_mode),
          post_mode(p_post_mode) {}

    SkeletalTrack(SkeletalTrack&& rhs) noexcept = default;
    SkeletalTrack& operator=(SkeletalTrack&& rhs) noexcept = default;

    SkeletalTrack(const SkeletalTrack& rhs) = delete;
    SkeletalTrack& operator=(const SkeletalTrack& rhs) = delete;

    std::uint32_t bone_index;
    std::vector<TranslationKey> translation_key_sequence;
    InterpolationMode translation_interp_mode;
    std::vector<RotationKey> rotation_key_sequence;
    InterpolationMode rotation_interp_mode;
    std::vector<ScalingKey> scaling_key_sequence;
    InterpolationMode scaling_interp_mode;
    ExtrapolationMode pre_mode;
    ExtrapolationMode post_mode;
  };

  struct MorphTargetTrack {
    MorphTargetTrack() = default;
    MorphTargetTrack(uint32_t p_bone_index,
                     std::vector<MorphingKey>&& p_key_sequence) noexcept
        : bone_index(p_bone_index), key_sequence(std::move(p_key_sequence)) {}
    MorphTargetTrack(MorphTargetTrack&& rhs) noexcept = default;
    MorphTargetTrack& operator=(MorphTargetTrack&& rhs) noexcept = default;

    MorphTargetTrack(const MorphTargetTrack& rhs) = delete;
    MorphTargetTrack& operator=(const MorphTargetTrack& rhs) = delete;

    uint32_t bone_index;
    std::vector<MorphingKey> key_sequence;
  };

  Animation() = default;
  Animation(std::string&& p_name, real p_duration,
            std::vector<SkeletalTrack>&& p_skeletal_tracks,
            std::vector<MorphTargetTrack>&& p_morph_target_tracks) noexcept
      : name(std::move(p_name)),
        duration(p_duration),
        skeletal_tracks(std::move(p_skeletal_tracks)),
        morph_target_tracks(std::move(p_morph_target_tracks)) {
    for (uint32_t i = 0; i < skeletal_tracks.size(); i++) {
      bone_index_to_skeletal_track[skeletal_tracks[i].bone_index] = i;
    }
    for (uint32_t i = 0; i < morph_target_tracks.size(); i++) {
      bone_index_to_morph_target_track[morph_target_tracks[i].bone_index] = i;
    }
  }

  Animation(Animation&& rhs) noexcept = default;
  Animation& operator=(Animation&& rhs) noexcept = default;

  Animation(const Animation&) = delete;
  Animation& operator=(const Animation&) = delete;

  ALWAYS_INLINE uint32_t GetSkeletalTrackCount() const {
    return static_cast<uint32_t>(skeletal_tracks.size());
  }
  ALWAYS_INLINE bool HasSkeletalTrack(uint32_t bone_index) const {
    return bone_index_to_skeletal_track.find(bone_index) !=
           bone_index_to_skeletal_track.end();
  }
  ALWAYS_INLINE const SkeletalTrack& GetSkeletalTrack(
      uint32_t bone_index) const {
    return skeletal_tracks[bone_index_to_skeletal_track.at(bone_index)];
  }

  ALWAYS_INLINE uint32_t GetMorphTargetTrackCount() const {
    return static_cast<uint32_t>(morph_target_tracks.size());
  }
  ALWAYS_INLINE bool HasMorphTargetTrack(uint32_t bone_index) const {
    return bone_index_to_morph_target_track.find(bone_index) !=
           bone_index_to_morph_target_track.end();
  }
  ALWAYS_INLINE const MorphTargetTrack& GetMorphTargetTrack(
      uint32_t bone_index) const {
    return morph_target_tracks[bone_index_to_morph_target_track.at(bone_index)];
  }

  std::string name;
  real duration;
  std::vector<SkeletalTrack> skeletal_tracks;
  std::vector<MorphTargetTrack> morph_target_tracks;
  std::unordered_map<uint32_t, uint32_t> bone_index_to_skeletal_track;
  std::unordered_map<uint32_t, uint32_t> bone_index_to_morph_target_track;
};

}  // namespace ho
