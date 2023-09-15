#ifndef UI_H_
#define UI_H_

#include <stdbool.h>

typedef enum {
    UI_ELEMENT_TYPE_NONE,
    UI_ELEMENT_TYPE_LABEL,
    UI_ELEMENT_TYPE_BUTTON
} UiElementType;

typedef enum {
    UI_EVENT_CLICK = 0
} UiEventType;

#define _UI_EVENT_TYPES_COUNT 1

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
    void* data;
    bool (*events[_UI_EVENT_TYPES_COUNT])(struct UiElement* element, void* data);
    bool isFocusable;
    struct UiElement* focusLeft;
    struct UiElement* focusRight;
    struct UiElement* next;
} UiElement;

typedef struct {
    char* text;
} _UiLabelData;

typedef struct {
    char* text;
} _UiButtonData;

typedef struct UiScreen {
    UiElement* firstElement;
    bool modified;
    unsigned int focusedElementIndex;
    unsigned int elementCount;
} UiScreen;

UiElement* ui_newElement(UiScreen* screen, UiBoundingBox bounds);
UiElement* ui_newLabel(UiScreen* screen, UiBoundingBox bounds, char* text);
UiElement* ui_newButton(UiScreen* screen, UiBoundingBox bounds, char* text);

void ui_destroyElement(UiElement* element);
bool ui_dispatchElementEvent(UiElement* element, UiEventType eventType, void* data);
void ui_renderElement(UiElement* element, bool isFocused);

UiScreen* ui_newScreen();
void ui_destroyScreen(UiScreen* screen);
UiElement* ui_getElementByIndex(UiScreen* screen, unsigned int index);
unsigned int ui_getIndexOfElement(UiScreen* screen, UiElement* element);
UiElement* ui_getFocusedElement(UiScreen* screen);
bool ui_dispatchFocusedElementEvent(UiScreen* screen, UiEventType eventType, void* data);
bool ui_renderScreen(UiScreen* screen);

#endif