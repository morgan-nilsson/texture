#include "../include/editor.h"

#ifndef KEYMAP_H
#define KEYMAP_H

// Keymap strings hold the raw bytes of the key press.
// Special keys (enum editorKey >= ARROW_LEFT) don't fit in a byte, so they are encoded
// as KEY_SPECIAL_PREFIX followed by (key - ARROW_LEFT + 1). 0xFF never appears in UTF-8.
#define KEY_SPECIAL_PREFIX '\xff'
#define KEY_ARROW_LEFT  "\xff\x01"
#define KEY_ARROW_RIGHT "\xff\x02"
#define KEY_ARROW_UP    "\xff\x03"
#define KEY_ARROW_DOWN  "\xff\x04"
#define KEY_DEL         "\xff\x05"
#define KEY_HOME        "\xff\x06"
#define KEY_END         "\xff\x07"
#define KEY_PAGE_UP     "\xff\x08"
#define KEY_PAGE_DOWN   "\xff\x09"
#define KEY_BACKSPACE   "\x7f"
#define KEY_CTRL_H      "\x08"
#define KEY_CTRL_S      "\x13"

typedef enum {
	ACTION_UNKOWN,
	ACTION_IGNORE,
	ACTION_DISCARD,
	ACTION_WAIT,
	ACTION_ENTER_INSERT_MODE,
	ACTION_ENTER_VISUAL_MODE,
	ACTION_ENTER_NORMAL_MODE,
	ACTION_MOVE_CURSOR_RIGHT,
	ACTION_MOVE_CURSOR_LEFT,
	ACTION_MOVE_CURSOR_UP,
	ACTION_MOVE_CURSOR_DOWN,
	ACTION_FS_SAVE_FILE,
	ACTION_FS_OPEN_FILE,
	ACTION_EDITOR_WINDOWS_CYCLE_FORWARD,
	ACTION_EDITOR_WINDOWS_CYCLE_BACKWARD,
	ACTION_EDITOR_WINDOWS_EXIT,
	ACTION_EXIT_EDITOR,
	ACTION_EXECUTE_DIR,
	
	ACTION_INSERT_NEWLINE,

	ACTION_REMOVE_BACKSPACE,
	ACTION_REMOVE_DEL_KEY,
	ACTION_REMOVE_LINE,

	ACTION_MOVE_HOME_KEY,
	ACTION_MOVE_END_KEY,

	ACTION_GET_INPUT,
} EditorAction;

#define CTRL_KEY(key) ((key) & 0x1f)

EditorAction getEditorActionFromKey(EditorMode mode, const char* action);
void initKeymaps(void);
int myStrncmp(const char*, const char*);

#endif
