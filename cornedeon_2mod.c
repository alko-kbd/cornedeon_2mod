#ifdef __INTELLISENSE__
    #include "quantum.h"
    #include "info_config.h"
    #include "default_keyboard.h"
    #include "config.h"
    #include "debug.h"
#else
    #include QMK_KEYBOARD_H
#endif // __INTELLISENSE__
#ifdef VIAL_ENABLE
    #include "dynamic_keymap.h"
#endif // VIAL_ENABLE
#ifdef DEBUG_ENABLE
    #include "debug.h"
#endif
#ifdef CONSOLE_ENABLE
    #include "print.h"
#endif // CONSOLE_ENABLE
#include "analog.h"
#include "transactions.h"
#include "split_util.h"

#ifdef CUSTOM_JOYSTICK_ENABLE

typedef struct {
    int input_pin;
    uint16_t min_value;
    uint16_t mid_value;
    uint16_t max_value;
} custom_joystick_config_t;

typedef struct {
    uint8_t button_state;
    int16_t delta_x;
    uint16_t delay_x;
    int16_t delta_y;
    uint16_t delay_y;
} custom_joystick_state_t;

//static int button_state = 0;
custom_joystick_config_t joystick_config[JOYSTICK_AXIS_COUNT] = {
    { JOYSTICK_AXIS_X_PIN, 45, 516, 930 },
    { JOYSTICK_AXIS_Y_PIN, 85, 516, 970 }
};

custom_joystick_state_t prev_joystick_state = { 0, 0, 0, 0, 0 };
custom_joystick_state_t joystick_state = { 0, 0, 0, 0, 0 };

static bool is_joystick_side = false;

// Joystick threshold and repeating
#define JS_THRESHOLD 80           // Deathzone center
#define JS_MAX_SPEED_DELAY 15     // Faster - min delay
#define JS_MIN_SPEED_DELAY 200    // Slower - max delay

// Joystick mapped matrix keys
#define JS_KEY_ROW        4
#define JS_KEY_COL_LEFT   0
#define JS_KEY_COL_UP     1
#define JS_KEY_COL_DOWN   2
#define JS_KEY_COL_RIGHT  3
#define JS_KEY_COL_BUTTON 4

void init_joystick(void) {
    is_joystick_side = 
    #ifdef JOYSTICK_LEFT
        is_keyboard_left();
    #else
        !is_keyboard_left();
    #endif
    uprintf("is_joystick_side: %d\n", is_joystick_side);
    if (is_joystick_side) {
        dprint("init joystick\n");
        setPinInputHigh(JOYSTICK_BUTTON_PIN);
        joystick_config[0].mid_value = analogReadPin(joystick_config[0].input_pin);
        joystick_config[1].mid_value = analogReadPin(joystick_config[1].input_pin);
    }
}

// Функция для маппинга значений
// Преобразует отклонение в задержку (мс)
uint16_t get_joystick_delay(int16_t diff, custom_joystick_config_t* cfg) {
    int abs_diff = abs(diff);
    int max = 512;
    if (diff > 0)
        max = cfg->max_value - cfg->mid_value;
    else
        max = cfg->mid_value - cfg->min_value;
    if (abs_diff < JS_THRESHOLD)  // In deathzone
        return 0;
    uint16_t delay = JS_MIN_SPEED_DELAY - ((abs_diff - JS_THRESHOLD) * (JS_MIN_SPEED_DELAY - JS_MAX_SPEED_DELAY) / (max - JS_THRESHOLD));
    return delay;
}

//extern uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS];

void update_joystick_state(void) {
    prev_joystick_state = joystick_state;

    // Read button state
    joystick_state.button_state = readPin(JOYSTICK_BUTTON_PIN);

    // Read axis values
    int16_t x_value = analogReadPin(joystick_config[0].input_pin);
    joystick_state.delta_x = x_value - joystick_config[0].mid_value;
    joystick_state.delay_x = get_joystick_delay(joystick_state.delta_x, &joystick_config[0]);

    int16_t y_value = analogReadPin(joystick_config[1].input_pin);
    joystick_state.delta_y = y_value - joystick_config[1].mid_value;
    joystick_state.delay_y = get_joystick_delay(joystick_state.delta_y, &joystick_config[1]);

    // For calibrate
    //uprintf("Joystick raw: X=%d Y=%d\n", x_value, y_value);

    //if (x_delay > 0 || y_delay > 0)
    //    uprintf("Joystick: x_delay=%d y_delay=%d\n", x_delay, y_delay);
}

