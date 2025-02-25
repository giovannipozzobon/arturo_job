/* Nano-style editor
   Copyright 2025, L.C. Benschop, Eindhoven, The Netherlands.
   MIT license
*/

#include "common.h"
#include "edit.h"


static void EDT_StartLine(void)
{
  EDT_BufStartLine();
  EDT.mem_start[CURLINE_POS_OFFS]=0;
  EDT.mem_start[CURSOR_COL_MAX_OFFS]=0;
  EDT_RenderCurrentLine();
}

static void EDT_EndLine(void)
{
  EDT_BufEndLine();
  EDT.mem_start[CURLINE_POS_OFFS]=EDT.mem_start[CUR_LINE_LEN_OFFS];
  EDT.mem_start[CURSOR_COL_MAX_OFFS]=0;
  EDT_RenderCurrentLine();
}

static void EDT_CursorLeft(void)
{
  EDT.mem_start[CURSOR_COL_MAX_OFFS]=0;
  if(EDT.mem_start[CURLINE_POS_OFFS] > 0) {
    EDT_BufPrevChar();
    EDT.mem_start[CURLINE_POS_OFFS]--;
    EDT_RenderCurrentLine();
  } else if (EDT.lineno > 1) {
    EDT.lineno--;
    EDT_LeaveCurrentLine();
    EDT.mem_start[CURSOR_ROW_OFFS]--;
    EDT_BufPrevLine();
    EDT_BufAdjustCol();
    EDT_BufEndLine();
    EDT.mem_start[CURLINE_POS_OFFS]=EDT.mem_start[CUR_LINE_LEN_OFFS];
    EDT_AdjustTop(false);
  }    
}

static void EDT_CursorRight(void)
{
  EDT.mem_start[CURSOR_COL_MAX_OFFS]=0;
  if(EDT.mem_start[CURLINE_POS_OFFS] < EDT.mem_start[CUR_LINE_LEN_OFFS]) {
    EDT_BufNextChar();
    EDT.mem_start[CURLINE_POS_OFFS]++;
    EDT_RenderCurrentLine();
  } else if (EDT.lineno < EDT.total_lines) {
    EDT.lineno++;
    EDT_LeaveCurrentLine();
    EDT.mem_start[CURSOR_ROW_OFFS]++;
    EDT_BufNextLine();
    EDT_BufAdjustCol();
    EDT_BufStartLine();
    EDT.mem_start[CURLINE_POS_OFFS]=0;
    EDT_AdjustTop(false);    
  }
}

static void EDT_CursorUp(void)
{
  if (EDT.lineno > 1) {
    EDT.lineno--;
    EDT_LeaveCurrentLine();
    EDT.mem_start[CURSOR_ROW_OFFS]--;
    EDT_BufPrevLine();
    EDT_BufAdjustCol();
    EDT_AdjustTop(false);
  }    
}

static void EDT_CursorDown(void)
{
  if (EDT.lineno < EDT.total_lines) {
    EDT.lineno++;
    EDT_LeaveCurrentLine();
    EDT.mem_start[CURSOR_ROW_OFFS]++;
    EDT_BufNextLine();
    EDT_BufAdjustCol();
    EDT_AdjustTop(false);
  }    
}

static void EDT_Delete(void)
{
  if(EDT.mem_start[CURLINE_POS_OFFS] < EDT.mem_start[CUR_LINE_LEN_OFFS]) {
    EDT_BufDeleteChar();
    EDT_RenderCurrentLine();
  } else {
    EDT_BufJoinLines();
    EDT_AdjustTop(true);
  }
}

static void EDT_BackSpace(void)
{
  EDT.mem_start[CURSOR_COL_MAX_OFFS]=0;
  if(EDT.mem_start[CURLINE_POS_OFFS]>0) {
    EDT.mem_start[CURLINE_POS_OFFS]--;
    EDT_BufPrevChar();
    EDT_BufDeleteChar();
    EDT_RenderCurrentLine();
  } else if (EDT.gap_start > EDT.text_start) {
    EDT.lineno--;
    EDT.mem_start[CURSOR_ROW_OFFS]--;
    EDT_BufPrevLine();
    EDT_BufAdjustCol();
    EDT_BufEndLine();
    EDT.mem_start[CURLINE_POS_OFFS]=EDT.mem_start[CUR_LINE_LEN_OFFS];
    EDT_BufJoinLines();
    EDT_AdjustTop(true);    
  }
}
static void EDT_NewLine(void)
{
  if (EDT.gap_start < EDT.gap_end-1) {
    EDT.mem_start[CURSOR_ROW_OFFS]++;
    EDT_BufInsertNL();
    EDT_BufAdjustCol();
    EDT_BufStartLine();
    EDT.mem_start[CURLINE_POS_OFFS]=0;
    EDT_AdjustTop(true);
  }
}

