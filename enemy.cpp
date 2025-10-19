#include "enemy.h"
#include "level.h"
#include "globals.h"  // Still needed for ENEMY_MOVEMENT_SPEED, WALL, etc.

// This is enemy.cpp
// Initialize static member
std::vector<Enemy> Enemy::all_enemies;

Enemy::Enemy(Vector2 position, bool isLookingRight)
    : pos(position), is_looking_right(isLookingRight) {
    // Constructor
}

void Enemy::update() {
    // Find the enemy's next x
    float next_x = pos.x;
    next_x += (is_looking_right ? ENEMY_MOVEMENT_SPEED : -ENEMY_MOVEMENT_SPEED);

    // If its next position collides with a wall, turn around
    if (Level::getInstance()->isColliding({next_x, pos.y}, WALL)) {
        is_looking_right = !is_looking_right;
    }
    // Otherwise, keep moving
    else {
        pos.x = next_x;
    }
}

Vector2 Enemy::getPosition() const {
    return pos;
}

bool Enemy::isLookingRight() const {
    return is_looking_right;
}

// Static methods for enemy management
void Enemy::spawnAll() {
    Level* levelPtr = Level::getInstance();
    const struct level& currentLevel = levelPtr->getCurrentLevel();

    // Create enemies, incrementing their amount every time a new one is created
    all_enemies.clear();

    for (size_t row = 0; row < currentLevel.rows; ++row) {
        for (size_t column = 0; column < currentLevel.columns; ++column) {
            char cell = levelPtr->getLevelCell(row, column);

            if (cell == ENEMY) {
                // Create enemy object and add to enemies vector
                all_enemies.push_back(Enemy({static_cast<float>(column), static_cast<float>(row)}, true));
                levelPtr->setLevelCell(row, column, AIR);
            }
        }
    }
}

void Enemy::updateAll() {
    for (auto &enemy : all_enemies) {
        enemy.update();
    }
}

bool Enemy::isCollidingWith(Vector2 pos) {
    Rectangle entityHitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (auto &enemy : all_enemies) {
        Rectangle enemyHitbox = {enemy.pos.x, enemy.pos.y, 1.0f, 1.0f};
        if (CheckCollisionRecs(entityHitbox, enemyHitbox)) {
            return true;
        }
    }
    return false;
}

void Enemy::removeColliding(Vector2 pos) {
    Rectangle entityHitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (auto it = all_enemies.begin(); it != all_enemies.end(); it++) {
        Rectangle enemyHitbox = {it->pos.x, it->pos.y, 1.0f, 1.0f};
        // Erase a colliding enemy
        if (CheckCollisionRecs(entityHitbox, enemyHitbox)) {
            all_enemies.erase(it);

            // Call the function again to remove any remaining colliding enemies
            // Cannot continue as calling erase on a vector invalidates current iterators
            removeColliding(pos);
            return;
        }
    }
}

const std::vector<Enemy>& Enemy::getAllEnemies() {
    return all_enemies;
}