#ifdef __INTELLISENSE__
   #include "quantum.h"
   #include "info_config.h"
   #include "default_keyboard.h"
   #include "../../config.h"
#else
    #include QMK_KEYBOARD_H
#endif


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(
  //,-----------------------------------------------------.    ,-----------------------------------------------------.
       KC_ESC,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|    |--------+--------+--------+--------+--------+--------|
       KC_TAB,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_RSFT,
  //|--------+--------+--------+--------+--------+--------|    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, KC_LEFT,KC_RIGHT, KC_LALT, TL_LOWR,  KC_SPC,       KC_SPC, TL_UPPR, KC_RGUI, KC_DOWN,   KC_UP, KC_RCTL,
  //| Joystick keys---------------------------------------|    |-----------------------------------------------------|
      KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT, KC_SPC,   KC_NO,         KC_NO,   KC_NO,   KC_NO,  KC_NO,   KC_NO,   KC_NO
  //`------------------------------------------------------'   `-----------------------------------------------------'
  ),
  [1] = LAYOUT(
  //,-----------------------------------------------------.    ,-----------------------------------------------------.
       KC_TAB, KC_TILD, KC_LPRN, KC_RPRN, KC_PLUS,KC_MINUS,         KC_7,    KC_8,    KC_9,    KC_0,  KC_INS,  KC_DEL,
  //|--------+--------+--------+--------+--------+--------|    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, KC_LBRC, KC_RBRC,KC_DQUO, KC_SLASH,         KC_4,    KC_5,    KC_6,KC_KP_DOT,KC_UNDS, KC_BSLS,
  //|--------+--------+--------+--------+--------+--------|    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, KC_LCBR, KC_RCBR,   KC_LT,   KC_GT,         KC_1,    KC_2,    KC_3,KC_KP_EQUAL,KC_SLSH,KC_RSFT,
  //|--------+--------+--------+--------+--------+--------|    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, KC_HOME,  KC_END, KC_LALT, TL_LOWR,  KC_SPC,       KC_ENT, TL_UPPR, KC_RGUI, KC_PGDN, KC_PGUP, KC_RCTL,
  //| Joystick keys---------------------------------------|    |-----------------------------------------------------|
      KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT, KC_SPC,   KC_NO,         KC_NO,   KC_NO,   KC_NO,  KC_NO,   KC_NO,   KC_NO
  //`-----------------------------------------------------'    `-----------------------------------------------------'
  )
};
