#ifndef GRAPHICS_H
#define GRAPHICS_H

// This is graphics.h

#include "globals.h"
#include "player.h"
#include "level.h"
#include "enemy.h"

void draw_text(Text &text) {
    // Measure the text, center it to the required position, and draw it
    Vector2 dimensions = MeasureTextEx(*text.font, text.str.c_str(), text.size * screen_scale, text.spacing);

    Vector2 pos = {
        (screen_size.x * text.position.x) - (0.5f * dimensions.x),
        (screen_size.y * text.position.y) - (0.5f * dimensions.y)
    };

    DrawTextEx(*text.font, text.str.c_str(), pos, dimensions.y, text.spacing, text.color);
}

void derive_graphics_metrics_from_loaded_level() {
    // Get Level instance
    Level* level = Level::getInstance();
    const struct level& currentLevel = level->getCurrentLevel();

    // Level and UI setup
    screen_size.x = static_cast<float>(GetScreenWidth());
    screen_size.y = static_cast<float>(GetScreenHeight());

    // For very wide levels, use a reasonable cell size that fits the screen height
    // while allowing a good view distance
    float ideal_visible_rows = 12.0f; // Aim to see about 12 rows at once
    cell_size = screen_size.y / ideal_visible_rows;

    // Make sure the cell size isn't too small
    cell_size = std::max(cell_size, 20.0f);

    // Scale UI elements based on cell size
    screen_scale = std::min(screen_size.x, screen_size.y) / SCREEN_SCALE_DIVISOR;

    // Parallax background setup
    float larger_screen_side = std::max(screen_size.x, screen_size.y);

    if (screen_size.x > screen_size.y) {
        background_size = {larger_screen_side, larger_screen_side / 16 * 10};
    }
    else {
        background_size = {larger_screen_side / 10 * 16, larger_screen_side};
    }

    background_y_offset = (screen_size.y - background_size.y) * 0.5f;
}

void draw_parallax_background() {
    // First uses the player's position
    Player* player = Player::getInstance();
    float player_x = player->getPosition().x;
    float initial_offset = -(player_x * PARALLAX_PLAYER_SCROLLING_SPEED + game_frame * PARALLAX_IDLE_SCROLLING_SPEED);

    // Calculate offsets for different layers
    float background_offset   = initial_offset;
    float middleground_offset = background_offset * PARALLAX_LAYERED_SPEED_DIFFERENCE;
    float foreground_offset   = middleground_offset * PARALLAX_LAYERED_SPEED_DIFFERENCE;

    // Wrap offsets to create a loop effect
    background_offset   = fmod(background_offset, 1.0f);
    middleground_offset = fmod(middleground_offset, 1.0f);
    foreground_offset   = fmod(foreground_offset, 1.0f);

    // Scale to background size
    background_offset   *= background_size.x;
    middleground_offset *= background_size.x;
    foreground_offset   *= background_size.x;

    // Each layer is drawn twice, side by side, the first starting from its offset, and the other from its offset + background_size
    // This ensures a seamless scrolling effect, because when one copy moves out of sight, the second jumps into its place.
    draw_image(background,   {background_offset + background_size.x, background_y_offset},   background_size.x, background_size.y);
    draw_image(background,   {background_offset,                     background_y_offset},   background_size.x, background_size.y);

    draw_image(middleground, {middleground_offset + background_size.x, background_y_offset}, background_size.x, background_size.y);
    draw_image(middleground, {middleground_offset,                     background_y_offset}, background_size.x, background_size.y);

    draw_image(foreground,   {foreground_offset + background_size.x, background_y_offset},   background_size.x, background_size.y);
    draw_image(foreground,   {foreground_offset,                     background_y_offset},   background_size.x, background_size.y);
}

void draw_game_overlay() {
    Player* player = Player::getInstance();
    const float ICON_SIZE = 48.0f * screen_scale;

    float slight_vertical_offset = 8.0f;
    slight_vertical_offset *= screen_scale;

    // Hearts
    for (int i = 0; i < player->getLives(); i++) {
        const float SPACE_BETWEEN_HEARTS = 4.0f * screen_scale;
        draw_image(heart_image, {ICON_SIZE * i + SPACE_BETWEEN_HEARTS, slight_vertical_offset}, ICON_SIZE);
    }

    // Timer
    Vector2 timer_dimensions = MeasureTextEx(menu_font, std::to_string(timer / 60).c_str(), ICON_SIZE, 2.0f);
    Vector2 timer_position = {(GetRenderWidth() - timer_dimensions.x) * 0.5f, slight_vertical_offset};
    DrawTextEx(menu_font, std::to_string(timer / 60).c_str(), timer_position, ICON_SIZE, 2.0f, WHITE);

    // Score
    Vector2 score_dimensions = MeasureTextEx(menu_font, std::to_string(player->getTotalScore()).c_str(), ICON_SIZE, 2.0f);
    Vector2 score_position = {GetRenderWidth() - score_dimensions.x - ICON_SIZE, slight_vertical_offset};
    DrawTextEx(menu_font, std::to_string(player->getTotalScore()).c_str(), score_position, ICON_SIZE, 2.0f, WHITE);
    draw_sprite(coin_sprite, {GetRenderWidth() - ICON_SIZE, slight_vertical_offset}, ICON_SIZE);
}

