#ifndef PLAYER_H
#define PLAYER_H

// This is player.h
#include "raylib.h"

class Player {
    static Player* instance;

    Vector2 position;
    float y_velocity;
    bool is_on_ground;
    bool is_looking_forward;
    bool is_moving;
    int* level_scores;
    int lives;
    const int MAX_LIVES = 3;

    // Private constructor for singleton pattern
    Player();

    // Destructor
    ~Player();

public:
    // Singleton accessor
    static Player* getInstance();

    // Initialize player
    void init();

    // Player statistics management
    void resetStats();
    void incrementScore();
    int getTotalScore();
    int getLives() const;
    void setLives(int newLives);
    int getMaxLives() const;

    // Player state getters and setters
    Vector2 getPosition() const;
    void setPosition(Vector2 newPos);

    float getYVelocity() const;
    void setYVelocity(float velocity);

    bool isOnGround() const;
    void setOnGround(bool onGround);

    bool isLookingForward() const;
    void setLookingForward(bool forward);

    bool isMoving() const;
    void setMoving(bool moving);

    // Player actions
    void spawn();
    void kill();
    void moveHorizontally(float delta);
    void updateGravity();
    void update();
};

#endif // PLAYER_H