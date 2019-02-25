//
// Created by Prayansh Srivastava on 2019-02-11.
//

#include "level_system.h"

LevelSystem::LevelSystem() : rng_(Random(4)),
                             chunks_() {

}

void LevelSystem::generateEntity(int value, float x, float y,
                                 Blackboard &blackboard, entt::DefaultRegistry &registry) {
    switch (value) {
        case 1: {
            auto texture = blackboard.textureManager.get_texture(
                    (rng_.nextInt(0, 100) % 2 == 0) ? "platform1" : "platform2");
            auto shader = blackboard.shader_manager.get_shader("sprite");
            auto scale = static_cast<float>(CELL_WIDTH / texture.width());
            auto platform = registry.create();
            registry.assign<Platform>(platform);
            registry.assign<Transform>(platform, x, y, 0., scale,
                                       scale);
            registry.assign<Sprite>(platform, texture, shader);
            registry.assign<Collidable>(platform, texture.width() * scale,
                                        texture.height() * scale);
        }
            break;
        case 3: {
            auto texture = blackboard.textureManager.get_texture("bread");
            auto shader = blackboard.shader_manager.get_shader("sprite");
            auto scale = static_cast<float>(CELL_WIDTH / texture.width()/2);
            auto bread = registry.create();
            registry.assign<Transform>(bread, x, y, 0., scale,
                                       scale);
            registry.assign<Sprite>(bread, texture, shader);
            registry.assign<Bread>(bread);
            registry.assign<CausesDamage>(bread, false, true, 1);
            registry.assign<Health>(bread,1);
            registry.assign<Velocity>(bread, -BREAD_SPEED, 0.f);
            registry.assign<Interactable>(bread);
            registry.assign<Collidable>(bread, texture.width() * scale,
                                        texture.height() * scale);
            registry.assign<ObeysGravity>(bread);
        }
            break;
        case 5: {
            auto texture = blackboard.textureManager.get_texture("llama");
            auto shader = blackboard.shader_manager.get_shader("sprite");
            auto scale = static_cast<float>(CELL_HEIGHT / texture.height());
            auto llama = registry.create();
            registry.assign<Transform>(llama, x, y - 200, 0., scale,
                                       scale);
            registry.assign<Sprite>(llama, texture, shader);
            registry.assign<Llama>(llama);
            registry.assign<CausesDamage>(llama, false, true, 1);
            registry.assign<Health>(llama,1);
            registry.assign<Velocity>(llama, 0.f, 0.f);
            registry.assign<Interactable>(llama);
            registry.assign<Collidable>(llama, texture.width() * scale,
                                        texture.height() * scale);
            registry.assign<ObeysGravity>(llama);
        }
            break;
        default:
            break;
    }
}

void LevelSystem::generateProjectile(float x, float y, Blackboard &blackboard, entt::DefaultRegistry &registry) {
    auto texture = blackboard.textureManager.get_texture("spit");
    auto shader = blackboard.shader_manager.get_shader("sprite");
    auto scale = static_cast<float>(CELL_WIDTH / texture.width()/ 5);
    auto projectile = registry.create();
    registry.assign<Transform>(projectile, x, y - 30, 0., scale,
                               scale);
    registry.assign<Sprite>(projectile, texture, shader);
    registry.assign<Spit>(projectile);
    registry.assign<CausesDamage>(projectile, false, true, 1);
    registry.assign<Health>(projectile,1);
    registry.assign<Velocity>(projectile, PROJECTILE_SPEED_X, PROJECTILE_SPEED_Y);
    registry.assign<Interactable>(projectile);
    registry.assign<Collidable>(projectile, texture.width() * scale,
                                texture.height() * scale);
}