// Axis timers
static uint16_t timer_x = 0;
static uint16_t timer_y = 0;

extern const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS];

void process_joystick_state(void) {
    // Button state
    if (joystick_state.button_state == 0 && joystick_state.button_state != prev_joystick_state.button_state) {
        uint8_t layer = get_highest_layer(layer_state);
        uint16_t keycode = KC_NO;
        #ifdef VIAL_ENABLE
            keycode = dynamic_keymap_get_keycode(layer, JS_KEY_ROW, JS_KEY_COL_BUTTON);
        #endif
        if (keycode == KC_NO) {
            keycode = keymaps[layer][JS_KEY_ROW][JS_KEY_COL_BUTTON];
        }
        tap_code16(keycode);
    }

    // X axis
    if (joystick_state.delay_x > 0 && timer_elapsed(timer_x) > joystick_state.delay_x) {
        uint8_t layer = get_highest_layer(layer_state);
        int key_idx =
            #ifdef JOYSTICK_INVERT_X_AXIS
                (joystick_state.delta_x > 0)
            #else
                (joystick_state.delta_x < 0)
            #endif
            ? JS_KEY_COL_LEFT : JS_KEY_COL_RIGHT;
        uint16_t keycode = KC_NO;
        #ifdef VIAL_ENABLE
            keycode = dynamic_keymap_get_keycode(layer, JS_KEY_ROW, key_idx);
        #endif
        if (keycode == KC_NO) {
            keycode = keymaps[layer][JS_KEY_ROW][key_idx];
        }
        tap_code16(keycode);
        timer_x = timer_read();
    }

    // Y axis - inverted
    if (joystick_state.delay_y > 0 && timer_elapsed(timer_y) > joystick_state.delay_y) {
        uint8_t layer = get_highest_layer(layer_state);
        int key_idx =
            #ifdef JOYSTICK_INVERT_Y_AXIS
                (joystick_state.delta_y < 0)
            #else
                (joystick_state.delta_y > 0)
            #endif
            ? JS_KEY_COL_UP : JS_KEY_COL_DOWN;
        uint16_t keycode = KC_NO;
        #ifdef VIAL_ENABLE
            keycode = dynamic_keymap_get_keycode(layer, JS_KEY_ROW, key_idx);
        #endif
        if (keycode == KC_NO) {
            keycode = keymaps[layer][JS_KEY_ROW][key_idx];
        }
        tap_code16(keycode);
        timer_y = timer_read();
    }

}

#endif // CUSTOM_JOYSTICK_ENABLE

static uint32_t last_sync = 0;

void sync_slave_handler(uint8_t in_size, const void* in_data, uint8_t out_size, void* out_data) {
#ifdef CUSTOM_JOYSTICK_ENABLE
    memcpy(out_data, &joystick_state, sizeof(joystick_state));
#endif
}

void keyboard_post_init_kb(void) {
    last_sync = timer_read32();
    transaction_register_rpc(SYNC_JOYSTICK_TR_ID, sync_slave_handler);
#ifdef CUSTOM_JOYSTICK_ENABLE
    init_joystick();
#endif // CUSTOM_JOYSTICK_ENABLE
    keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
    if (timer_elapsed32(last_sync) < 10)
        return;
    last_sync = timer_read32();

#ifdef CUSTOM_JOYSTICK_ENABLE
    if (is_joystick_side) {
        update_joystick_state();
        if (is_keyboard_master()) {
            process_joystick_state();
            // if (joystick_state.delay_x > 0 || joystick_state.delay_y > 0)
            //     uprintf("Joy: dX=%d dY=%d\n", joystick_state.delta_x, joystick_state.delta_y);
        }
    } else {
        if (!is_keyboard_master())
            return;
        prev_joystick_state = joystick_state;
        if (transaction_rpc_exec(SYNC_JOYSTICK_TR_ID, 0, 0, sizeof(joystick_state), &joystick_state)) {
            // uprintf("Sync slave success: dX=%d _X=%d dY=%d _Y=%d\n", 
            //     joystick_state.delta_x, joystick_state.delay_x, joystick_state.delta_y, joystick_state.delay_y);
            process_joystick_state();
        }
    }
#endif // CUSTOM_JOYSTICK_ENABLE
}
