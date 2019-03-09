//
// Created by Prayansh Srivastava on 2019-02-15.
//

#include <util/constants.h>
#include <util/csv_reader.h>
#include <components/transform.h>
#include <components/collidable.h>
#include <components/timer.h>
#include "horizontal_level_system.h"

HorizontalLevelSystem::HorizontalLevelSystem(): LevelSystem() {

}

void HorizontalLevelSystem::init(){
    LevelSystem::init();
    last_col_generated_ = last_col_loaded_ = FIRST_COL_X;
    difficulty = MIN_DIFFICULTY;
    difficulty_timer.save_watch(LEVEL_UP_LABEL, LEVEL_UP_INTERVAL);
    load_next_chunk(0);
}

void HorizontalLevelSystem::load_next_chunk() {
    std::string level_path = levels_path("");
    int level = rng_.nextInt(std::max(1, difficulty - DIFFICULTY_RANGE), difficulty);
    load_next_chunk(level);
}

void HorizontalLevelSystem::load_next_chunk(int level) {
    std::string level_path = levels_path("");
    std::string levelFile = level_path + "level_" + std::to_string(level) + ".csv";
    CSVReader reader(levelFile);
    std::vector<std::vector<int>> dataList = reader.getData();
    for (int i = 0; i < dataList[0].size(); i++) {
        std::vector<int> col;
        col.reserve(9);
        for (int j = 0; j < 9; j++) {
            col.push_back(dataList[j][i]);
        }
        last_col_loaded_ += CELL_WIDTH;
        chunks_.push(col);
    }
}

// y should range from (-400, 400)

void HorizontalLevelSystem::generate_next_chunk(Blackboard &blackboard,
                                                entt::DefaultRegistry &registry) {
    float off_screen = blackboard.camera.position().x + blackboard.camera.size().x;
    while (last_col_generated_ < off_screen && !chunks_.empty()) { // second condn is safety check
        std::vector<int> col = chunks_.front();
        float y = -400.0f;
        for (int c:col) {
            generateEntity(c, last_col_generated_, y, blackboard, registry);
            y += CELL_HEIGHT;
        }
        last_col_generated_ += CELL_WIDTH;
        chunks_.pop();
    }
}

void HorizontalLevelSystem::destroy_entities(entt::DefaultRegistry &registry) {
    registry.destroy<Platform>();
    registry.destroy<Llama>();
    registry.destroy<Spit>();
    registry.destroy<Bread>();
    registry.destroy<Ghost>();
    registry.destroy<Obstacle>();

    while (!chunks_.empty()) {
        chunks_.front().clear();
        chunks_.pop();
    }
}

void HorizontalLevelSystem::update(Blackboard &blackboard, entt::DefaultRegistry &registry) {
    float max_x =
            blackboard.camera.position().x + blackboard.camera.size().x; // some distance off camera
    float min_x =
            blackboard.camera.position().x - blackboard.camera.size().x; // some distance off camera
    while (last_col_loaded_ < max_x) {
        load_next_chunk();
    }

    difficulty_timer.update(blackboard.delta_time);

    if (difficulty < MAX_DIFFICULTY && difficulty_timer.is_done(LEVEL_UP_LABEL)) {
        difficulty++;
        difficulty_timer.reset_watch(LEVEL_UP_LABEL);
    }

    destroy_off_screen(registry, min_x);
    generate_next_chunk(blackboard, registry);
}

void HorizontalLevelSystem::destroy_off_screen(entt::DefaultRegistry &registry, float x) {
    auto platforms = registry.view<Platform, Transform>();
    for (uint32_t entity: platforms) {
        auto &transform = platforms.get<Transform>(entity);
        if (transform.x < x) {
            registry.destroy(entity);
        }
    }

    auto llamas = registry.view<Llama, Transform>();
    for (uint32_t entity: llamas) {
        auto &transform = llamas.get<Transform>(entity);
        if (transform.x < x) {
            registry.destroy(entity);
        }
    }

    auto spits = registry.view<Spit, Transform>();
    for (uint32_t entity: spits) {
        auto &transform = spits.get<Transform>(entity);
        if (transform.x < x) {
            registry.destroy(entity);
        }
    }

    auto breads = registry.view<Bread, Transform>();
    for (uint32_t entity: breads) {
        auto &transform = breads.get<Transform>(entity);
        if (transform.x < x) {
            registry.destroy(entity);
        }
    }

    auto ghosts = registry.view<Ghost, Transform>();
    for (uint32_t entity: ghosts) {
        auto &transform = ghosts.get<Transform>(entity);
        if (transform.x < x) {
            registry.destroy(entity);
        }
    }

    auto obstacles = registry.view<Obstacle, Transform>();
    for (uint32_t entity: obstacles) {
        auto &transform = obstacles.get<Transform>(entity);
        if (transform.x < x) {
            registry.destroy(entity);
        }
    }
}
