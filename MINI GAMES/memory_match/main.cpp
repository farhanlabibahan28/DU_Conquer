#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cstdio>

#define MAX_PAIRS 8
#define MAX_CARDS (MAX_PAIRS * 2)
#define CARD_WIDTH 180
#define CARD_HEIGHT 120
#define PADDING 30
#define ROWS 4
#define COLUMNS 4
#define FLIP_TIME 0.3f

typedef struct {
    Rectangle rect;
    char label[32];
    bool revealed;
    bool matched;
    float flipTimer;
    bool flipping;
} Card;

const char *pairs[MAX_PAIRS][2] = {
    {"Raima", "irony"}, {"Rapunzel", "disney"},
    {"csedu", "death"}, {"Rana sir", "GOAT"},
    {"DU", "dream"}, {"Raisa", "cat"},
    {"Salwa", "high"}, {"Ahan", "guy"}
};

Card cards[MAX_CARDS];
int selectedIndices[2] = {-1, -1};
int matchedCount = 0;
float gameTime = 40.0f; // 40 seconds timer
bool gameOver = false;
Rectangle restartButton = {350, 560, 150, 30};

Color cardColor = (Color){200, 230, 255, 255};

void Shuffle(char labels[MAX_CARDS][32]) {
    for (int i = MAX_CARDS - 1; i > 0; i--) {
        int j = GetRandomValue(0, i);
        char temp[32];
        strcpy(temp, labels[i]);
        strcpy(labels[i], labels[j]);
        strcpy(labels[j], temp);
    }
}

void InitCards() {
    char labels[MAX_CARDS][32];
    for (int i = 0; i < MAX_PAIRS; i++) {
        strcpy(labels[2 * i], pairs[i][0]);
        strcpy(labels[2 * i + 1], pairs[i][1]);
    }
    Shuffle(labels);

    for (int i = 0; i < MAX_CARDS; i++) {
        int row = i / COLUMNS;
        int col = i % COLUMNS;
        cards[i].rect = (Rectangle){
            (float)(PADDING + col * (CARD_WIDTH + PADDING)),
            (float)(PADDING + row * (CARD_HEIGHT + PADDING)),
            CARD_WIDTH, CARD_HEIGHT
        };
        strcpy(cards[i].label, labels[i]);
        cards[i].revealed = false;
        cards[i].matched = false;
        cards[i].flipTimer = 0;
        cards[i].flipping = false;
    }
    selectedIndices[0] = -1;
    selectedIndices[1] = -1;
    matchedCount = 0;
    gameTime = 40.0f;
    gameOver = false;
}

bool IsMatchingPair(const char *a, const char *b) {
    for (int i = 0; i < MAX_PAIRS; i++) {
        if ((strcmp(pairs[i][0], a) == 0 && strcmp(pairs[i][1], b) == 0) ||
            (strcmp(pairs[i][1], a) == 0 && strcmp(pairs[i][0], b) == 0)) {
            return true;
        }
    }
    return false;
}

void UpdateGame(float delta) {
    if (gameOver) return;

    gameTime -= delta;
    if (gameTime <= 0) {
        gameOver = true;
        return;
    }

    for (int i = 0; i < MAX_CARDS; i++) {
        if (cards[i].flipping) {
            cards[i].flipTimer += delta;
            if (cards[i].flipTimer >= FLIP_TIME) {
                cards[i].flipTimer = 0;
                cards[i].flipping = false;
                cards[i].revealed = !cards[i].revealed;
            }
        }
    }

    if (selectedIndices[0] != -1 && selectedIndices[1] != -1) {
        int i1 = selectedIndices[0];
        int i2 = selectedIndices[1];

        if (!cards[i1].flipping && !cards[i2].flipping) {
            if (IsMatchingPair(cards[i1].label, cards[i2].label)) {
                cards[i1].matched = true;
                cards[i2].matched = true;
                matchedCount++;
            } else {
                cards[i1].flipping = true;
                cards[i2].flipping = true;
            }
            selectedIndices[0] = -1;
            selectedIndices[1] = -1;
        }
    }

    if (matchedCount == MAX_PAIRS) {
        gameOver = true;
    }
}

void DrawCard(Card *card, Font font) {
    Color borderColor = BLACK;
    DrawRectangleRec(card->rect, card->revealed || card->matched ? cardColor : DARKGRAY);
    DrawRectangleLinesEx(card->rect, 2, borderColor);
    if (card->revealed || card->matched) {
        Vector2 textSize = MeasureTextEx(font, card->label, 24, 2);
        DrawTextEx(font, card->label,
            (Vector2){card->rect.x + (CARD_WIDTH - textSize.x)/2, card->rect.y + (CARD_HEIGHT - textSize.y)/2},
            24, 2, BLACK);
    }
}

int main(void) {
    InitWindow(900, 700, "Memory Match Game");
    InitAudioDevice();
    SetTargetFPS(60);
    srand(time(NULL));

    Font font = LoadFont("resources/Roboto-Bold.ttf");
    if (!font.texture.id) font = GetFontDefault();

    InitCards();

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        UpdateGame(delta);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();

            if (gameOver && CheckCollisionPointRec(mouse, restartButton)) {
                InitCards();
            } else if (!gameOver) {
                for (int i = 0; i < MAX_CARDS; i++) {
                    if (!cards[i].revealed && !cards[i].matched && CheckCollisionPointRec(mouse, cards[i].rect)) {
                        if (selectedIndices[0] == -1) {
                            selectedIndices[0] = i;
                            cards[i].flipping = true;
                        } else if (selectedIndices[1] == -1 && i != selectedIndices[0]) {
                            selectedIndices[1] = i;
                            cards[i].flipping = true;
                        }
                        break;
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < MAX_CARDS; i++) DrawCard(&cards[i], font);

        char timerText[64];
        snprintf(timerText, sizeof(timerText), "Time Left: %.1f", gameTime);
        DrawTextEx(font, timerText, (Vector2){680, 20}, 22, 2, RED);

        if (gameOver) {
            const char* resultText = (matchedCount == MAX_PAIRS) ? "YOU WIN LESGOO!" : "WHARRA MEMORY HAHA";
            Vector2 size = MeasureTextEx(font, resultText, 38, 2);
            DrawTextEx(font, resultText, (Vector2){(900 - size.x)/2, 600}, 38, 2, BLUE);

            DrawRectangleRec(restartButton, LIGHTGRAY);
            DrawTextEx(font, "RESTART", (Vector2){restartButton.x + 20, restartButton.y + 5}, 22, 2, BLACK);
        }

        EndDrawing();
    }

    UnloadFont(font);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
