#ifndef KEYS_H_
#define KEYS_H_

#include <gint/keyboard.h>

typedef enum {
    MODIFIER_STATE_NONE,
    MODIFIER_STATE_SHIFT,
    MODIFIER_STATE_ALPHA,
    MODIFIER_STATE_ALPHA_LOCK
} ModifierState;

typedef enum {
    CASING_STATE_UPPER,
    CASING_STATE_LOWER
} CasingState;

typedef struct {
    int keyCode;
    char* string;
} KeyStringMap;

extern ModifierState keys_modifierState;
extern CasingState keys_casingState;

char* keys_getStringManual(int keyCode, bool shift, bool alpha, bool filenameFormatOnly);
char* keys_getString(bool filenameFormatOnly);
bool keys_poll();
key_event_t keys_getEvent();
void keys_toggleCasingState();

#endif