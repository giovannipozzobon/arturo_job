/* Nano-style editor
   Copyright 2025, L.C. Benschop, Eindhoven, The Netherlands.
   MIT license
*/

#include "common.h"
#include "edit.h"


unsigned int  EDT_GetKey(void)
{
  int k;
    do {
      if (SYSYield()) {
	if (!SYSAppRunning()) {
	  return -1;
	}
	KBDEscapePressed(true);
      }
      k = KBDGetKey();
    } while (k==0);
    return k;
}


void EDT_InitScreen(void)
{
  VDUWrite(22); VDUWrite(EDT.scr_rows==60?2:0);  
}

void EDT_InvVideo(void)
{
  VDUWrite(17);VDUWrite(0);VDUWrite(17);VDUWrite(135);
}

void EDT_TrueVideo(void)
{
  VDUWrite(17);VDUWrite(7);VDUWrite(17);VDUWrite(128);
}

void EDT_SetCursor(int x, int y)
{
  VDUWrite(31);VDUWrite(x);VDUWrite(y);
}

void EDT_ReadLine(unsigned char* buf, int len)
{
  int i=0;
  int c;
  for(;;) {
    if (buf[i]==0) break;
    VDUWrite(buf[i++]);
  }
  for(;;) {
    c=EDT_GetKey();
    switch(c) {
    case -1:
      return;
    case 13:
      return;
    case 127:
      if (i>0) {
	VDUWrite(127);
	i--;
	buf[i]=0;
      }
      break;
    default:
      if (i<len && c>=32) {
	VDUWrite(c);
	buf[i++]=c;
	buf[i]=0;
      }
    }
  }
}

void EDT_ClrEOL(void)
{
  int x,y,n;
  VDUGetCursor(&x,&y);
  n = SCR_COLS-x;
  if (y==EDT.scr_rows-1)
    n--;
  for (int i=0; i<n; i++) VDUWrite(32);
}

unsigned char * EDT_RenderLine(unsigned char *p, bool is_current)
{
  unsigned char c;
  bool is_scrolled=false;
  int col = 0;
  unsigned char *q=p;
  int tabstop = EDT.tab_stop;
  if (is_current) {
    /* First find out how much to scroll the line to the left if the current
       edit position is beyond the width of the screen */
    for(;;) {
      if (q==EDT.gap_start) {
	break;
      }
      if (*q=='\n') {
	break;
      }
      if (*q++=='\t') {
	col = (col + tabstop) & (-tabstop);
	if (col>SCR_COLS-1)col=SCR_COLS-1;
      } else {
	col++;
      }
      if (col==SCR_COLS-1) {
	col=1;
	p = q;
	is_scrolled=true;
      }
    }
    EDT.cursor_col=col;
  }
  if (is_scrolled) {
    EDT_InvVideo();
    VDUWrite('<');
    EDT_TrueVideo();
    col=1;
  } else {
    col=0;
  }
  for(;;) {
    if (p==EDT.gap_start) {
      p=EDT.gap_end;
    }
    c=*p++;
    if (c=='\n') {
      if (col==SCR_COLS) {
	EDT_InvVideo();
	VDUWrite('>');
	EDT_TrueVideo();	
      }  else if (is_current) {   
	EDT_ClrEOL();
      } else {
	VDUWrite(13);VDUWrite(10);
      }
      break;
    } else if (c=='\t' && col<SCR_COLS-1) {
      do {
	VDUWrite(' ');
	col++;
      } while ( (col & (tabstop-1)) && col < SCR_COLS-1);
    } else if (col<SCR_COLS-1) {
      VDUWrite(c);
      col++;
    } else {
      col = SCR_COLS;
    }
  }
  return p;
}

void EDT_RenderCurrentLine(void)
{
  EDT_SetCursor(0,
		EDT.cursor_row+1);
  EDT_RenderLine(EDT.gap_start-EDT.curline_pos,true);
  EDT_ShowCursor();
}

void EDT_LeaveCurrentLine(void)
{
  EDT_SetCursor(0,
		EDT.cursor_row+1);
  EDT_RenderLine(EDT.gap_start-EDT.curline_pos,false);
}


void EDT_ShowCursor(void)
{
  EDT_SetCursor(EDT.cursor_col,
		EDT.cursor_row+1);
  if (EDT.cursor_col_max < EDT.cursor_col) {
    EDT.cursor_col_max = EDT.cursor_col;
  }
}


void EDT_ShowBottom(void)
{
  EDT_SetCursor(0,EDT.scr_rows-1);
  EDT_InvVideo(); 
  VDUWriteString("Line %d/%d, %d/%d bytes -- ESC to exit, ^G for help %c Cut %d",
		 EDT.lineno,
		 EDT.total_lines,
		 EDT.gap_start-EDT.text_start + EDT.text_end-EDT.gap_end,
		 EDT.text_end - EDT.text_start,
		 EDT.is_changed?'*':' ',
		 EDT.cut_lines
		 );
  EDT_ClrEOL();
  EDT_TrueVideo();
}

void EDT_ShowScreen(void)
{
  unsigned char *p = EDT.top_line;
  VDUWrite(12);
  EDT_InvVideo();
  VDUWriteString("Arturo Editor: %s",EDT.mem_start+FILENAME_OFFS);
  EDT_ClrEOL();
  EDT_TrueVideo();

  for (int i=0; i<EDT.scr_rows-2 && p!=EDT.text_end; i++) {
    p = EDT_RenderLine(p,i==EDT.cursor_row);
  }
  EDT_ShowBottom();
  EDT_ShowCursor();
}

void EDT_AdjustTop(bool always_redraw)
{
  if (EDT.top_line > EDT.gap_start ||
      EDT.cursor_row >= EDT.scr_rows -2) {
    int lines_to_move = (EDT.scr_rows -2)/2;
    EDT.cursor_row = 0;
    EDT.top_line = EDT.gap_start - EDT.curline_pos;
    while (lines_to_move && EDT.top_line > EDT.text_start) {
      lines_to_move--; 
      EDT.cursor_row++;
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

