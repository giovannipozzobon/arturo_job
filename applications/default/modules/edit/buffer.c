/* Nano-style editor
   Copyright 2025, L.C. Benschop, Eindhoven, The Netherlands.
   MIT license
*/

#include "common.h"
#include "edit.h"


void EDT_BufStartLine(void)
{
  while (EDT.gap_start > EDT.text_start &&
	 EDT.gap_start[-1] != '\n') {
    EDT_BufPrevChar();
  }
}

void EDT_BufEndLine(void)
{
  while (*EDT.gap_end != '\n') {
    EDT_BufNextChar();
  }  
}


int EDT_BufLenCurLine(void)
{
  int i=0;
  while (EDT.gap_end[i] != '\n') {
    i++;
  }
  return i;
}



void EDT_BufInsertChar(unsigned char c)
{
  if (EDT.mem_start[CUR_LINE_LEN_OFFS] < 254 &&
      EDT.gap_start < EDT.gap_end-1) {
    EDT.mem_start[IS_CHANGED_OFFS]=1;
    *EDT.gap_start++ = c;
    EDT.mem_start[CUR_LINE_LEN_OFFS]++;
    EDT.mem_start[CURLINE_POS_OFFS]++;
  }
}

void EDT_BufInsertNL(void)
{
  if (EDT.gap_start < EDT.gap_end-1) {
    *EDT.gap_start++ = '\n';    
    EDT.mem_start[IS_CHANGED_OFFS]=1;
    EDT.lineno++;
    EDT.total_lines++;
  }
}


void EDT_BufDeleteChar(void)
{
  EDT.gap_end++;
  EDT.mem_start[CUR_LINE_LEN_OFFS]--;
  EDT.mem_start[IS_CHANGED_OFFS]=1;
}

/*
 * 
 * Check that length current line + length next line <= 254.
 * If so, delete newline char. decrease total number of lines,
 *
 * Pre: gap_end points to (non-final) newline.
 */
void EDT_BufJoinLines(void)
{
  int nextlen;
  if (EDT.gap_end < EDT.text_end) {
    EDT_BufNextChar();
    nextlen = EDT_BufLenCurLine();
    if (EDT.mem_start[CUR_LINE_LEN_OFFS] + nextlen < 255) {
      EDT.mem_start[IS_CHANGED_OFFS]=1;
      EDT.mem_start[CUR_LINE_LEN_OFFS] += nextlen;
      EDT.gap_start--;
      EDT.total_lines--;
    }
  }
}


void EDT_BufNextLine(void)
{
  EDT_BufEndLine();
  EDT_BufNextChar();
}

void EDT_BufPrevLine(void)
{
  EDT_BufStartLine();
  EDT_BufPrevChar();
  EDT_BufStartLine();
}


void EDT_BufNextChar(void)
{
  if (EDT.gap_end < EDT.text_end) {
    *EDT.gap_start++ = *EDT.gap_end++;
  }
}

void EDT_BufPrevChar(void)
{
  if (EDT.gap_start > EDT.text_start) {
    *--EDT.gap_end = *--EDT.gap_start;
  }
}

void EDT_BufAdjustCol(void)
{
  int targetcol = EDT.mem_start[CURSOR_COL_MAX_OFFS];
  int col=0;
  int tabstop = EDT.mem_start[TAB_STOP_OFFS];
  EDT.mem_start[CUR_LINE_LEN_OFFS]=EDT_BufLenCurLine();
  if (targetcol == 0) {
    targetcol = EDT.mem_start[CURSOR_COL_OFFS];
  }
  EDT.mem_start[CURLINE_POS_OFFS] = 0;
  while (col < targetcol) {
    if (*EDT.gap_end=='\n') return;
    if (*EDT.gap_end=='\t') {
      col = (col + tabstop) & (-tabstop);
    } else {
      col++;
    }
    EDT_BufNextChar();  
    EDT.mem_start[CURLINE_POS_OFFS] ++;
  }
  if (col > EDT.mem_start[CURSOR_COL_MAX_OFFS]) {
    EDT.mem_start[CURSOR_COL_MAX_OFFS] = col;
  }    
}


void EDT_AdjustTop(bool always_redraw)
{
  if (EDT.top_line > EDT.gap_start ||
      EDT.mem_start[CURSOR_ROW_OFFS] >= EDT.mem_start[SCR_ROWS_OFFS] -2) {
    int lines_to_move = (EDT.mem_start[SCR_ROWS_OFFS] -2)/2;
    EDT.mem_start[CURSOR_ROW_OFFS] = 0;
    EDT.top_line = EDT.gap_start - EDT.mem_start[CURLINE_POS_OFFS];
    while (lines_to_move && EDT.top_line > EDT.text_start) {
      lines_to_move--; 
      EDT.mem_start[CURSOR_ROW_OFFS]++;
      EDT.top_line--;
      while (EDT.top_line > EDT.text_start && EDT.top_line[-1] != '\n') {
	EDT.top_line--;
      }
    }
    EDT_ShowScreen();
  } else if (always_redraw) {
    EDT_ShowScreen();
  } else {
    EDT_ShowBottom();
    EDT_RenderCurrentLine();
  }
}
