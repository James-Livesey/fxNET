#include <stdlib.h>
#include <gint/gint.h>
#include <gint/display.h>

#include "ui.h"
#include "common.h"
#include "keys.h"

UiElement* ui_newElement(UiScreen* screen, UiBoundingBox bounds) {
    UiElement* element = malloc(sizeof(UiElement));

    element->type = UI_ELEMENT_TYPE_NONE;
    element->screen = screen;
    element->bounds = bounds;
    element->data = NULL;
    element->isFocusable = true;
    element->focusLeft = NULL;
    element->focusRight = NULL;
    element->next = NULL;

    for (size_t i = 0; i < _UI_EVENT_TYPES_COUNT; i++) {
        element->events[i] = NULL;
    }

    screen->modified = true;
    screen->elementCount++;

    if (screen->firstElement) {
        UiElement* lastElement = screen->firstElement;

        while (true) {
            UiElement* nextElement = lastElement->next;

            if (nextElement == NULL) {
                break;
            }

            lastElement = nextElement;
        }

        lastElement->next = element;
    } else {
        screen->firstElement = element;
    }

    return element;
}

UiElement* ui_newLabel(UiScreen* screen, UiBoundingBox bounds, char* text) {
    UiElement* element = ui_newElement(screen, bounds);
    _UiLabelData* data = malloc(sizeof(_UiLabelData));

    data->text = text;

    element->type = UI_ELEMENT_TYPE_LABEL;
    element->data = data;
    element->isFocusable = false;

    return element;
}

UiElement* ui_newButton(UiScreen* screen, UiBoundingBox bounds, char* text) {
    UiElement* element = ui_newElement(screen, bounds);
    _UiButtonData* data = malloc(sizeof(_UiButtonData));

    data->text = text;

    element->type = UI_ELEMENT_TYPE_BUTTON;
    element->data = data;

    return element;
}

void ui_destroyElement(UiElement* element) {
    element->screen->modified = true;
    element->screen->elementCount--;

    if (element->screen->firstElement == element) {
        element->screen->firstElement = element->next;
    } else {
        UiElement* currentElement = element->screen->firstElement;
        UiElement* previousElement = currentElement;
        bool shouldRelink = true;

        while (currentElement != element) {
            if (currentElement == NULL) {
                shouldRelink = false;

                break;
            }

            previousElement = currentElement;
            currentElement = currentElement->next;
        }

        if (shouldRelink) {
            previousElement->next = element->next;
        }
    }

    if (element->data) {
        free(element->data);
    }

    free(element);
}

bool ui_dispatchElementEvent(UiElement* element, UiEventType eventType, void* data) {
    if (!element) {
        return true;
    }

    if (!element->events[eventType]) {
        return false;
    }

    return (*element->events[eventType])(element, data);
}

void renderLabel(UiElement* element, unused bool isFocused) {
    _UiButtonData* data = element->data;

    dtext_opt(element->bounds.x, element->bounds.y, C_BLACK, C_WHITE, DTEXT_LEFT, DTEXT_TOP, data->text, -1);
}

void renderButton(UiElement* element, bool isFocused) {
    unsigned int x1 = element->bounds.x;
    unsigned int y1 = element->bounds.y;
    unsigned int x2 = x1 + element->bounds.width - 1;
    unsigned int y2 = y1 + element->bounds.height - 1;

    dline(x1 + 1, y1, x2 - 1, y1, C_BLACK);
    dline(x1 + 1, y2, x2 - 1, y2, C_BLACK);
    dline(x1, y1 + 1, x1, y2 - 1, C_BLACK);
    dline(x2, y1 + 1, x2, y2 - 1, C_BLACK);

    drect(x1 + 1, y1 + 1, x2 - 1, y2 - 1, isFocused ? C_BLACK : C_WHITE);

    _UiButtonData* data = element->data;

    dtext_opt((x1 + x2) / 2, (y1 + y2) / 2, C_INVERT, C_NONE, DTEXT_CENTER, DTEXT_CENTER, data->text, -1);
}

