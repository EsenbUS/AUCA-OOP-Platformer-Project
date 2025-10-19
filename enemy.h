#ifndef ENEMY_H
#define ENEMY_H

// This is enemy.h
#include "raylib.h"
#include <vector>

class Enemy {
private:
    Vector2 pos;
    bool is_looking_right;

    // Static collection of all enemies
    static std::vector<Enemy> all_enemies;

public:
    // Constructor
    Enemy(Vector2 position, bool isLookingRight = true);

    // Enemy methods
    void update();

    // Getters
    Vector2 getPosition() const;
    bool isLookingRight() const;

    // Static methods for enemy management
    static void spawnAll();
    static void updateAll();
    static bool isCollidingWith(Vector2 pos);
    static void removeColliding(Vector2 pos);

    // Get all enemies (for rendering, etc.)
    static const std::vector<Enemy>& getAllEnemies();
};

#endif // ENEMY_H