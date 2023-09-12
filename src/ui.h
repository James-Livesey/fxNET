#ifndef UI_H_
#define UI_H_

typedef enum {
    UI_ELEMENT_TYPE_NONE,
    UI_ELEMENT_TYPE_BUTTON
} UiElementType;

typedef struct {
    unsigned int x;
    unsigned int y;
} UiPos;

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} UiBoundingBox;

typedef struct UiElement {
    UiElementType type;
    struct UiScreen* screen;
    UiBoundingBox bounds;
    struct UiElement* next;
} UiElement;

typedef struct UiScreen {
    UiElement* firstElement;
    unsigned int focusedElementIndex;
} UiScreen;

UiElement* ui_newElement(UiScreen* screen, UiBoundingBox bounds);
void ui_destroyElement(UiElement* element);
UiScreen* ui_newScreen();
void ui_destroyScreen(UiScreen* screen);

#endif