void draw_level() {
    Player* player = Player::getInstance();
    Level* level = Level::getInstance();
    Vector2 playerPos = player->getPosition();
    const struct level& currentLevel = level->getCurrentLevel();

    // Move the x-axis' center to the middle of the screen
    horizontal_shift = (screen_size.x - cell_size) / 2;

    for (size_t row = 0; row < currentLevel.rows; ++row) {
        for (size_t column = 0; column < currentLevel.columns; ++column) {

            Vector2 pos = {
                    // Move the level to the left as the player advances to the right,
                    // shifting to the left to allow the player to be centered later
                    (static_cast<float>(column) - playerPos.x) * cell_size + horizontal_shift,
                    static_cast<float>(row) * cell_size
            };

            // Draw the level itself
            char cell = level->getLevelCell(row, column);
            switch (cell) {
                case WALL:
                    draw_image(wall_image, pos, cell_size);
                    break;
                case WALL_DARK:
                    draw_image(wall_dark_image, pos, cell_size);
                    break;
                case SPIKE:
                    draw_image(spike_image, pos, cell_size);
                    break;
                case COIN:
                    draw_sprite(coin_sprite, pos, cell_size);
                    break;
                case EXIT:
                    draw_image(exit_image, pos, cell_size);
                    break;
                default:
                    break;
            }
        }
    }

    draw_player();
    draw_enemies();
}

void draw_player() {
    Player* player = Player::getInstance();
    Vector2 playerPos = player->getPosition();
    horizontal_shift = (screen_size.x - cell_size) / 2;

    // Shift the camera to the center of the screen to allow to see what is in front of the player
    Vector2 pos = {
            horizontal_shift,
            playerPos.y * cell_size
    };

    // Pick an appropriate sprite for the player
    if (game_state == GAME_STATE) {
        if (!player->isOnGround()) {
            draw_image((player->isLookingForward() ? player_jump_forward_image : player_jump_backwards_image), pos, cell_size);
        }
        else if (player->isMoving()) {
            draw_sprite((player->isLookingForward() ? player_walk_forward_sprite : player_walk_backwards_sprite), pos, cell_size);
            player->setMoving(false);
        }
        else {
            draw_image((player->isLookingForward() ? player_stand_forward_image : player_stand_backwards_image), pos, cell_size);
        }
    }
    else {
        draw_image(player_dead_image, pos, cell_size);
    }
}

void draw_enemies() {
    Player* player = Player::getInstance();
    Vector2 playerPos = player->getPosition();

    // Get all enemies from the Enemy class
    const std::vector<Enemy>& allEnemies = Enemy::getAllEnemies();

    // Go over all enemies and draw them, accounting for the player's movement and horizontal shift
    for (const auto &enemy : allEnemies) {
        horizontal_shift = (screen_size.x - cell_size) / 2;

        Vector2 pos = {
            (enemy.getPosition().x - playerPos.x) * cell_size + horizontal_shift,
            enemy.getPosition().y * cell_size
        };

        draw_sprite(enemy_walk, pos, cell_size);
    }
}

// Menus
void draw_menu() {
    draw_text(game_title);
    draw_text(game_subtitle);
}

void draw_pause_menu() {
    draw_text(game_paused);
}

void draw_death_screen() {
    draw_parallax_background();
    draw_level();
    draw_game_overlay();
    DrawRectangle(0, 0, GetRenderWidth(), GetRenderHeight(), {0, 0, 0, 100});
    draw_text(death_title);
    draw_text(death_subtitle);
}

void draw_game_over_menu() {
    draw_text(game_over_title);
    draw_text(game_over_subtitle);
}

void create_victory_menu_background() {
    for (auto &ball : victory_balls) {
        ball.x  = rand_up_to(screen_size.x);
        ball.y  = rand_up_to(screen_size.y);
        ball.dx = rand_from_to(-VICTORY_BALL_MAX_SPEED, VICTORY_BALL_MAX_SPEED);
        ball.dx *= screen_scale;
        if (abs(ball.dx) < 0E-1) ball.dx = 1.0f;
        ball.dy = rand_from_to(-VICTORY_BALL_MAX_SPEED, VICTORY_BALL_MAX_SPEED);
        ball.dy *= screen_scale;
        if (abs(ball.dy) < 0E-1) ball.dy = 1.0f;
        ball.radius = rand_from_to(VICTORY_BALL_MIN_RADIUS, VICTORY_BALL_MAX_RADIUS);
        ball.radius *= screen_scale;
    }

    /* Clear both the front buffer and the back buffer to avoid ghosting of the game graphics. */
    ClearBackground(BLACK);
    EndDrawing();
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
    BeginDrawing();
}

void animate_victory_menu_background() {
    for (auto &ball : victory_balls) {
        ball.x += ball.dx;
        if (ball.x - ball.radius < 0 ||
            ball.x + ball.radius >= screen_size.x) {
            ball.dx = -ball.dx;
        }
        ball.y += ball.dy;
        if (ball.y - ball.radius < 0 ||
            ball.y + ball.radius >= screen_size.y) {
            ball.dy = -ball.dy;
        }
    }
}

void draw_victory_menu_background() {
    for (auto &ball : victory_balls) {
        DrawCircleV({ ball.x, ball.y }, ball.radius, VICTORY_BALL_COLOR);
    }
}

void draw_victory_menu() {
    DrawRectangle(
        0, 0,
        static_cast<int>(screen_size.x), static_cast<int>(screen_size.y),
        { 0, 0, 0, VICTORY_BALL_TRAIL_TRANSPARENCY }
    );

    animate_victory_menu_background();
    draw_victory_menu_background();

    draw_text(victory_title);
    draw_text(victory_subtitle);
}

#endif //GRAPHICS_H