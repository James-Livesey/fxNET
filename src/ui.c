#include <stdlib.h>
#include <stdbool.h>

#include "ui.h"

UiElement* ui_newElement(UiScreen* screen, UiBoundingBox bounds) {
    UiElement* element = malloc(sizeof(UiElement));

    element->type = UI_ELEMENT_TYPE_NONE;
    element->screen = screen;
    element->bounds = bounds;
    element->next = NULL;

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

void ui_destroyElement(UiElement* element) {
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

    free(element);
}

UiScreen* ui_newScreen() {
    UiScreen* screen = malloc(sizeof(UiScreen));

    screen->firstElement = NULL;
    screen->focusedElementIndex = 0;

    return screen;
}

void ui_destroyScreen(UiScreen* screen) {
    if (screen->firstElement) {
        UiElement* currentElement = screen->firstElement;

        while (currentElement) {
            UiElement* nextElement = currentElement->next;

            free(currentElement);

            currentElement = nextElement;
        }
    }

    free(screen);
}