void ui_renderElement(UiElement* element, bool isFocused) {
    switch (element->type) {
        case UI_ELEMENT_TYPE_NONE: return;
        case UI_ELEMENT_TYPE_LABEL: return renderLabel(element, isFocused);
        case UI_ELEMENT_TYPE_BUTTON: return renderButton(element, isFocused);
    }
}

UiScreen* ui_newScreen() {
    UiScreen* screen = malloc(sizeof(UiScreen));

    screen->firstElement = NULL;
    screen->modified = true;
    screen->focusedElementIndex = 0;
    screen->elementCount = 0;

    return screen;
}

void ui_destroyScreen(UiScreen* screen) {
    if (screen->firstElement) {
        UiElement* currentElement = screen->firstElement;

        while (currentElement) {
            UiElement* nextElement = currentElement->next;

            if (currentElement->data) {
                free(currentElement->data);
            }

            free(currentElement);

            currentElement = nextElement;
        }
    }

    free(screen);
}

UiElement* ui_getElementByIndex(UiScreen* screen, unsigned int index) {
    UiElement* currentElement = screen->firstElement;

    while (currentElement && index > 0) {
        currentElement = currentElement->next;
        index--;
    }

    return currentElement;
}

unsigned int ui_getIndexOfElement(UiScreen* screen, UiElement* element) {
    UiElement* currentElement = screen->firstElement;
    unsigned int i = 0;

    while (currentElement) {
        if (currentElement == element) {
            return i;
        }

        currentElement = currentElement->next;
        i++;
    }

    return i;
}

UiElement* ui_getFocusedElement(UiScreen* screen) {
    return ui_getElementByIndex(screen, screen->focusedElementIndex);
}

bool ui_dispatchFocusedElementEvent(UiScreen* screen, UiEventType eventType, void* data) {
    return ui_dispatchElementEvent(ui_getFocusedElement(screen), eventType, data);
}

bool ui_renderScreen(UiScreen* screen) {
    bool shouldExit = false;

    UiElement* focusedElement = ui_getFocusedElement(screen);

    if (keys_poll()) {
        switch (keys_getEvent().key) {
            case KEY_MENU:
                gint_osmenu();
                break;

            case KEY_EXIT:
                shouldExit = true;
                break;

            case KEY_UP:
            case KEY_DOWN:
                if (screen->elementCount == 0) {
                    break;
                }

                unsigned int initialIndex = screen->focusedElementIndex;

                screen->modified = true;

                do {
                    if (keys_getEvent().key == KEY_UP) {
                        if (screen->focusedElementIndex <= 0) {
                            screen->focusedElementIndex = screen->elementCount - 1;

                            break;
                        }

                        screen->focusedElementIndex--;
                    } else {
                        if (screen->focusedElementIndex >= screen->elementCount - 1) {
                            screen->focusedElementIndex = 0;

                            break;
                        }

                        screen->focusedElementIndex++;
                    }
                } while (!ui_getFocusedElement(screen)->isFocusable && screen->focusedElementIndex != initialIndex);

                break;

            case KEY_LEFT:
                if (focusedElement && focusedElement->focusLeft) {
                    screen->focusedElementIndex = ui_getIndexOfElement(screen, focusedElement->focusLeft);
                    screen->modified = true;
                }

                break;

            case KEY_RIGHT:
                if (focusedElement && focusedElement->focusRight) {
                    screen->focusedElementIndex = ui_getIndexOfElement(screen, focusedElement->focusRight);
                    screen->modified = true;
                }

                break;

            case KEY_EXE:
                if (ui_dispatchFocusedElementEvent(screen, UI_EVENT_CLICK, NULL)) {
                    goto render;
                }

                break;
        }
    }

    render:

    focusedElement = ui_getFocusedElement(screen);

    if (screen->modified) {
        UiElement* currentElement = screen->firstElement;

        dclear(C_WHITE);

        while (currentElement) {
            ui_renderElement(currentElement, currentElement == focusedElement);

            currentElement = currentElement->next;
        }

        dupdate();

        screen->modified = false;
    }

    return !shouldExit;
}