static void EDT_InsertChar(int c)
{
  EDT_BufInsertChar(c);
  EDT_RenderCurrentLine();
}
static void EDT_CutLine(bool FContinue)
{
}
static void EDT_CopyLine(bool FContinue)
{
}
static void EDT_Paste(void)
{
}

static void EDT_CenterScreen(void)
{
    EDT.mem_start[CURSOR_ROW_OFFS]=127;
    EDT_AdjustTop(false);
}

static bool EDT_AskSave(void)
{
  VDUWriteString("Save file: ");
  EDT_ReadLine(EDT.mem_start+FILENAME_OFFS,MAX_NAME_LENGTH);  
  VDUWriteString("\r\n");
  return EDT_SaveFile(EDT.mem_start+FILENAME_OFFS);
}
static void EDT_GotoLine(void)
{
  int i,n;
  EDT.mem_start[LINENOSTRING_OFFS]=0;
  EDT_SetCursor(0,EDT.mem_start[SCR_ROWS_OFFS]-1);
  EDT_ClrEOL();
  EDT_SetCursor(0,EDT.mem_start[SCR_ROWS_OFFS]-1);
  VDUWriteString("Goto line: ");
  EDT_ReadLine(EDT.mem_start+LINENOSTRING_OFFS,4);
  n=atoi((char*)EDT.mem_start+LINENOSTRING_OFFS);
  if (n<1) n=1;
  if (n>EDT.total_lines) n=EDT.total_lines;
  if (n<EDT.lineno) {
    for (i=0; i<EDT.lineno-n; i++) {
      EDT_BufPrevLine();
    }
    EDT_BufAdjustCol();
  } else if (n>EDT.lineno) {
    for (i=0; i<n-EDT.lineno; i++) {
      EDT_BufNextLine();
    }
    EDT_BufAdjustCol();
  }
  EDT.lineno=n;  
  EDT.mem_start[CURSOR_ROW_OFFS]=127;
  EDT_AdjustTop(false);  
}
static void EDT_PageUp(void)
{
  int i;
  int n=EDT.mem_start[SCR_ROWS_OFFS] -2;
  EDT_LeaveCurrentLine();
  for (i=0; i<n;i++) {
    if (EDT.lineno==1) break;
    EDT.lineno--;
    EDT.mem_start[CURSOR_ROW_OFFS]--;
    EDT_BufPrevLine();
  }
  if (i>0)EDT_BufAdjustCol();
  EDT_AdjustTop(false);
}

static void EDT_PageDown(void)
{
  int i;
  int n=EDT.mem_start[SCR_ROWS_OFFS] -2;
  EDT_LeaveCurrentLine();
  for (i=0; i<n;i++) {
    if (EDT.lineno ==EDT.total_lines) break;
    EDT.lineno++;
    EDT.mem_start[CURSOR_ROW_OFFS]++;
    EDT_BufNextLine();
  }
  if (i>0)EDT_BufAdjustCol();
  EDT_AdjustTop(false);
}
static void EDT_SearchF(void)
{
}
static void EDT_SearchB(void)
{
}
static void EDT_ReadFile(void)
{
}
static void EDT_InsertHex(void)
{
}


static char HelpText[] = 
  "Text editor for Arturo v0.01, Copyright 2025, L.C. Benschop\r\n"
  "\r\n"
  "Cursor movement:\r\n"	
  "Ctrl-B or cursor left, Ctrl-F or cursor right\r\n"
  "Ctrl-P or cursor up, Ctrl-N or cursor down\r\n"
  "Ctrl-Y: page up, Ctrl-V: page down\r\n"
  "Ctrl-A; start of line, Ctrl-E: end of line\r\n"
  "Ctrl-L, redraw screen with current line in centre\r\n"
  "Ctrl-H goto line (enter number)\r\n"
  "\r\n"
  "Delete:\r\n"
  "Backspace: delete to left, Control-D: delete to right\r\n"
  "\r\n"
  "Cut and paste:\r\n"
  "Ctrl-K cut current line, repeat to cut block of multiple lines\r\n"
  "Ctrl-C copy current line, repeat to copy block of multiple lines\r\n"
  "Ctrl-U paste cut/copied lines, can be repeated to paste multiple times\r\n"
  "\r\n"
  "Find:\r\n"
  "Ctrl-W: find forward, Ctrl-Q: find backward\r\n"
  "\r\n"
  "Other:\r\n"
  "Ctrl-R: insert file before current line, Ctrl-O to save file\r\n"
  "Ctrl-X or ESC: Exit editor (ask to save)\r\n"
  "Ctrl-T followed by two hex digits: insert special character\r\n"
  "\r\n"
  "Press any key to return to editor.";

