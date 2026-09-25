#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/highlight.h"
#include "../include/utils.h"

int editorRowCxToRx(EditorRow *row, int cx){
    int rx = 0;
    int j;
    for(j = 0; j < cx; j++){
        if (row->chars[j] == '\t'){
            rx += (TEXTURE_TAB_STOP - 1) - (rx % TEXTURE_TAB_STOP);
        }
        rx++;
    }
    return rx;
}

int editorRowRxToCx(EditorRow *row, int rx){
    int cur_rx = 0;
    int cx;
    for(cx = 0; cx < row->size; cx++){
        if (row->chars[cx] == '\t'){
            cur_rx += (TEXTURE_TAB_STOP - 1) - (cur_rx % TEXTURE_TAB_STOP);
        }
        cur_rx++;
        if(cur_rx > rx){
            return cx;
        }
    }
    return cx;
}

void editorUpdateRow(struct Editor* E, EditorRow *row){
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++){
        if (row->chars[j] == '\t'){
            tabs++;
        }
    }
    free(row->render);
    row->render = (char *)malloc(row->size + ( tabs * (TEXTURE_TAB_STOP - 1)) + 1);

    int tempLength = 0;
    for (j = 0; j < row->size; j++){
        if (row->chars[j] == '\t'){
            row->render[tempLength++] = ' ';
            while (tempLength % TEXTURE_TAB_STOP != 0){
                row->render[tempLength++] = ' ';
            }
        } else{
            row->render[tempLength++] = row->chars[j];
        }
    }
    row->render[tempLength] = '\0';
    row->renderSize = tempLength;

    editorUpdateSyntax(E, row);
}

void editorFreeRow(EditorRow *row){
    free(row->render);
    free(row->chars);
    free(row->highLight);
}

void editorInsertRow(struct Editor* E, int at, char* s, size_t length, struct EditorBuffer* buf){
    if(at < 0 || at > buf->displayLength){
        return;
    }

    buf->row = (EditorRow *)realloc(buf->row, sizeof(EditorRow) * (buf->displayLength + 1));
    //memmove(&E.editors[E.screenNumber].row[at + 1], &E.editors[E.screenNumber].row[at], sizeof(EditorRow) * (E.editors[E.screenNumber].displayLength - at));
    memmove(&buf->row[at + 1], &buf->row[at], sizeof(EditorRow) * (buf->displayLength - at));

    // add a row to display
    buf->row[at].size = length;
    buf->row[at].chars = (char *)malloc(length + 1);
    memcpy(buf->row[at].chars, s, length);
    buf->row[at].chars[length] = '\0';

    buf->row[at].renderSize = 0;
    buf->row[at].render = NULL;
    buf->row[at].highLight = NULL;
    editorUpdateRow(E, &buf->row[at]);

    buf->displayLength++;
    buf->dirty++;
}

void editorMoveCursor(int key, struct EditorBuffer* buf){
    struct EditorBuffer e = *buf;
    EditorRow* row = (e.cy >= e.displayLength) ? NULL : &e.row[e.cy];
    
    // update the cursor position based on the key inputs
    switch (key)
    {
        case ARROW_LEFT:
            if (e.cx != 0){
                e.cx--;
            } else if(e.cy > 0){
                e.cy--;
                e.cx = e.row[e.cy].size;
            }
            break;
        case ARROW_RIGHT:
            if (row && e.cx < row->size){
                e.cx++;
            // if go right on a the end of line
            } else if(row && e.cx == row->size){
                e.cy++;
                e.cx = 0;
            }
            break;
        case ARROW_UP:
            if (e.cy != 0){
                e.cy--;
            }
            break;
        case ARROW_DOWN:
            if (e.cy < e.displayLength){
                e.cy++;
            }
            break;
    }

    // snap cursor to the end of line
    row = (e.cy >= e.displayLength) ? NULL : &e.row[e.cy];
    int rowLength = row ? row->size : 0;
    if (e.cx > rowLength){
        e.cx = rowLength;
    }
}

void editorDeleteRow(struct Editor* E, int at, struct EditorBuffer* buf){
    if(at < 0 || at >= buf->displayLength){
        return;
    }
    editorFreeRow(&buf->row[at]);
    memmove(&buf->row[at], &buf->row[at + 1], sizeof(EditorRow) * (buf->displayLength - at - 1));
    buf->displayLength--;
    buf->dirty++;
}

