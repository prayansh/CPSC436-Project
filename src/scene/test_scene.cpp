//
// Created by alex on 29/01/19.
//

#include <components/bread.h>
#include <components/panda.h>
#include <components/collidable.h>
#include <components/obeys_gravity.h>
#include <components/health.h>
#include <components/velocity.h>
#include <components/interactable.h>
#include <components/platform.h>
#include <components/causes_damage.h>
#include "test_scene.h"
#include "components/transform.h"



TestScene::TestScene(Blackboard& blackboard, SceneManager& scene_manager) :
    Scene(scene_manager),
    platforms(),
    enemies(),
    sprite_transform_system(),
    sprite_render_system(),
    physics_system(),
    player_movement_system(),
    collision_system()
{
    init_scene(blackboard);
    gl_has_errors();
}


void TestScene::init_scene(Blackboard &blackboard) {
    srand(0);
    blackboard.camera.set_position(CAMERA_START_X, CAMERA_START_Y);
    blackboard.camera.compose();
    last_placed_x = PLATFORM_START_X;
    last_bread_x = BREAD_START_X;
    create_panda(blackboard);
    create_bread(blackboard);
}

void TestScene::update(Blackboard& blackboard) {
    vec2 cam_size = blackboard.camera.size();
    vec2 cam_position = blackboard.camera.position();
    blackboard.camera.set_position(cam_position.x + CAMERA_SPEED * blackboard.delta_time, cam_position.y);
    blackboard.camera.compose();
    generate_platforms(blackboard);

    auto &transform = registry_.get<Transform>(panda_entity);
    auto &panda = registry_.get<Panda>(panda_entity);
    auto &panda_collidable = registry_.get<Collidable>(panda_entity);

    if (transform.x + panda_collidable.width < cam_position.x - cam_size.x / 2 ||
        transform.y - panda_collidable.height > cam_position.y + cam_size.y / 2 || !panda.alive) {
        reset_scene(blackboard);
    } else if (transform.x + panda_collidable.width / 2 > cam_position.x + cam_size.x / 2) {
        transform.x = cam_position.x + cam_size.x / 2 - panda_collidable.width / 2;
    }

    if (cam_position.x + 500 >= last_bread_x) {
        create_bread(blackboard);
    }

    player_movement_system.update(blackboard, registry_);
    collision_system.update(blackboard, registry_);
    physics_system.update(blackboard, registry_);
    sprite_transform_system.update(blackboard, registry_);
}

void TestScene::render(Blackboard &blackboard) {
    // update the rendering systems
    sprite_render_system.update(blackboard, registry_);

}

void TestScene::create_panda(Blackboard &blackboard) {
    panda_entity = registry_.create();

    auto texture = blackboard.textureManager.get_texture("panda");
    auto shader = blackboard.shader_manager.get_shader("sprite");
    float scale = 0.15f;
    registry_.assign<Transform>(panda_entity, PANDA_START_X, PANDA_START_Y, 0., scale, scale);
    registry_.assign<Sprite>(panda_entity, texture, shader);
    registry_.assign<Panda>(panda_entity);
    registry_.assign<ObeysGravity>(panda_entity);
    registry_.assign<Health>(panda_entity,1);
    registry_.assign<Interactable>(panda_entity);
    registry_.assign<CausesDamage>(panda_entity, false, true, 1);
    registry_.assign<Velocity>(panda_entity,0.f,0.f);
    registry_.assign<Collidable>(panda_entity, texture.width() * scale, texture.height() * scale);

}

void TestScene::generate_platforms(Blackboard &blackboard) {
    auto shader = blackboard.shader_manager.get_shader("sprite");
    float max_x =
            blackboard.camera.position().x + blackboard.camera.size().x; // some distance off camera
    while (last_placed_x < max_x) {
        auto texture = blackboard.textureManager.get_texture(
                (blackboard.randNumGenerator.nextInt(0, 100) % 2 == 0) ? "platform1" : "platform2");
        float scale = 50.0f / texture.width();
        if (platforms.size() > MAX_PLATFORMS) {//reuse
            auto platform = platforms.front();
            platforms.pop();
            registry_.replace<Transform>(platform, last_placed_x, PLATFORM_START_Y, 0.f, scale,
                                         scale);
            platforms.push(platform);
        } else {
            auto platform = registry_.create();
            registry_.assign<Platform>(platform);
            registry_.assign<Transform>(platform, last_placed_x, PLATFORM_START_Y, 0., scale,
                                        scale);
            registry_.assign<Sprite>(platform, texture, shader);
            registry_.assign<Collidable>(platform, texture.width() * scale, texture.height() * scale);

            platforms.push(platform);
        }
        last_placed_x += texture.width();
    }
}

void TestScene::create_bread(Blackboard &blackboard) {
    auto bread = registry_.create();
    auto texture = blackboard.textureManager.get_texture("bread");
    auto shader = blackboard.shader_manager.get_shader("sprite");

    float scale = 0.5;

    registry_.assign<Transform>(bread, last_bread_x, BREAD_START_Y - texture.height(), 0.,
                                scale, scale);
    registry_.assign<Sprite>(bread, texture, shader);
    registry_.assign<Bread>(bread);
    registry_.assign<CausesDamage>(bread, false, true, 1);
    registry_.assign<Health>(bread,1);
    registry_.assign<Velocity>(bread, -BREAD_SPEED, 0.f);
    registry_.assign<Collidable>(bread, texture.width() * scale, texture.height() * scale);
    registry_.assign<Interactable>(bread);
    registry_.assign<ObeysGravity>(bread);

    int next_interval = rand() % 10 + 2;
    last_bread_x = last_bread_x + (next_interval * 100);
}

void TestScene::reset_scene(Blackboard &blackboard) {
    registry_.destroy(panda_entity);
    while (!platforms.empty()) {
        uint32_t platform = platforms.front();
        registry_.destroy(platform);
        platforms.pop();
    }
    while (!enemies.empty()) {
        uint32_t enemy = enemies.front();
        registry_.destroy(enemy);
        enemies.pop();
    }
    init_scene(blackboard);
}