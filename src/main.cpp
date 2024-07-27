#include "raylib.h"
#include <iostream>
#include <cmath>
#include <random>

constexpr int RENDER_WIDTH = 3600;  // 4K width
constexpr int RENDER_HEIGHT = 3600; // 4K height
constexpr int DISPLAY_WIDTH = 800; // Actual display width
constexpr int DISPLAY_HEIGHT = 800; // Actual display height
constexpr float INITIAL_BALL_SPEED = 400.0f;
constexpr float SPEED_INCREASE = 1.05f;
constexpr float SIZE_INCREASE = 5.0f;

// Rainbow colors array
constexpr Color rainbowColors[] = {
        {255, 0,   0,   255},   // Red
        {255, 51,  0,   255},   // Orange-Red
        {255, 102, 0,   255},   // Dark Orange
        {255, 153, 0,   255},   // Orange
        {255, 204, 0,   255},   // Amber
        {255, 255, 0,   255},   // Yellow
        {204, 255, 0,   255},   // Lime
        {153, 255, 0,   255},   // Chartreuse
        {51,  255, 0,   255},   // Harlequin
        {0,   255, 51,  255},   // Mint
        {0,   255, 153, 255},   // Aquamarine
        {0,   255, 204, 255},   // Turquoise
        {0,   255, 255, 255},   // Cyan
        {0,   204, 255, 255},   // Sky Blue
        {0,   153, 255, 255},   // Azure
        {0,   102, 255, 255},   // Cerulean
        {0,   51,  255, 255},   // Sapphire
        {0,   0,   255, 255},   // Blue
        {51,  0,   255, 255},   // Indigo
        {102, 0,   255, 255},   // Violet
        {153, 0,   255, 255},   // Purple
        {204, 0,   255, 255},   // Magenta
        {255, 0,   255, 255},   // Fuchsia
        {255, 0,   204, 255},   // Deep Pink
        {255, 0,   153, 255},   // Hot Pink
        {255, 0,   102, 255},   // Crimson
        {255, 0,   51,  255},   // Ruby
        {128, 128, 0,   255},   // Olive
        {128, 64,  0,   255},   // Brown
        {153, 102, 51,  255},   // Tan
        {245, 222, 179, 255},   // Beige
        {204, 204, 204, 255},   // Light Gray
        {128, 128, 128, 255},   // Gray
        {51,  51,  51,  255},   // Dark Gray
        {255, 215, 0,   255},   // Gold
        {192, 192, 192, 255},   // Silver
        {138, 43,  226, 255}    // Purple
};

constexpr int NUM_COLORS = sizeof(rainbowColors) / sizeof(rainbowColors[0]);

int main() {
    SetConfigFlags(FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_MAXIMIZED);
    InitWindow(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Bouncing Ball Animation");
    InitAudioDevice();
    SetTargetFPS(60);

    // Create render texture to draw the game at 4K resolution
    RenderTexture2D target = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);

    Sound collisionSound = LoadSound("../res/CartoonJump.mp3");

    Vector2 circleCenter = {RENDER_WIDTH / 2.0f, RENDER_HEIGHT / 2.0f};
    float circleRadius = std::min(RENDER_WIDTH, RENDER_HEIGHT) * 0.4f;
    float borderThickness = circleRadius * 0.04f;
    float ballRadius = circleRadius * 0.05f;

    Vector2 ballPosition = {circleCenter.x, circleCenter.y - circleRadius + ballRadius};
    Vector2 ballVelocity = {INITIAL_BALL_SPEED, INITIAL_BALL_SPEED};
    float ballSpeed = INITIAL_BALL_SPEED;

    bool animationComplete = false;
    bool animationStarted = false;
    auto ballColor = RED;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> colorDist(0, NUM_COLORS - 1);

    // Start button
    Rectangle startButton = {
            RENDER_WIDTH / 2.0f - 100,
            RENDER_HEIGHT / 2.0f - 30,
            200,
            60
    };

    while (!WindowShouldClose()) {
        // Check for start button click
        if (!animationStarted) {
            Vector2 mousePos = GetMousePosition();
            mousePos.x *= (float) RENDER_WIDTH / DISPLAY_WIDTH;
            mousePos.y *= (float) RENDER_HEIGHT / DISPLAY_HEIGHT;

            if (CheckCollisionPointRec(mousePos, startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                animationStarted = true;
            }
        }

        if (animationStarted && !animationComplete) {
            // Update ball position
            ballPosition.x += ballVelocity.x * GetFrameTime();
            ballPosition.y += ballVelocity.y * GetFrameTime();

            // Check collision with circle boundary
            Vector2 displacement = {ballPosition.x - circleCenter.x, ballPosition.y - circleCenter.y};
            float distance = sqrt(displacement.x * displacement.x + displacement.y * displacement.y);

            if (distance + ballRadius > circleRadius - borderThickness / 2) {
                // Normalize displacement vector
                float normalizedX = displacement.x / distance;
                float normalizedY = displacement.y / distance;

                // Set ball position to circle boundary (accounting for border thickness)
                ballPosition.x = circleCenter.x + normalizedX * (circleRadius - borderThickness / 2 - ballRadius);
                ballPosition.y = circleCenter.y + normalizedY * (circleRadius - borderThickness / 2 - ballRadius);

                // Reflect velocity vector
                float dot = ballVelocity.x * normalizedX + ballVelocity.y * normalizedY;
                ballVelocity.x -= 2 * dot * normalizedX;
                ballVelocity.y -= 2 * dot * normalizedY;

                // Increase ball speed and size
                ballSpeed *= SPEED_INCREASE;
                ballRadius += SIZE_INCREASE;

                // Normalize and apply new speed
                float velocityMagnitude = sqrt(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
                ballVelocity.x = (ballVelocity.x / velocityMagnitude) * ballSpeed;
                ballVelocity.y = (ballVelocity.y / velocityMagnitude) * ballSpeed;

                // Change ball color randomly
                ballColor = rainbowColors[colorDist(gen)];

                PlaySound(collisionSound);
            }

            // Check if animation is complete
            if (ballRadius >= circleRadius - borderThickness / 2) {
                animationComplete = true;
                ballRadius = circleRadius - borderThickness / 2;
                ballPosition = circleCenter;
                ballColor = GREEN;
            }
        }

        // Drawing
        BeginTextureMode(target);
        ClearBackground(BLACK);

        // Draw outer circle (green border only, with increased thickness)
        DrawRing(circleCenter, circleRadius - borderThickness / 2, circleRadius + borderThickness / 2, 0, 360, 360,
                 GREEN);

        // Draw Ball
        if (animationStarted) {
            DrawCircleSector(ballPosition, ballRadius, 0, 360, 360, ballColor);
        }

        // Draw start button if animation hasn't started
        if (!animationStarted) {
            DrawRectangleRec(startButton, RAYWHITE);
            DrawText("START", startButton.x + 50, startButton.y + 15, 30, BLACK);
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw the scaled render texture to fit the display
        DrawTexturePro(target.texture,
                       Rectangle{0, 0, (float) target.texture.width, (float) -target.texture.height},
                       Rectangle{0, 0, (float) DISPLAY_WIDTH, (float) DISPLAY_HEIGHT},
                       Vector2{0, 0}, 0.0f, WHITE);

        DrawFPS(10, 10);

        EndDrawing();
    }

    UnloadRenderTexture(target);
    UnloadSound(collisionSound);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}