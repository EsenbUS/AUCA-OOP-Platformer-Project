#include "level.h"
#include "player.h"
#include "enemy.h"
#include "globals.h"  // Still needed for game_state, timer, etc.
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>

// This is level.cpp

// Initialize static instance
Level* Level::instance = nullptr;

Level::Level() :
    level_index(0),
    LEVEL_COUNT(3),
    current_level_data(nullptr)
{
    // Allocate memory for levels array
    levels = new level[LEVEL_COUNT];
}

Level::~Level() {
    // Clean up allocated memory
    delete[] levels;
    unloadLevel();
}

Level* Level::getInstance() {
    if (instance == nullptr) {
        instance = new Level();
    }
    return instance;
}

bool Level::isInsideLevel(int row, int column) {
    if (row < 0 || row >= current_level.rows) return false;
    if (column < 0 || column >= current_level.columns) return false;
    return true;
}

bool Level::isColliding(Vector2 pos, char lookFor) {
    Rectangle entityHitbox = {pos.x, pos.y, 1.0f, 1.0f};

    // Scan the adjacent area in the level to look for a match in collision
    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            // Check if the cell is out-of-bounds
            if (!isInsideLevel(row, column)) continue;
            if (getLevelCell(row, column) == lookFor) {
                Rectangle blockHitbox = {(float) column, (float) row, 1.0f, 1.0f};
                if (CheckCollisionRecs(entityHitbox, blockHitbox)) {
                    return true;
                }
            }
        }
    }
    return false;
}

char& Level::getCollider(Vector2 pos, char lookFor) {
    // Like isColliding(), except returns a reference to the colliding object
    Rectangle playerHitbox = {pos.x, pos.y, 1.0f, 1.0f};

    for (int row = pos.y - 1; row < pos.y + 1; ++row) {
        for (int column = pos.x - 1; column < pos.x + 1; ++column) {
            // Check if the cell is out-of-bounds
            if (!isInsideLevel(row, column)) continue;
            if (getLevelCell(row, column) == lookFor) {
                Rectangle blockHitbox = {(float) column, (float) row, 1.0f, 1.0f};
                if (CheckCollisionRecs(playerHitbox, blockHitbox)) {
                    return getLevelCell(row, column);
                }
            }
        }
    }

    // If failed, get an approximation
    return getLevelCell(pos.x, pos.y);
}

void Level::resetLevelIndex() {
    level_index = 0;
}

int Level::getLevelIndex() const {
    return level_index;
}

int Level::getLevelCount() const {
    return LEVEL_COUNT;
}

const level& Level::getCurrentLevel() const {
    return current_level;
}

void Level::loadLevel(int offset) {
    level_index += offset;

    // Win logic
    if (level_index >= LEVEL_COUNT) {
        game_state = VICTORY_STATE;
        create_victory_menu_background();
        level_index = 0;
        return;
    }

    loadLevelFromRLE("data/levels.rll");

}

void Level::loadLevelFromRLE(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open file: %s", filename.c_str());
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;
    std::vector<std::string> levelStrings;

    // Read all levels from the file
    while (std::getline(file, line)) {
        // Skip comments (lines starting with semicolon)
        if (line.empty() || line[0] == ';') {
            continue;
        }
        levelStrings.push_back(line);
    }

    TraceLog(LOG_INFO, "Found %d levels in %s", levelStrings.size(), filename.c_str());

    // Make sure we have enough levels
    if (levelStrings.empty()) {
        TraceLog(LOG_ERROR, "No valid levels found in file");
        throw std::runtime_error("No valid levels found in file");
    }

    if (level_index >= levelStrings.size()) {
        TraceLog(LOG_ERROR, "Level index %d is out of range (max %d)", level_index, levelStrings.size() - 1);
        level_index = 0; // Reset to first level instead of throwing
    }

    // Use the level at the current index
    std::string content = levelStrings[level_index];
    TraceLog(LOG_INFO, "Loading level %d, content length: %d", level_index, content.length());

    // COMPLETELY REWRITTEN PARSING LOGIC
    std::vector<std::string> rows;
    size_t start = 0;
    size_t pos = 0;

    // Split by pipes
    while (pos < content.length()) {
        if (content[pos] == '|') {
            // Add everything from start to pos
            rows.push_back(content.substr(start, pos - start));
            start = pos + 1;  // Skip the pipe
        } else if (content[pos] == '.') {
            // Add the last part before the period
            if (pos > start) {
                rows.push_back(content.substr(start, pos - start));
            }
            break;  // End of level
        }
        pos++;
    }

    // If we reached the end without a period, add the last part
    if (pos == content.length() && pos > start) {
        rows.push_back(content.substr(start));
    }

    TraceLog(LOG_INFO, "Found %d rows in level", rows.size());

    // Decode RLE
    std::vector<std::string> decodedRows;
    for (const auto& row : rows) {
        std::string decodedRow;
        size_t i = 0;

        while (i < row.length()) {
            // If it's a digit, we need to parse a number
            if (isdigit(row[i])) {
                // Parse the number for repetition count
                int count = 0;
                while (i < row.length() && isdigit(row[i])) {
                    count = count * 10 + (row[i] - '0');
                    i++;
                }

                // Ensure we haven't reached the end
                if (i < row.length()) {
                    // Get the character to repeat
                    char c = row[i++];
                    // Repeat the character 'count' times
                    for (int j = 0; j < count; j++) {
                        decodedRow += c;
                    }
                }
            }
            // If not a digit, just add the character as-is
            else {
                decodedRow += row[i++];
            }
        }

        decodedRows.push_back(decodedRow);
    }

    // Create level
    size_t numRows = decodedRows.size();
    size_t maxCols = 0;

    // Find the maximum column count
    for (const auto& row : decodedRows) {
        maxCols = std::max(maxCols, row.length());
    }

    TraceLog(LOG_INFO, "Level dimensions: %d rows x %d columns", numRows, maxCols);

    // Allocate memory for level data
    delete[] current_level_data; // Clear previous data if any
    current_level_data = new char[numRows * maxCols];

    // Fill the level data
    for (size_t row = 0; row < numRows; row++) {
        for (size_t col = 0; col < maxCols; col++) {
            if (col < decodedRows[row].length()) {
                current_level_data[row * maxCols + col] = decodedRows[row][col];
            } else {
                // Fill with air if the row is shorter than maxCols
                current_level_data[row * maxCols + col] = AIR;
            }
        }
    }

    // Update current level structure
    current_level = {numRows, maxCols, current_level_data};

    // Setup entities and game state
    Player::getInstance()->spawn();
    Enemy::spawnAll();
    derive_graphics_metrics_from_loaded_level();
    timer = MAX_LEVEL_TIME;
}

void Level::unloadLevel() {
    delete[] current_level_data;
    current_level_data = nullptr;
}

char& Level::getLevelCell(size_t row, size_t column) {
    return current_level.data[row * current_level.columns + column];
}

void Level::setLevelCell(size_t row, size_t column, char chr) {
    getLevelCell(row, column) = chr;
}