static void EDT_ShowHelp(void)
{
  char *p=HelpText;
  VDUWrite(12);
  while (*p)
    VDUWrite(*p++);
  EDT_GetKey();
  EDT_ShowScreen();
}


void EDT_EditCore(void)
{
  bool fCutContinue = false;
  EDT.text_start = EDT.mem_start + VAR_END_OFFS;
  EDT.text_end = EDT.mem_end - CUT_BUF_SIZE;
  EDT.gap_start = EDT.text_start;
  EDT.gap_end = EDT.text_end;
  EDT.cut_end = EDT.text_end;
  EDT.mem_start[TAB_STOP_OFFS] = 8;
  EDT.mem_start[CRLF_FLAG_OFFS] = 0;
  EDT.mem_start[SCR_ROWS_OFFS] = 30;
  EDT.mem_start[CURSOR_ROW_OFFS] = 0;
  EDT.mem_start[CURSOR_COL_OFFS] = 0;
  EDT.mem_start[CURSOR_COL_MAX_OFFS] = 0;
  EDT.mem_start[CURLINE_SCROLLED_OFFS] = 0;
  EDT.mem_start[CURLINE_POS_OFFS] = 0;
  EDT.mem_start[IS_CHANGED_OFFS] = 0;
  EDT.total_lines = 0;
  EDT.cut_lines = 0;
  
  EDT_InitScreen();
  EDT_LoadFile(EDT.mem_start+FILENAME_OFFS);
  EDT.top_line = EDT.gap_start;
  EDT.mem_start[CUR_LINE_LEN_OFFS]=EDT_BufLenCurLine();
  EDT.lineno = 1;
  EDT_ShowScreen();
  for(;;){
    int k = EDT_GetKey();
    switch (k) {
    case -1:
      return;
    case 1:
    case 130:
      EDT_StartLine();
      break;
    case 2:
    case 136:
      EDT_CursorLeft();
      break;
    case 3:
      EDT_CopyLine(fCutContinue);
      fCutContinue = true;
      break;
    case 4:
    case 135:
      EDT_Delete();
      break;
    case 5:
    case 131:
      EDT_EndLine();
      break;
    case 6:
    case 137:
      EDT_CursorRight();
      break;
    case 7:
      EDT_ShowHelp();
      break;
    case 8:
      EDT_GotoLine();
      break;
    case 9:
      EDT_InsertChar(9);
      break;
    case 11:
      EDT_CutLine(fCutContinue);
      fCutContinue = true;
      break;
    case 12:
      EDT_CenterScreen();
      break;
    case 13:
      EDT_NewLine();
      break;
    case 14:
    case 138:
      EDT_CursorDown();
      break;
    case 15:
      EDT_SetCursor(0,EDT.mem_start[SCR_ROWS_OFFS]-1);
      EDT_ClrEOL();
      EDT_SetCursor(0,EDT.mem_start[SCR_ROWS_OFFS]-1);
      EDT_AskSave();
      EDT_ShowBottom();
      break;
    case 16:
    case 139:
      EDT_CursorUp();
      break;
    case 17:
      EDT_SearchB();
      break;
    case 18:
      EDT_ReadFile();
      break;
    case 20:
      EDT_InsertHex();
      break;
    case 21:
      EDT_Paste();
      break;
    case 22:
    case 133:
      EDT_PageDown();
      break;
    case 23:
      EDT_SearchF();
      break;
    case 24:
    case 27:
      VDUWrite(12);
      if (EDT.mem_start[IS_CHANGED_OFFS]) {
	bool success;
	do {
	  success =true;
	  VDUWriteString("Save file (Y/n)? ");
	  k = EDT_GetKey();
	  VDUWriteString("\r\n");
	  if (k!='n' && k!='N')
	    success=EDT_AskSave();
	} while (!success);
      }
      return;
    case 25:
    case 132:
      EDT_PageUp();
      break;
    case 127:
      EDT_BackSpace();
      break;
    default:
      if ( k>=32)
	EDT_InsertChar(k);
    }
    if (k != 11 && k != 3)
      fCutContinue = false;
  }
}
