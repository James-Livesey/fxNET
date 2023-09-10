#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>

#include "serial.h"

int main(void) {
    serial_init();

    dclear(C_WHITE);
    dtext(0, 0, C_BLACK, "Hello from fxNET!");
    dupdate();
    sleep_us(1e6);

    if (!serial_open()) {
        dtext(0, 8, C_BLACK, "Open fail");
        dupdate();

        getkey();

        return 1;
    }

    if (!serial_isOpen()) {
        dtext(0, 8, C_BLACK, "Not open");
        dupdate();

        getkey();

        return 1;
    }

    dtext(0, 8, C_BLACK, "Opened");
    dupdate();
    sleep_us(1e6);

    dprint(0, 16, C_BLACK, "Buffer size: %d", serial_txBufferFreeSize());
    dupdate();
    sleep_us(1e6);

    char data[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    serial_write(data, sizeof(data));

    dtext(0, 24, C_BLACK, "Written");
    dupdate();
    sleep_us(1e6);

    dclear(C_WHITE);
    dprint(0, 0, C_BLACK, "Read data: %d", serial_readByte());
    dupdate();
    sleep_us(1e6);

    dprint(0, 8, C_BLACK, "Read data: %d", serial_readByte());
    dupdate();
    sleep_us(1e6);

    serial_close();

    return 1;
}