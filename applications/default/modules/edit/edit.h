/* Nano-style editor
   Copyright 2025, L.C. Benschop, Eindhoven, The Netherlands.
   MIT license
*/

#include "common.h"

#define EDIT_BUF_SIZE (128*1024)
#define CUT_BUF_SIZE (16*1024)
#define MAX_NAME_LENGTH 40

/* Global editor state is only the _EditState structure and the
   emeory space (shared between applications).
*/

struct _EditState {
  unsigned char *mem_start; /* start of memory allocated to editior */
  unsigned char *text_start; /* Start of text being edited */
  unsigned char *top_line; /* Top line visible on screen */
  unsigned char *gap_start;
  unsigned char *gap_end;
  unsigned char *text_end;
  unsigned char *cut_end;  
  unsigned char *mem_end;
  unsigned int lineno; /* Line number at cursor (1-based) */
  unsigned int total_lines; /* Number of lines of text in buffer */
  unsigned int cut_lines;
};

extern struct _EditState EDT;

/* Memory space *
   mem_start <= addr < text_start: various byte-size and string vartables
                                   (see defines below).
   mem_start <= addr < gap_start:  text before cursor.
   gap_start <= addr < gap_end:    free space (gap).
   gap_end <= addr < text_end:     text at cursor and after (file always ends
                                   with newline that cursor cannot move past).
   text_end <= addr < cut_end      text in cut buffer.
   cut_end <= addr < mem_end       free space after cut buffer.
*/
   
#define FILENAME_OFFS 0
  /* 41 bytes 40-byte null-terminaed filename */
#define BACKFILENAME_OFFS (FILENAME_OFFS+MAX_NAME_LENGTH+1)
  /* 41 bytes 40-byte null-terminaed backup filename */
#define SEARCHSTRING_OFFS (BACKFILENAME_OFFS+MAX_NAME_LENGTH+1)
  /* 41 bytes 40-byte null-terminated search string */
#define LINENOSTRING_OFFS (SEARCHSTRING_OFFS+41)
  /* 5 bytes 4-byte type buffer for line number */
#define CRLF_FLAG_OFFS  (LINENOSTRING_OFFS + 5)
  /* flag to indicate that file should be saved with CR-LF instead of LR */
#define SCR_ROWS_OFFS (CRLF_FLAG_OFFS+1)
  /* Number of text rows on the screen (30 or 60) */
#define TAB_STOP_OFFS (SCR_ROWS_OFFS+1)
  /* Tab stops at 4 or 8 chars? */
#define CUR_LINE_LEN_OFFS (TAB_STOP_OFFS+1)
  /*  Lenght of current line in bytes */
#define CURSOR_ROW_OFFS (CUR_LINE_LEN_OFFS+1)
   /* Cursor row 0 is top of text area */
#define CURSOR_COL_OFFS (CURSOR_ROW_OFFS+1)
  /*  Cursor column (can be >= 80) for long lines. */
#define CURSOR_COL_MAX_OFFS (CURSOR_COL_OFFS+1)
  /* Cursor column to which we may return if we move to a longer line */
#define CURLINE_SCROLLED_OFFS (CURSOR_COL_MAX_OFFS+1)
  /* Number of characters scrolled to the left in current line */
#define CURLINE_POS_OFFS (CURLINE_SCROLLED_OFFS+1)
  /* Position within current line of cursor */
#define IS_CHANGED_OFFS  (CURLINE_POS_OFFS+1)
  /* Flag to indicate if file is changed */
#define VAR_END_OFFS (IS_CHANGED_OFFS+1)
  /* Offset where we can have the text_start pointer */

#define SCR_COLS 80 /* The number of columns is set fixed to 80 */

void EDT_EditCore(void);
unsigned int  EDT_GetKey(void);
void EDT_InitScreen(void);
void EDT_LoadFile(unsigned char *name);
bool EDT_SaveFile(unsigned char *name);
void EDT_InvVideo(void);
void EDT_TrueVideo(void);
void EDT_SetCursor(int x, int y);
void EDT_ClrEOL(void);
void EDT_ReadLine(unsigned char* buf, int len);
unsigned char * EDT_RenderLine(unsigned char *p, bool is_current);
void EDT_RenderCurrentLine(void);
void EDT_LeaveCurrentLine(void);
void EDT_ShowScreen(void);
void EDT_ShowCursor(void);
void EDT_ShowBottom(void);

void EDT_BufStartLine(void);
void EDT_BufEndLine(void);
int EDT_BufLenCurLine(void);
void EDT_BufInsertChar(unsigned char c);
void EDT_BufInsertNL(void);
void EDT_BufDeleteChar(void);
void EDT_BufJoinLines(void);
void EDT_BufNextLine(void);
void EDT_BufPrevLine(void);
void EDT_BufNextChar(void);
void EDT_BufPrevChar(void);
void EDT_BufAdjustCol(void);
void EDT_AdjustTop(bool always_redraw);
