//
// Created by Prayansh Srivastava on 2019-02-11.
//

#ifndef PANDAEXPRESS_LEVEL_MANAGER_H
#define PANDAEXPRESS_LEVEL_MANAGER_H
#define CELL_WIDTH 100.0  // Should these be macros and not constants?
#define CELL_HEIGHT 100.0

#include <components/transform.h>
#include <components/collidable.h>
#include <components/platform.h>
#include <components/bread.h>
#include <components/ghost.h>
#include <components/falling_platform.h>
#include <components/llama.h>
#include <components/spit.h>
#include <components/causes_damage.h>
#include <components/health.h>
#include <components/velocity.h>
#include <components/obstacle.h>
#include <components/interactable.h>
#include <components/obeys_gravity.h>
#include <graphics/cave.h>
#include "util/random.h"
#include "systems/system.h"
#include <queue>

class LevelSystem : public System {
protected:
    Random rng_;
    std::queue<std::vector<char>> chunks_;

    const float BREAD_SPEED = 50.f;
    const float PROJECTILE_SPEED_X = -300.f;
    const float PROJECTILE_SPEED_Y = 10.f;

    const float PLATFORM_HEIGHT = 20.f;

    void generateEntity(char value, float x, float y,
                        Blackboard &blackboard, entt::DefaultRegistry &registry);


public:

    LevelSystem();

    virtual void init();

    void update(Blackboard &blackboard, entt::DefaultRegistry &registry) override = 0;

    void generate_bread(bool move_left, float x, float y, Blackboard &blackboard, entt::DefaultRegistry &registry);
    virtual void destroy_entities(entt::DefaultRegistry &registry);

    const std::string FALLING_PLATFORM_TIMER_LABEL = "fall";
    const std::string SPIT_TIMER_LABEL = "spit";
    static const unsigned int STORY_SEED = 1;

};


#endif //PANDAEXPRESS_LEVEL_MANAGER_H