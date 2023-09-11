#include "keys.h"
 
#define ksm KeyStringMap

key_event_t lastKeyEvent;

ModifierState keys_modifierState = MODIFIER_STATE_NONE; // The modifier state that factors next key, should be read by external code
ModifierState immediateModifierState = MODIFIER_STATE_NONE; // The immediate modifier state that resets after next key is pressed
ModifierState internalModifierState = MODIFIER_STATE_NONE; // The modifier state that resets immediately after modifier key is pressed
CasingState keys_casingState = CASING_STATE_UPPER; // The state for whether characters should be uppercase or lowercase

const KeyStringMap KEY_STRINGS[] = {
    (ksm) {KEY_7, "7"}, (ksm) {KEY_8, "8"}, (ksm) {KEY_9, "9"}, (ksm) {KEY_DEL, "\x7F"},
    (ksm) {KEY_4, "4"}, (ksm) {KEY_5, "5"}, (ksm) {KEY_6, "6"},
    (ksm) {KEY_1, "1"}, (ksm) {KEY_2, "2"}, (ksm) {KEY_3, "3"},
    (ksm) {KEY_0, "0"}, (ksm) {KEY_DOT, "."}, (ksm) {KEY_NEG, "-"}, (ksm) {KEY_EXE, "\n"},
    (ksm) {-1, ""}
};
 
const KeyStringMap KEY_STRINGS_ALPHA[] = {
    (ksm) {KEY_SQUARE, "r"}, (ksm) {KEY_POWER, "\xE6\x47"},
    (ksm) {KEY_XOT, "A"}, (ksm) {KEY_LOG, "B"}, (ksm) {KEY_LN, "C"}, (ksm) {KEY_SIN, "D"}, (ksm) {KEY_COS, "E"}, (ksm) {KEY_TAN, "F"},
    (ksm) {KEY_FRAC, "G"}, (ksm) {KEY_FD, "H"}, (ksm) {KEY_LEFTP, "I"}, (ksm) {KEY_RIGHTP, "J"}, (ksm) {KEY_COMMA, "K"}, (ksm) {KEY_ARROW, "L"},
    (ksm) {KEY_7, "M"}, (ksm) {KEY_8, "N"}, (ksm) {KEY_9, "O"}, (ksm) {KEY_DEL, "\x7F"},
    (ksm) {KEY_4, "P"}, (ksm) {KEY_5, "Q"}, (ksm) {KEY_6, "R"}, (ksm) {KEY_MUL, "S"}, (ksm) {KEY_DIV, "T"},
    (ksm) {KEY_1, "U"}, (ksm) {KEY_2, "V"}, (ksm) {KEY_3, "W"}, (ksm) {KEY_ADD, "X"}, (ksm) {KEY_SUB, "Y"},
    (ksm) {KEY_0, "Z"}, (ksm) {KEY_DOT, " "}, (ksm) {KEY_EXP, "\""}, (ksm) {KEY_EXE, "\n"},
    (ksm) {-1, ""}
};
 
char* keys_getStringManual(int keyCode, bool shift, bool alpha, bool filenameFormatOnly) {
    if (lastKeyEvent.type == KEYEV_NONE) {
        return "";
    }

    if (shift) {
        return ""; // No shift key string maps implemented yet
    }
 
    const KeyStringMap* map = alpha ? KEY_STRINGS_ALPHA : KEY_STRINGS;
 
    for (unsigned int i = 0; map[i].keyCode > 0; i++) {
        if (filenameFormatOnly) {
            if (alpha && (keyCode == KEY_SQUARE || keyCode == KEY_POWER || keyCode == KEY_EXP)) {
                return "";
            }
 
            if (keyCode == KEY_DOT || keyCode == KEY_NEG || keyCode == KEY_EXE) {
                return "";
            }
        }
 
        if (map[i].keyCode == keyCode) {
            return map[i].string;
        }
    }
 
    return "";
}

char* keys_getString(bool filenameFormatOnly) {
    return keys_getStringManual(
        lastKeyEvent.key,
        keys_modifierState == MODIFIER_STATE_SHIFT,
        keys_modifierState == MODIFIER_STATE_ALPHA || keys_modifierState == MODIFIER_STATE_ALPHA_LOCK,
        filenameFormatOnly
    );
}

bool keys_poll() {
    key_event_t keyEvent = pollevent();

    lastKeyEvent = keyEvent;

    if (lastKeyEvent.type != KEYEV_DOWN) {
        return false;
    }

    switch (keyEvent.key) {
        case KEY_SHIFT:
            internalModifierState = MODIFIER_STATE_SHIFT;
            break;

        case KEY_ALPHA:
            if (immediateModifierState == MODIFIER_STATE_ALPHA_LOCK) {
                internalModifierState = MODIFIER_STATE_NONE;

                break;
            }

            if (immediateModifierState == MODIFIER_STATE_SHIFT) {
                internalModifierState = MODIFIER_STATE_ALPHA_LOCK;

                break;
            }

            internalModifierState = MODIFIER_STATE_ALPHA;

            break;
    }

    keys_modifierState = immediateModifierState;
    immediateModifierState = internalModifierState;

    if ((immediateModifierState == MODIFIER_STATE_SHIFT && keyEvent.key != KEY_ALPHA) || immediateModifierState == MODIFIER_STATE_ALPHA) {
        internalModifierState = MODIFIER_STATE_NONE;
    }

    return true;
}

key_event_t keys_getEvent() {
    return lastKeyEvent;
}

void keys_toggleCasingState() {
    switch (keys_casingState) {
        case CASING_STATE_UPPER:
            keys_casingState = CASING_STATE_LOWER;
            break;

        default:
            keys_casingState = CASING_STATE_UPPER;
            break;
    }
}