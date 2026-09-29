#include "../include/keymap.h"
#include "../include/editor.h"
#include "../include/assert.h"
#include "tests.h"

void editorActionFromKeyTest() {
    initKeymaps();
    assertEquals(getEditorActionFromKey(EDITOR_NORMAL_MODE, "i"), ACTION_ENTER_INSERT_MODE, "Enter insert mode");
    assertEquals(getEditorActionFromKey(EDITOR_NORMAL_MODE, "dd"), ACTION_REMOVE_LINE, "Delete a line from normal mode");
    assertEquals(getEditorActionFromKey(EDITOR_NORMAL_MODE, "h"), ACTION_MOVE_CURSOR_LEFT, "Move the cursor left with h");

    assertEquals(getEditorActionFromKey(EDITOR_VISUAL_MODE, "\x1b"), ACTION_ENTER_NORMAL_MODE, "Enter normal mode from visual");

    assertEquals(getEditorActionFromKey(EDITOR_INSERT_MODE, "\x1b"), ACTION_ENTER_NORMAL_MODE, "Enter normal mode from insert");
    assertEquals(getEditorActionFromKey(EDITOR_INSERT_MODE, STRING(BACKSPACE)), ACTION_REMOVE_BACKSPACE, "Backspace in insert mode");

    // assertEquals(getEditorActionFromKey(EDITOR_COMMAND_MODE, "q"), ACTION_EDITOR_WINDOWS_EXIT, "Exit texture");

    allTestsPassing("Keymap");
}
           // case BACKSPACE:
           // case CTRL_KEY('h'):
           // case DEL_KEY:
           //     if(c == DEL_KEY){
           //         editorMoveCursor(ARROW_RIGHT, &E.editors[E.screenNumber]);
           //     }

void run_keymap_tests() {
    printTestingSegment("Keymap tests");
    editorActionFromKeyTest();
    return;
}
