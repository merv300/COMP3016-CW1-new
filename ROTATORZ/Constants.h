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

// Enemy shots fire far more often than player shots, so the enemy sound plays
// quieter than the rest of the mix. 1.0 is full volume.
inline constexpr float kEnemyShootGain = 0.35f;

// Round-robin voices so near-simultaneous enemy shots overlap instead of
// cutting each other off when they share one track.
inline constexpr int kEnemyShootVoices = 4;

// Minimum gap between "wrong orientation" hit sounds. Several bullets can
// strike in the same frame, and stacking the same sample sounds like clipping.
inline constexpr float kWrongHitSfxCooldown = 0.06f;
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

// --- Stars ---
inline constexpr int kStarCount = 160;
inline constexpr int kStarLayers = 3;

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

// --- High scores ---
inline constexpr int kMaxHighScores = 10;
inline constexpr const char* kScoresFilename = "scores.dat";

// --- Asset paths (place your files in the assets/ folder next to the exe) ---
inline constexpr const char* kAssetPlayerSprite = "assets/player.png";
inline constexpr const char* kAssetEnemyPortrait = "assets/enemy_portrait.png"; // orange, killable at 0°
inline constexpr const char* kAssetEnemyLandscape = "assets/enemy_landscape.png"; // green, killable at 90°
inline constexpr const char* kAssetMusic = "assets/music.ogg";
inline constexpr const char* kAssetShootSfx = "assets/shoot.wav";
inline constexpr const char* kAssetEnemyShootSfx = "assets/enemy_shoot.wav";
inline constexpr const char* kAssetWrongHitSfx = "assets/hit_wrong.wav"; // shot hit an enemy aligned to the other axis

} // namespace rtz
