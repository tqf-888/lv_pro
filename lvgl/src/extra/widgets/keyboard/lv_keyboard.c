
/**
 * @file lv_keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_keyboard.h"
#if LV_USE_KEYBOARD

#include "../../../widgets/lv_textarea.h"
#include "../../../misc/lv_assert.h"

#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS    &lv_keyboard_class
#define LV_KB_BTN(width) LV_BTNMATRIX_CTRL_POPOVER | width
#define LV_KEYBOARD_MODE_CNT       (LV_KEYBOARD_MODE_USER_4 + 1)
#define LV_KEYBOARD_LAYOUT_CNT     2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_keyboard_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);

static void lv_keyboard_update_map(lv_obj_t * obj);

static void lv_keyboard_update_ctrl_map(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_keyboard_class = {
    .constructor_cb = lv_keyboard_constructor,
    .width_def = LV_PCT(100),
    .height_def = LV_PCT(50),
    .instance_size = sizeof(lv_keyboard_t),
    .editable = 1,
    .base_class = &lv_btnmatrix_class
};

static const char * const qwerty_kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                 "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                 LV_SYMBOL_KEYBOARD, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_btnmatrix_ctrl_t qwerty_kb_ctrl_lc_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_BTNMATRIX_CTRL_CHECKED | 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_BTNMATRIX_CTRL_CHECKED | 7,
    LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_BTNMATRIX_CTRL_CHECKED | 2, 6, LV_BTNMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const qwerty_kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                                 "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                                 LV_SYMBOL_KEYBOARD, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_btnmatrix_ctrl_t qwerty_kb_ctrl_uc_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_KB_BTN(4), LV_BTNMATRIX_CTRL_CHECKED | 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_KB_BTN(3), LV_BTNMATRIX_CTRL_CHECKED | 7,
    LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_BTNMATRIX_CTRL_CHECKED | 2, 6, LV_BTNMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const qwerty_kb_map_spec[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                                   "abc", "&", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                                   "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                                   LV_SYMBOL_KEYBOARD, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                  };

static const lv_btnmatrix_ctrl_t qwerty_kb_ctrl_spec_map[] = {
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_BTNMATRIX_CTRL_CHECKED | 2,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1), LV_KB_BTN(1),
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_BTNMATRIX_CTRL_CHECKED | 2, 6, LV_BTNMATRIX_CTRL_CHECKED | 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const qwerty_kb_map_num[] = {"1", "2", "3", LV_SYMBOL_KEYBOARD, "\n",
                                                  "4", "5", "6", LV_SYMBOL_OK, "\n",
                                                  "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                                  "+/-", "0", ".", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""
                                                 };

static const lv_btnmatrix_ctrl_t qwerty_kb_ctrl_num_map[] = {
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, 2,
    1, 1, 1, 1, 1
};

/* Layout 2: simplified A-Z/0-9 keyboard. */
static const char * const az_kb_map[] = {
    "A", "B", "C", "D", "E", "F", "\n",
    "G", "H", "I", "J", "K", "L", "\n",
    "M", "N", "O", "P", "Q", "R", "\n",
    "S", "T", "U", "V", "W", "X", "\n",
    "Y", "Z", "0", "1", "2", "3", "\n",
    "4", "5", "6", "7", "8", "9", ""
};

static const lv_btnmatrix_ctrl_t az_kb_ctrl_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,

    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,

    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,

    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,

    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,

    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 1
};

