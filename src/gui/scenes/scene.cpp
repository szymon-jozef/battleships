#include "scene.hpp"

namespace battleship {
namespace gui {
Scene::Scene(GameContext &gameContext, Texture2D &background)
    : gameContext(gameContext)
    , background(background) {}

void Scene::update() {
  updateSceneRect();
}

void Scene::draw() {
  DrawTexturePro(background,
                 {0.0f, 0.0f, static_cast<float>(background.width), static_cast<float>(background.height)},
                 sceneRect,
                 {0.0f, 0.0f},
                 0.0f,
                 backgroundTint);
}

} // namespace gui
} // namespace battleship
