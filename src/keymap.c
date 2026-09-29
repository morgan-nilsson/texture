#include <stdlib.h>
#include <string.h>
#include "../include/keymap.h"
#include "../include/editor.h"
#include "../include/vector.h"
#include "../include/utils.h"

typedef struct {
    const char* key;
    EditorAction action;
    const char* desc;
} Map;

typedef struct {
    int type;
    union {
        EditorAction action;
        char seq[20];
    } value;
} KeyAction;

Map normal_keymap[] = {
    {":", ACTION_GET_INPUT, "Get for input"},
    {"i", ACTION_ENTER_INSERT_MODE, "Enter insert mode"},
    {"v", ACTION_ENTER_VISUAL_MODE, "Enter visual mode"},
    {STRING(ARROW_RIGHT), ACTION_MOVE_CURSOR_RIGHT, "Move cursor right"},
    {STRING(ARROW_LEFT), ACTION_MOVE_CURSOR_LEFT, "Move cursor left"},
    {STRING(ARROW_UP), ACTION_MOVE_CURSOR_UP, "Move cursor up"},
    {STRING(ARROW_DOWN), ACTION_MOVE_CURSOR_DOWN, "Move cursor down"},
    {STRING(CTRL_KEY('s')), ACTION_FS_SAVE_FILE, "Save the current buffer"},
    {"d", ACTION_WAIT, "Wait for next enter"},
    {"dd", ACTION_REMOVE_LINE, "Delete the current line"},
    {"l", ACTION_MOVE_CURSOR_RIGHT, "Move cursor right"},
    {"h", ACTION_MOVE_CURSOR_LEFT, "Move cursor left"},
    {"k", ACTION_MOVE_CURSOR_UP, "Move cursor up"},
    {"j", ACTION_MOVE_CURSOR_DOWN, "Move cursor down"},
};
#define NORMAL_LENGTH sizeof(normal_keymap) / sizeof(normal_keymap[0])

Map visual_keymap[] = {
    {"\x1b", ACTION_ENTER_NORMAL_MODE, "Exit visual mode"}
};
#define VISUAL_LENGTH sizeof(visual_keymap) / sizeof(visual_keymap[0])

Map insert_keymap[] = {
    {"\x1b", ACTION_ENTER_NORMAL_MODE, "Exit insert mode"},
    {"\r", ACTION_INSERT_NEWLINE, "Insert Newline"},
    {STRING(HOME_KEY), ACTION_MOVE_HOME_KEY, "Move with the home key"},
    {STRING(END_KEY), ACTION_MOVE_END_KEY, "Move with the end key"},
    {STRING(BACKSPACE), ACTION_REMOVE_BACKSPACE, "Backspace"},
    {STRING(CTRL_KEY('h')), ACTION_REMOVE_BACKSPACE, "Backspace"},
    {STRING(DEL_KEY), ACTION_REMOVE_DEL_KEY, "Del"},
    {STRING(ARROW_RIGHT), ACTION_MOVE_CURSOR_RIGHT, "Move cursor right"},
    {STRING(ARROW_LEFT), ACTION_MOVE_CURSOR_LEFT, "Move cursor left"},
    {STRING(ARROW_UP), ACTION_MOVE_CURSOR_UP, "Move cursor up"},
    {STRING(ARROW_DOWN), ACTION_MOVE_CURSOR_DOWN, "Move cursor down"},
};
#define INSERT_LENGTH sizeof(insert_keymap) / sizeof(insert_keymap[0])

Map command_keymap[] = {
    {"e", ACTION_EDITOR_WINDOWS_EXIT, "Exit current window"},
    {"w", ACTION_FS_SAVE_FILE, "Save the current buffer"},
    {"q", ACTION_EXIT_EDITOR, "Exit the editor"}
};
#define COMMAND_LENGTH sizeof(command_keymap) / sizeof(command_keymap[0])

struct vector normal_mode_keymap_vector;
struct vector insert_mode_keymap_vector;
struct vector visual_mode_keymap_vector;
struct vector command_mode_keymap_vector;

struct vector* currKeymap_vector;

static void loadKeymap(struct vector* vec, Map* keymap, size_t length) {
    // free first so calling initKeymaps again doesn't leak
    vector_free(vec);
    if (vector_init_size(vec, sizeof(Map), length) != 0) {
        terminate("keymap vector init failed");
    }
    for (size_t i = 0; i < length; i++) {
        vector_push(vec, &keymap[i]);
    }
}

void initKeymaps() {
    loadKeymap(&normal_mode_keymap_vector, normal_keymap, NORMAL_LENGTH);
    loadKeymap(&insert_mode_keymap_vector, insert_keymap, INSERT_LENGTH);
    loadKeymap(&visual_mode_keymap_vector, visual_keymap, VISUAL_LENGTH);
    loadKeymap(&command_mode_keymap_vector, command_keymap, COMMAND_LENGTH);
}

EditorAction getEditorActionFromKey(EditorMode mode, const char* key) {
    switch (mode) {
        case EDITOR_NORMAL_MODE:
            currKeymap_vector = &normal_mode_keymap_vector;
            break;
        case EDITOR_INSERT_MODE:
            currKeymap_vector = &insert_mode_keymap_vector;
            break;
        case EDITOR_VISUAL_MODE:
            currKeymap_vector = &visual_mode_keymap_vector;
            break;
        case EDITOR_COMMAND_MODE:
            currKeymap_vector = &command_mode_keymap_vector;
            break;
    }

    for (size_t i = 0; i < currKeymap_vector->num_elements; i++) {
        Map* map = (Map *)vector_get_unchecked(currKeymap_vector, i);
        if (strcmp(key, map->key) == 0) return map->action;
        if (memcmp(key, map->key, min(strlen(key), strlen(map->key))) == 0) return map->action;
    }

    return ACTION_UNKOWN;
}
