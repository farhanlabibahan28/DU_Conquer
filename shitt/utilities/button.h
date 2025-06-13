#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"

class Button {
public:
    Rectangle bounds;
    const char* label;
    Color textColorNormal;
    Color textColorHover;

    Button(float x, float y, float width, float height, const char* text)
        : bounds{ x, y, width, height },
          label(text),
          textColorNormal(WHITE),
          textColorHover{255, 0, 255, 255}  // Purple-ish hover color
    {}

    bool IsHovered() const {
        return CheckCollisionPointRec(GetMousePosition(), bounds);
    }

    bool IsClicked() const {
        return IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    void DrawButton(int fontSize, Color bgColor) const {
    // DrawRectangleRec(bounds, bgColor); // ❌ remove or comment this line

    Color currentTextColor = IsHovered() ? textColorHover : textColorNormal;
    int textWidth = MeasureText(label, fontSize);
    DrawText(label,
             bounds.x + (bounds.width - textWidth) / 2,
             bounds.y + (bounds.height - fontSize) / 2,
             fontSize,
             currentTextColor);
    }

};

#endif // BUTTON_H
