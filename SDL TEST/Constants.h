#pragma once

namespace rtz {

// Window: the OS window the rotated world is presented into.
inline constexpr int kWindowWidth = 1280;
inline constexpr int kWindowHeight = 720;

// World: the fixed-size playfield rendered off-screen, then scaled and
// rotated to fit the window with its aspect ratio preserved.
inline constexpr float kWorldWidth = 480.0f;
inline constexpr float kWorldHeight = 800.0f;

// Rotation tween speed in degrees per second.
inline constexpr float kRotationSpeed = 300.0f;

// Frame deltas are clamped so a stall (breakpoint, alt-tab, slow first frame)
// cannot teleport entities through the world.
inline constexpr float kMaxFrameDelta = 0.05f;

// --- Player ---
inline constexpr float kPlayerSpeed = 360.0f;
inline constexpr float kPlayerSize = 36.0f;
inline constexpr float kPlayerMargin = 15.0f;
inline constexpr float kPlayerY = kWorldHeight - 60.0f;
inline constexpr float kPlayerInvuln = 1.5f;
inline constexpr float kFireCooldown = 0.22f;
inline constexpr float kPlayerBulletSpeed = 700.0f;

// --- Bomb ---
inline constexpr float kBombSize = 14.0f;
inline constexpr float kBombSpeed = 380.0f;
inline constexpr float kBombCooldownTime = 5.0f;
inline constexpr float kBombScore = 10.0f;

// --- Enemy bullets ---
inline constexpr float kEnemyBulletSpeed = 240.0f;

// --- Explosion ---
inline constexpr float kExplosionRadius = 80.0f;
inline constexpr float kExplosionDuration = 0.4f;

// --- Enemy grid ---
inline constexpr int kEnemyCols = 6;
inline constexpr int kEnemyRows = 4;
inline constexpr float kEnemyW = 40.0f;
inline constexpr float kEnemyH = 28.0f;
inline constexpr float kEnemyGapX = 15.0f;
inline constexpr float kEnemyGapY = 20.0f;
inline constexpr float kEnemyStartY = 60.0f;
inline constexpr float kEnemyDescendSpeed = 14.0f;
inline constexpr float kEnemyShootMin = 2.0f;
inline constexpr float kEnemyShootMax = 4.5f;

// Delay between clearing a wave and the next wave entering.
inline constexpr float kWaveClearDelay = 2.0f;

} // namespace rtz
