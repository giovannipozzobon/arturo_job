/* Nano-style editor
   Copyright 2025, L.C. Benschop, Eindhoven, The Netherlands.
   MIT license
*/

#include "common.h"
#include "edit.h"

void EDT_LoadFile(unsigned char* filename)
{
  unsigned int fp;
  int len,curlinelen;
  unsigned char *p;
  fp = FIOOpen((char*)filename);
  if((fp & 0x80000000)==0) {    
    len = FIORead(fp, EDT.gap_start, EDT.gap_end-EDT.gap_start);
    if (len>0) {
      //VDUWriteString("Read %d bytes\n",len);
      p=EDT.gap_start + len;
      if (*(p-1)!='\n') {
	*--(EDT.gap_end) = '\n';
      }
      curlinelen=0;
      while (p!=EDT.gap_start) {
	char c=*--p;
	if (EDT.gap_start == EDT.gap_end) {
	  // No more room for file.
	  break;
	}
	if (((unsigned)c>=32 && c!=127) || c=='\n' || c=='\t') {
	  /* Filter all control chars but LF and TAB */
	  if (c=='\n') {
	    curlinelen = 0;
	    EDT.total_lines++;
	  } else {
	    curlinelen++;
	  }
	  if (curlinelen==254) {
	    /* Force lines to be at most 254 chars (excluding nl) */
	    *--(EDT.gap_end) = '\n';
	    EDT.total_lines++;
	    if (EDT.gap_start == EDT.gap_end) {
	      // No more room for file.
	      break;
	    }
	    curlinelen=0;
	  }
	  *--(EDT.gap_end) = c;
	}
      }
    }
    FIOClose(fp);
  }
  if (EDT.text_end == EDT.gap_end) {
    *--(EDT.gap_end) = '\n';
    EDT.total_lines++;
  }
}


bool EDT_SaveFile(unsigned char* filename)
{
  unsigned int fp;
  char *p;
  /* Create backup file name */
  strcpy((char*)EDT.mem_start+BACKFILENAME_OFFS,(char*)filename);
  p = strrchr((char*)EDT.mem_start+BACKFILENAME_OFFS, '.');
  if (!p) {
    p=(char*)EDT.mem_start+BACKFILENAME_OFFS;
    p+=strlen(p);
  }
  if (p-(char*)(EDT.mem_start+BACKFILENAME_OFFS) > MAX_NAME_LENGTH-4)
    return false;
  strcpy(p,".bak");
  /* Delete old backup file, this may fail, don't care */
  FIODeleteFile((char*)EDT.mem_start+BACKFILENAME_OFFS);
  /* Rename current file into backup file, this may fail, don't care  */
  FIORenameFile((char*)filename, (char*)EDT.mem_start+BACKFILENAME_OFFS);
  /* Create file to save */
  FIOCreateFile((char*)filename);
  fp = FIOOpen((char*)filename);
  if (fp<0) return false;
  if (EDT.gap_start > EDT.text_start) {
    FIOWrite(fp,EDT.text_start,EDT.gap_start - EDT.text_start);
  }
  if (EDT.text_end > EDT.gap_end) {
    FIOWrite(fp,EDT.gap_end, EDT.text_end - EDT.gap_end);
  }
  FIOClose(fp);
  return true;
}