static const char ** kb_map[LV_KEYBOARD_LAYOUT_CNT][LV_KEYBOARD_MODE_CNT] = {
    [LV_KEYBOARD_LAYOUT_QWERTY] = {
        [LV_KEYBOARD_MODE_TEXT_LOWER] = (const char **)qwerty_kb_map_lc,
        [LV_KEYBOARD_MODE_TEXT_UPPER] = (const char **)qwerty_kb_map_uc,
        [LV_KEYBOARD_MODE_SPECIAL]    = (const char **)qwerty_kb_map_spec,
        [LV_KEYBOARD_MODE_NUMBER]     = (const char **)qwerty_kb_map_num,
        [LV_KEYBOARD_MODE_USER_1]     = (const char **)qwerty_kb_map_lc,
        [LV_KEYBOARD_MODE_USER_2]     = (const char **)qwerty_kb_map_lc,
        [LV_KEYBOARD_MODE_USER_3]     = (const char **)qwerty_kb_map_lc,
        [LV_KEYBOARD_MODE_USER_4]     = (const char **)qwerty_kb_map_lc,
    },
    [LV_KEYBOARD_LAYOUT_AZ] = {
        [LV_KEYBOARD_MODE_TEXT_LOWER] = (const char **)az_kb_map,
        [LV_KEYBOARD_MODE_TEXT_UPPER] = (const char **)az_kb_map,
        [LV_KEYBOARD_MODE_SPECIAL]    = (const char **)az_kb_map,
        [LV_KEYBOARD_MODE_NUMBER]     = (const char **)az_kb_map,
        [LV_KEYBOARD_MODE_USER_1]     = (const char **)az_kb_map,
        [LV_KEYBOARD_MODE_USER_2]     = (const char **)az_kb_map,
        [LV_KEYBOARD_MODE_USER_3]     = (const char **)az_kb_map,
        [LV_KEYBOARD_MODE_USER_4]     = (const char **)az_kb_map,
    }
};
static const lv_btnmatrix_ctrl_t * kb_ctrl[LV_KEYBOARD_LAYOUT_CNT][LV_KEYBOARD_MODE_CNT] = {
    [LV_KEYBOARD_LAYOUT_QWERTY] = {
        [LV_KEYBOARD_MODE_TEXT_LOWER] = qwerty_kb_ctrl_lc_map,
        [LV_KEYBOARD_MODE_TEXT_UPPER] = qwerty_kb_ctrl_uc_map,
        [LV_KEYBOARD_MODE_SPECIAL]    = qwerty_kb_ctrl_spec_map,
        [LV_KEYBOARD_MODE_NUMBER]     = qwerty_kb_ctrl_num_map,
        [LV_KEYBOARD_MODE_USER_1]     = qwerty_kb_ctrl_lc_map,
        [LV_KEYBOARD_MODE_USER_2]     = qwerty_kb_ctrl_lc_map,
        [LV_KEYBOARD_MODE_USER_3]     = qwerty_kb_ctrl_lc_map,
        [LV_KEYBOARD_MODE_USER_4]     = qwerty_kb_ctrl_lc_map,
    },
    [LV_KEYBOARD_LAYOUT_AZ] = {
        [LV_KEYBOARD_MODE_TEXT_LOWER] = az_kb_ctrl_map,
        [LV_KEYBOARD_MODE_TEXT_UPPER] = az_kb_ctrl_map,
        [LV_KEYBOARD_MODE_SPECIAL]    = az_kb_ctrl_map,
        [LV_KEYBOARD_MODE_NUMBER]     = az_kb_ctrl_map,
        [LV_KEYBOARD_MODE_USER_1]     = az_kb_ctrl_map,
        [LV_KEYBOARD_MODE_USER_2]     = az_kb_ctrl_map,
        [LV_KEYBOARD_MODE_USER_3]     = az_kb_ctrl_map,
        [LV_KEYBOARD_MODE_USER_4]     = az_kb_ctrl_map,
    }
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a Keyboard object
 * @param parent pointer to an object, it will be the parent of the new keyboard
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_keyboard_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(&lv_keyboard_class, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_keyboard_set_textarea(lv_obj_t * obj, lv_obj_t * ta)
{
    if(ta) {
        LV_ASSERT_OBJ(ta, &lv_textarea_class);
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;

    /*Hide the cursor of the old Text area if cursor management is enabled*/
    if(keyboard->ta) {
        lv_obj_clear_state(obj, LV_STATE_FOCUSED);
    }

    keyboard->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(keyboard->ta) {
        lv_obj_add_flag(obj, LV_STATE_FOCUSED);
    }
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_keyboard_mode_t'
 */
void lv_keyboard_set_mode(lv_obj_t * obj, lv_keyboard_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    if(mode >= LV_KEYBOARD_MODE_CNT) return;
    if(keyboard->mode == mode) return;

    keyboard->mode = mode;
    lv_keyboard_update_map(obj);
}

void lv_keyboard_set_layout(lv_obj_t * obj, lv_keyboard_layout_t layout)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    if(layout >= LV_KEYBOARD_LAYOUT_CNT) return;

    keyboard->layout = layout;
    /* Force a visible and deterministic switch. Otherwise switching while in NUMBER/SPECIAL
     * can look unchanged because some modes share similar maps. */
    keyboard->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
    lv_keyboard_update_map(obj);
}

void lv_keyboard_set_layout_mode(lv_obj_t * obj, lv_keyboard_layout_t layout, lv_keyboard_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    if(layout >= LV_KEYBOARD_LAYOUT_CNT) return;
    if(mode >= LV_KEYBOARD_MODE_CNT) return;

    keyboard->layout = layout;
    keyboard->mode = mode;
    lv_keyboard_update_map(obj);
}

/**
 * Show the button title in a popover when pressed.
 * @param kb pointer to a Keyboard object
 * @param en whether "popovers" mode is enabled
 */
void lv_keyboard_set_popovers(lv_obj_t * obj, bool en)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;

    if(keyboard->popovers == en) {
        return;
    }

    keyboard->popovers = en;
    lv_keyboard_update_ctrl_map(obj);
}

/**
 * Set a new map for the keyboard
 * @param kb pointer to a Keyboard object
 * @param mode keyboard map to alter 'lv_keyboard_mode_t'
 * @param map pointer to a string array to describe the map.
 *            See 'lv_btnmatrix_set_map()' for more info.
 */
void lv_keyboard_set_map(lv_obj_t * obj, lv_keyboard_mode_t mode, const char * map[],
                         const lv_btnmatrix_ctrl_t ctrl_map[])
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    if(mode >= LV_KEYBOARD_MODE_CNT) return;
    if(keyboard->layout >= LV_KEYBOARD_LAYOUT_CNT) keyboard->layout = LV_KEYBOARD_LAYOUT_DEFAULT;

    kb_map[keyboard->layout][mode] = map;
    kb_ctrl[keyboard->layout][mode] = ctrl_map;
    lv_keyboard_update_map(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    return keyboard->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_keyboard_mode_t'
 */
lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    return keyboard->mode;
}

lv_keyboard_layout_t lv_keyboard_get_layout(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    return keyboard->layout;
}


/**
 * Tell whether "popovers" mode is enabled or not.
 * @param kb pointer to a Keyboard object
 * @return true: "popovers" mode is enabled; false: disabled
 */
bool lv_btnmatrix_get_popovers(const lv_obj_t * obj)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    return keyboard->popovers;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function can be called from it to handle the
 * button clicks
 * @param kb pointer to a keyboard
 * @param event the triggering event
 */
void lv_keyboard_def_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    uint16_t btn_id   = lv_btnmatrix_get_selected_btn(obj);
    if(btn_id == LV_BTNMATRIX_BTN_NONE) return;

    const char * txt = lv_btnmatrix_get_btn_text(obj, lv_btnmatrix_get_selected_btn(obj));
    if(txt == NULL) return;

    if(strcmp(txt, "abc") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
        lv_keyboard_update_map(obj);
        return;
    }
    else if(strcmp(txt, "ABC") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_TEXT_UPPER;
        lv_keyboard_update_map(obj);
        return;
    }
    else if(strcmp(txt, "1#") == 0) {
        keyboard->mode = LV_KEYBOARD_MODE_SPECIAL;
        lv_keyboard_update_map(obj);
        return;
    }
    else if(strcmp(txt, LV_SYMBOL_CLOSE) == 0 || strcmp(txt, LV_SYMBOL_KEYBOARD) == 0) {
        lv_res_t res = lv_event_send(obj, LV_EVENT_CANCEL, NULL);
        if(res != LV_RES_OK) return;

        if(keyboard->ta) {
            res = lv_event_send(keyboard->ta, LV_EVENT_CANCEL, NULL);
            if(res != LV_RES_OK) return;
        }
        return;
    }
    else if(strcmp(txt, LV_SYMBOL_OK) == 0) {
        lv_res_t res = lv_event_send(obj, LV_EVENT_READY, NULL);
        if(res != LV_RES_OK) return;

        if(keyboard->ta) {
            res = lv_event_send(keyboard->ta, LV_EVENT_READY, NULL);
            if(res != LV_RES_OK) return;
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(keyboard->ta == NULL) return;

    if(strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) {
        lv_textarea_add_char(keyboard->ta, '\n');
        if(lv_textarea_get_one_line(keyboard->ta)) {
            lv_res_t res = lv_event_send(keyboard->ta, LV_EVENT_READY, NULL);
            if(res != LV_RES_OK) return;
        }
    }
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0) {
        lv_textarea_cursor_left(keyboard->ta);
    }
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0) {
        lv_textarea_cursor_right(keyboard->ta);
    }
    else if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
        lv_textarea_del_char(keyboard->ta);
    }
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur        = lv_textarea_get_cursor_pos(keyboard->ta);
        const char * ta_txt = lv_textarea_get_text(keyboard->ta);
        if(ta_txt[0] == '-') {
            lv_textarea_set_cursor_pos(keyboard->ta, 1);
            lv_textarea_del_char(keyboard->ta);
            lv_textarea_add_char(keyboard->ta, '+');
            lv_textarea_set_cursor_pos(keyboard->ta, cur);
        }
        else if(ta_txt[0] == '+') {
            lv_textarea_set_cursor_pos(keyboard->ta, 1);
            lv_textarea_del_char(keyboard->ta);
            lv_textarea_add_char(keyboard->ta, '-');
            lv_textarea_set_cursor_pos(keyboard->ta, cur);
        }
        else {
            lv_textarea_set_cursor_pos(keyboard->ta, 0);
            lv_textarea_add_char(keyboard->ta, '-');
            lv_textarea_set_cursor_pos(keyboard->ta, cur + 1);
        }
    }
    else {
        lv_textarea_add_text(keyboard->ta, txt);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_keyboard_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE);

    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    keyboard->ta         = NULL;
    keyboard->mode       = LV_KEYBOARD_MODE_TEXT_LOWER;
    keyboard->layout     = LV_KEYBOARD_LAYOUT_DEFAULT;
    keyboard->popovers   = 0;

    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(obj, lv_keyboard_def_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_style_base_dir(obj, LV_BASE_DIR_LTR, 0);

    lv_keyboard_update_map(obj);
}

