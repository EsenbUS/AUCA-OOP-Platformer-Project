#include "player.h"
#include "globals.h"  // Still needed for game_state, timer, etc.
#include "level.h"
#include "enemy.h"

// This is player.cpp

// Initialize static instance
Player* Player::instance = nullptr;

Player::Player() :
    y_velocity(0),
    position({0, 0}),
    is_on_ground(false),
    is_looking_forward(true),
    is_moving(false),
    lives(3)
{
    // Get level count from Level class
    int levelCount = Level::getInstance()->getLevelCount();
    // Allocate memory for level scores
    level_scores = new int[levelCount]();  // Initialize with zeros
}

Player::~Player() {
    // Clean up memory
    delete[] level_scores;
}

Player* Player::getInstance() {
    if (instance == nullptr) {
        instance = new Player();
    }
    return instance;
}

void Player::init() {
    // Initialize player values
    y_velocity = 0;
    position = {0, 0};
    is_on_ground = false;
    is_looking_forward = true;
    is_moving = false;
    lives = getMaxLives();

    // Initialize scores
    int levelCount = Level::getInstance()->getLevelCount();
    for (int i = 0; i < levelCount; i++) {
        level_scores[i] = 0;
    }
}

void Player::resetStats() {
    lives = getMaxLives();
    int levelCount = Level::getInstance()->getLevelCount();
    for (int i = 0; i < levelCount; i++) {
        level_scores[i] = 0;
    }
}

void Player::incrementScore() {
    PlaySound(coin_sound);
    int levelIndex = Level::getInstance()->getLevelIndex();
    level_scores[levelIndex]++;
}

int Player::getTotalScore() {
    int sum = 0;
    int levelCount = Level::getInstance()->getLevelCount();
    for (int i = 0; i < levelCount; i++) {
        sum += level_scores[i];
    }
    return sum;
}

int Player::getLives() const {
    return lives;
}

void Player::setLives(int newLives) {
    lives = newLives;
}

int Player::getMaxLives() const {
    return MAX_LIVES;
}

Vector2 Player::getPosition() const {
    return position;
}

void Player::setPosition(Vector2 newPos) {
    position = newPos;
}

float Player::getYVelocity() const {
    return y_velocity;
}

void Player::setYVelocity(float velocity) {
    y_velocity = velocity;
}

bool Player::isOnGround() const {
    return is_on_ground;
}

void Player::setOnGround(bool onGround) {
    is_on_ground = onGround;
}

bool Player::isLookingForward() const {
    return is_looking_forward;
}

void Player::setLookingForward(bool forward) {
    is_looking_forward = forward;
}

bool Player::isMoving() const {
    return is_moving;
}

void Player::setMoving(bool moving) {
    is_moving = moving;
}

void Player::spawn() {
    y_velocity = 0;
    Level* levelPtr = Level::getInstance();
    const struct level& currentLevel = levelPtr->getCurrentLevel();

    for (size_t row = 0; row < currentLevel.rows; ++row) {
        for (size_t column = 0; column < currentLevel.columns; ++column) {
            char cell = levelPtr->getLevelCell(row, column);

            if (cell == PLAYER) {
                position.x = column;
                position.y = row;
                levelPtr->setLevelCell(row, column, AIR);
                return;
            }
        }
    }
}

void Player::kill() {
    // Decrement a life and reset all collected coins in the current level
    PlaySound(player_death_sound);
    game_state = DEATH_STATE;
    lives--;
    int levelIndex = Level::getInstance()->getLevelIndex();
    level_scores[levelIndex] = 0;
}

void Player::moveHorizontally(float delta) {
    // See if the player can move further without touching a wall;
    // otherwise, prevent them from getting into a wall by rounding their position
    float next_x = position.x + delta;
    Level* levelPtr = Level::getInstance();

    if (!levelPtr->isColliding({next_x, position.y}, WALL)) {
        position.x = next_x;
    }
    else {
        position.x = roundf(position.x);
        return;
    }

    // For drawing player animations
    is_looking_forward = delta > 0;
    if (delta != 0) is_moving = true;
}

void Player::updateGravity() {
    Level* levelPtr = Level::getInstance();

    // Bounce downwards if approaching a ceiling with upwards velocity
    if (levelPtr->isColliding({position.x, position.y - 0.1f}, WALL) && y_velocity < 0) {
        y_velocity = CEILING_BOUNCE_OFF;
    }

    // Add gravity to player's y-position
    position.y += y_velocity;
    y_velocity += GRAVITY_FORCE;

    // If the player is on ground, zero player's y-velocity
    // If the player is *in* ground, pull them out by rounding their position
    is_on_ground = levelPtr->isColliding({position.x, position.y + 0.1f}, WALL);
    if (is_on_ground) {
        y_velocity = 0;
        position.y = roundf(position.y);
    }
}

void Player::update() {
    updateGravity();
    Level* levelPtr = Level::getInstance();
    const struct level& currentLevel = levelPtr->getCurrentLevel();

    // Interacting with other level elements
    if (levelPtr->isColliding(position, COIN)) {
        levelPtr->getCollider(position, COIN) = AIR; // Removes the coin
        incrementScore();
    }

    if (levelPtr->isColliding(position, EXIT)) {
        // Reward player for being swift
        if (timer > 0) {
            // For every 9 seconds remaining, award the player 1 coin
            timer -= 25;
            time_to_coin_counter += 5;

            if (time_to_coin_counter / 60 > 1) {
                incrementScore();
                time_to_coin_counter = 0;
            }
        }
        else {
            // Allow the player to exit after the level timer goes to zero
            levelPtr->loadLevel(1);
            PlaySound(exit_sound);
        }
    }
    else {
        // Decrement the level timer if not at an exit
        if (timer >= 0) timer--;
    }

    // Kill the player if they touch a spike or fall below the level
    if (levelPtr->isColliding(position, SPIKE) || position.y > currentLevel.rows) {
        kill();
    }

    // Upon colliding with an enemy...
    if (Enemy::isCollidingWith(position)) {
        // ...check if their velocity is downwards...
        if (y_velocity > 0) {
            // ...if yes, award the player and kill the enemy
            Enemy::removeColliding(position);
            PlaySound(kill_enemy_sound);

            incrementScore();
            y_velocity = -BOUNCE_OFF_ENEMY;
        }
        else {
            // ...if not, kill the player
            kill();
        }
    }
}