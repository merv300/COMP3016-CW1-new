#include "InputManager.h"

namespace rtz {

void InputManager::BeginFrame() {
    pressedThisFrame.clear();
}

void InputManager::ProcessEvent(const SDL_Event& event) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
        quitRequested = true;
        break;

    case SDL_EVENT_KEY_DOWN:
        // SDL repeats key-down events while a key is held; only the first one
        // counts as a fresh press so Q cannot rotate twice in one hold.
        if (!event.key.repeat) {
            pressedThisFrame.insert(static_cast<int>(event.key.scancode));
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            mouseLeftDown = true;
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT) {
            mouseLeftDown = false;
        }
        break;

    default:
        break;
    }
}

void InputManager::EndFrame() {
    keys = SDL_GetKeyboardState(nullptr);
    firing = mouseLeftDown || IsKeyDown(SDL_SCANCODE_SPACE);
}

bool InputManager::IsKeyDown(SDL_Scancode scancode) const {
    if (keys == nullptr) {
        return false;
    }
    return keys[static_cast<int>(scancode)];
}

bool InputManager::WasKeyPressed(SDL_Scancode scancode) const {
    return pressedThisFrame.find(static_cast<int>(scancode)) != pressedThisFrame.end();
}

Vec2 InputManager::GetMoveAxis() const {
    Vec2 axis{0.0f, 0.0f};

    if (IsKeyDown(SDL_SCANCODE_A) || IsKeyDown(SDL_SCANCODE_LEFT)) {
        axis.x -= 1.0f;
    }
    if (IsKeyDown(SDL_SCANCODE_D) || IsKeyDown(SDL_SCANCODE_RIGHT)) {
        axis.x += 1.0f;
    }
    if (IsKeyDown(SDL_SCANCODE_W) || IsKeyDown(SDL_SCANCODE_UP)) {
        axis.y -= 1.0f;
    }
    if (IsKeyDown(SDL_SCANCODE_S) || IsKeyDown(SDL_SCANCODE_DOWN)) {
        axis.y += 1.0f;
    }

    return axis.Normalized();
}

} // namespace rtz