void editorRowInsertChar(struct Editor* E, EditorRow *row, int at, int c, int* dirty){
    if (at < 0 || at > row->size){ 
        at = row->size;
    }
    row->chars = (char *)realloc(row->chars, row->size + 2);
        memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
        row->size++;
        row->chars[at] = c;
        editorUpdateRow(E, row);
        (*dirty)++;
}

void editorInsertChar(struct Editor* E, int c, struct EditorBuffer* buf){
    if (buf->cy == buf->displayLength){
        editorInsertRow(E, buf->displayLength, "", 0, buf);
    }
    editorRowInsertChar(E, &buf->row[buf->cy], buf->cx, c, &buf->dirty);
    buf->cx++;
}

void editorRowDeleteChar(struct Editor* E, EditorRow *row, int at, int* dirty){
    if (at < 0 || at >= row->size){
        return;
    }
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editorUpdateRow(E, row);
    (*dirty)++;
}

void editorInsertNewLine(struct Editor* E, struct EditorBuffer* buf){
    if(buf->cx == 0){
        editorInsertRow(E, buf->cy, "", 0, buf);
    } else{
        EditorRow *row = &buf->row[buf->cy];
        editorInsertRow(E, buf->cy + 1, &row->chars[buf->cx], row->size - buf->cx, buf);
        row = &buf->row[buf->cy];
        row->size = buf->cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(E, row);
    }
    buf->cy++;
    buf->cx = 0;
}

void editorRowAppendString(struct Editor* E, EditorRow *row, char *s, size_t length, int* dirty){
    row->chars = (char *)realloc(row->chars, row->size + length + 1);
    memcpy(&row->chars[row->size], s, length);
    row->size += length;
    row->chars[row->size] = '\0';
    editorUpdateRow(E, row);
    (*dirty)++;
}

void initBuffer(struct Editor* E, int screen) {
    // cursor positions
    E->editors[screen].cx = 0;
    E->editors[screen].cy = 0;
    E->editors[screen].rx = 0;
    E->editors[screen].mode = EDITOR_NORMAL_MODE;
    E->editors[screen].rowOffset = 0;
    E->editors[screen].columnOffset = 0;
    E->editors[screen].displayLength = 0;
    E->editors[screen].dirty = 0;
    E->editors[screen].row = NULL;
    E->editors[screen].fileName = NULL;
    E->editors[screen].statusMessage[0] = '\0';
    E->editors[screen].actionBuffer[0] = '\0';
    E->editors[screen].statusMessage_time = 0;
    E->editors[screen].syntax = NULL;

    if (getWindowSize(&E->editors[screen].screenRows, &E->editors[screen].screenColumns) == -1){
        terminate("getWindowSize");
    }
    E->editors[screen].screenRows = E->editors[screen].screenRows - 2;
}

void initEditor(struct Editor* E) {
    E->logger = initLogger("build/out.log");
    for (int i = SCREEN_MIN; i <= SCREEN_MAX; i++){
        initBuffer(E, i);
    }
    E->screenNumber = SCREEN_MIN;
}

char* convertModeToString(struct Editor* E){
    switch (E->editors[E->screenNumber].mode){
        case EDITOR_NORMAL_MODE: return "normal";
        case EDITOR_INSERT_MODE: return "insert";
        case EDITOR_VISUAL_MODE: return "visual";
        case EDITOR_COMMAND_MODE: return "command";
        default: return "";
    }
}

void editorScroll(struct Editor* E){
    // moving the screen around the file
    int screenNumber = E->screenNumber;
    if (E->editors[screenNumber].cy < E->editors[screenNumber].displayLength){
        E->editors[screenNumber].rx = editorRowCxToRx(&E->editors[screenNumber].row[E->editors[screenNumber].cy], E->editors[screenNumber].cx);
    }

    if (E->editors[screenNumber].cy < E->editors[screenNumber].rowOffset){
        E->editors[screenNumber].rowOffset = E->editors[screenNumber].cy;
    }
    if (E->editors[screenNumber].cy >= E->editors[screenNumber].rowOffset + E->editors[screenNumber].screenRows){
        E->editors[screenNumber].rowOffset = E->editors[screenNumber].cy - E->editors[screenNumber].screenRows + 1;
    }
    if (E->editors[screenNumber].rx < E->editors[screenNumber].columnOffset){
        E->editors[screenNumber].columnOffset = E->editors[screenNumber].rx;
    }
    if (E->editors[screenNumber].rx >= E->editors[screenNumber].columnOffset + E->editors[screenNumber].screenColumns){
        E->editors[screenNumber].columnOffset = E->editors[screenNumber].rx - E->editors[screenNumber].screenColumns + 1;
    }
}
