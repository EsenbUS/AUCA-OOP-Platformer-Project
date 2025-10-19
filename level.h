#ifndef LEVEL_H
#define LEVEL_H

// This is level.h
#include "raylib.h"
#include <string>

struct level {
    size_t rows = 0, columns = 0;
    char *data = nullptr;
};

class Level {
    static Level* instance;

    int level_index;
    const int LEVEL_COUNT;
    level* levels;
    level current_level;
    char* current_level_data;

    // Private constructor for singleton pattern
    Level();

    // Destructor to clean up
    ~Level();

public:
    // Singleton accessor
    static Level* getInstance();

    // Level methods
    bool isInsideLevel(int row, int column);
    bool isColliding(Vector2 pos, char lookFor = '#');
    char& getCollider(Vector2 pos, char lookFor);

    // Level management
    void resetLevelIndex();
    void loadLevel(int offset = 0);
    void unloadLevel();

    // Cell access
    char& getLevelCell(size_t row, size_t column);
    void setLevelCell(size_t row, size_t column, char chr);

    // Level RLE loading
    void loadLevelFromRLE(const std::string& filename);

    // Getters for previously global variables
    int getLevelIndex() const;
    int getLevelCount() const;
    const level& getCurrentLevel() const;
};

#endif // LEVEL_H