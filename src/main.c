#include <string.h>
#include <ctype.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gray.h>

#include "common.h"
#include "keys.h"
#include "ui.h"
#include "serial.h"

#define COLS 21
#define ROWS 8

extern font_t font_mini;
extern bopti_image_t img_splash;

char console[ROWS][COLS];

unsigned int cursorCol = 0;
unsigned int cursorRow = 0;

void console_clear() {
    cursorCol = 0;
    cursorRow = 0;

    for (size_t row = 0; row < ROWS; row++) {
        for (size_t col = 0; col < COLS; col++) {
            console[row][col] = '\0';
        }
    }
}

void console_nextRow() {
    if (cursorRow >= ROWS - 1) {
        cursorRow = ROWS - 1;

        for (size_t row = 0; row < ROWS - 1; row++) {
            strncpy(console[row], console[row + 1], COLS);
        }

        for (size_t col = 0; col < COLS; col++) {
            console[ROWS - 1][col] = '\0';
        }
    } else {
        cursorRow++;
    }
}

void console_nextCol() {
    cursorCol++;

    if (cursorCol >= COLS) {
        console_nextRow();

        cursorCol = 0;
    }
}

void console_render() {
    dclear(C_WHITE);

    for (size_t row = 0; row < ROWS; row++) {
        for (size_t col = 0; col < COLS; col++) {
            char cell = console[row][col];

            if (cell == '\0') {
                continue;
            }

            dprint(col * 6, row * 8, C_BLACK, "%c", cell);
        }
    }

    drect(cursorCol * 6, cursorRow * 8, (cursorCol * 6) + 5, (cursorRow * 8) + 7, C_INVERT);

    dupdate();
}

void console_writeByte(char byte) {
    switch (byte) {
        case '\x7F': // Backspace
            if (cursorCol > 0) {
                cursorCol--;
            }

            console[cursorRow][cursorCol] = '\0';

            break;

        case '\n': // Newline
            console_nextRow();

            // Fall through

        case '\r': // Carriage return
            cursorCol = 0;
            break;

        default:
            console[cursorRow][cursorCol] = byte;

            console_nextCol();

            break;
    }

    console_render();
}

void console_write(char* bytes) {
    size_t i = 0;

    while (bytes[i] != '\0') {
        console_writeByte(bytes[i++]);
    }
}

int main(void) {
    dgray(DGRAY_ON);
    dgray_setdelays(869, 1311);

    dimage(0, 0, &img_splash);

    dfont(&font_mini);
    dtext(4, 48, C_WHITE, "Version 0.1.0");
    dtext(4, 56, C_WHITE, "James Livesey");
    dfont(dfont_default());

    dupdate();

    while (true) {
        if (keys_poll()) {
            if (keys_getEvent().key == KEY_EXIT) {
                break;
            }
        }
    }

    dgray(DGRAY_OFF);

    UiScreen* screen = ui_newScreen();
    UiElement* button1 = ui_newButton(screen, (UiBoundingBox) {0, 0, 96, 12}, "Hello, world!");
    unused UiElement* label = ui_newLabel(screen, (UiBoundingBox) {0, 16, 96, 12}, "This is fxNET");
    UiElement* button2 = ui_newButton(screen, (UiBoundingBox) {32, 32, 96, 12}, "Testing!");
    unused UiElement* input = ui_newInput(screen, (UiBoundingBox) {16, 48, 96, 12}, 16);

    button1->focusRight = button2;
    button2->focusLeft = button1;

    while (ui_renderScreen(screen)) {}

    console_clear();
    serial_init();

    console_write("fxNET serial console\n");

    if (!serial_open()) {
        console_write("Couldn't connect to\nserial interface\n");

        getkey();

        return 1;
    }

    console_write("Serial interface\nopened\n");

    while (true) {
        if (serial_readAvailable()) {
            console_writeByte(serial_readByte());
        }

        if (keys_poll()) {
            if (keys_getEvent().key == KEY_MENU) {
                break;
            }

            if (keys_getEvent().key == KEY_F5) {
                keys_toggleCasingState();

                continue;
            }

            char keyString[16];
            
            strcpy(keyString, keys_getString(false));

            if (keys_casingState == CASING_STATE_LOWER) {
                for (size_t i = 0; i < strlen(keyString); i++) {
                    keyString[i] = tolower(keyString[i]);
                }
            }

            serial_write(keyString, strlen(keyString));
        }
    }

    serial_close();

    return 1;
}