#pragma once

namespace rtz {

// The world is square so that rotating the view 90 degrees always fits the
// window exactly, no matter which quarter turn is active.
inline constexpr int kWindowWidth = 800;
inline constexpr int kWindowHeight = 800;

inline constexpr float kWorldWidth = static_cast<float>(kWindowWidth);
inline constexpr float kWorldHeight = static_cast<float>(kWindowHeight);

// Frame deltas are clamped so a stall (breakpoint, alt-tab, slow first frame)
// cannot teleport entities through the world.
inline constexpr float kRotationSpeed = 300.0f;

inline constexpr float kMaxFrameDelta = 0.05f;

inline constexpr float kPlayerSpeed = 320.0f;
inline constexpr float kPlayerSize = 36.0f;

} // namespace rtz
