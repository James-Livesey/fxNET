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
        element->screen->firstElement = NULL;
    } else {
        UiElement* currentElement = element->screen->firstElement;
        UiElement* previousElement = currentElement;

        while (currentElement != element) {
            if (currentElement == NULL) {
                // TODO: Handle edge case by preventing relinking
            }

            previousElement = currentElement;
            currentElement = currentElement->next;
        }

        previousElement->next = currentElement->next;
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
    // TODO: Destroy all elements

    if (screen->firstElement) {
        ui_destroyElement(screen->firstElement);
    }

    free(screen);
}