/**
 * Update the key and control map for the current mode
 * @param obj pointer to a keyboard object
 */
static void lv_keyboard_update_map(lv_obj_t * obj)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    if(keyboard->layout >= LV_KEYBOARD_LAYOUT_CNT) keyboard->layout = LV_KEYBOARD_LAYOUT_DEFAULT;
    if(keyboard->mode >= LV_KEYBOARD_MODE_CNT) keyboard->mode = LV_KEYBOARD_MODE_TEXT_LOWER;

    const char ** cur_map = kb_map[keyboard->layout][keyboard->mode];
    if(cur_map == NULL) cur_map = kb_map[LV_KEYBOARD_LAYOUT_DEFAULT][LV_KEYBOARD_MODE_TEXT_LOWER];

    lv_btnmatrix_set_map(obj, cur_map);
    lv_keyboard_update_ctrl_map(obj);
}

/**
 * Update the control map for the current mode
 * @param obj pointer to a keyboard object
 */
static void lv_keyboard_update_ctrl_map(lv_obj_t * obj)
{
    lv_keyboard_t * keyboard = (lv_keyboard_t *)obj;
    if(keyboard->layout >= LV_KEYBOARD_LAYOUT_CNT) keyboard->layout = LV_KEYBOARD_LAYOUT_DEFAULT;
    if(keyboard->mode >= LV_KEYBOARD_MODE_CNT) keyboard->mode = LV_KEYBOARD_MODE_TEXT_LOWER;

    const lv_btnmatrix_ctrl_t * cur_ctrl = kb_ctrl[keyboard->layout][keyboard->mode];
    if(cur_ctrl == NULL) cur_ctrl = kb_ctrl[LV_KEYBOARD_LAYOUT_DEFAULT][LV_KEYBOARD_MODE_TEXT_LOWER];
    if(cur_ctrl == NULL) return;

    if(keyboard->popovers) {
        /*Apply the current control map (already includes LV_BTNMATRIX_CTRL_POPOVER flags)*/
        lv_btnmatrix_set_ctrl_map(obj, cur_ctrl);
    }
    else {
        /*Make a copy of the current control map*/
        lv_btnmatrix_t * btnm = (lv_btnmatrix_t *)obj;
        lv_btnmatrix_ctrl_t * ctrl_map = lv_mem_alloc(btnm->btn_cnt * sizeof(lv_btnmatrix_ctrl_t));
        if(ctrl_map == NULL) return;
        lv_memcpy(ctrl_map, cur_ctrl, sizeof(lv_btnmatrix_ctrl_t) * btnm->btn_cnt);

        /*Remove all LV_BTNMATRIX_CTRL_POPOVER flags*/
        for(uint16_t i = 0; i < btnm->btn_cnt; i++) {
            ctrl_map[i] &= (~LV_BTNMATRIX_CTRL_POPOVER);
        }

        /*Apply new control map and clean up*/
        lv_btnmatrix_set_ctrl_map(obj, ctrl_map);
        lv_mem_free(ctrl_map);
    }
}

#endif  /*LV_USE_KEYBOARD*/
