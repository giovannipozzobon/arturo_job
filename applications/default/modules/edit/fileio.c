/* Nano-style editor
   Copyright 2025, L.C. Benschop, Vught, The Netherlands.
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
	if (EDT.crlf_flag == 2 && c==13) EDT.crlf_flag = 1;
	/* Mark file for saving in CR-LF mode. */
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
  if (EDT.crlf_flag != 1) {
    if (EDT.gap_start > EDT.text_start) {
      FIOWrite(fp,EDT.text_start,EDT.gap_start - EDT.text_start);
    }
    if (EDT.text_end > EDT.gap_end) {
      FIOWrite(fp,EDT.gap_end, EDT.text_end - EDT.gap_end);
    }
  } else {
    unsigned char *p;
    for (p=EDT.text_start; p<EDT.gap_start; p++) {
      if (*p=='\n')
	FIOWrite(fp,"\r",1);
      FIOWrite(fp,p,1);
    }
    for (p=EDT.gap_end; p<EDT.text_end; p++) {
      if (*p=='\n')
	FIOWrite(fp,"\r",1);
      FIOWrite(fp,p,1);
    }
  }
  FIOClose(fp);
  return true;
}

/* Replacement for fgets as the one in AgDev appears to be broken,
   does not detect EOF on real machine */
static char * my_fgets(char *s, unsigned int maxlen, unsigned int f)
{
  unsigned int i;
  char c;
  for(i=0; i<maxlen-1;) {
    if (FIORead(f,&c,1) <= 0) break;
    if (c=='\r') continue;
    s[i] = c;
    i++;
    if (c=='\n') break;
  }
  s[i] = 0;
  if (i==0)
    return 0;
  else
    return s;
}

static unsigned char *
parse_word(unsigned char *p)
{
  unsigned char *q;
  while (*p <= 32) {
    if (*p==0) return p;
    p++;
  }
  q=p;
  while (*q > 32) {
    q++;
  }
  *q=0;
  return p;
}

void EDT_LoadConfig(char *name)
{
  unsigned int fp;
  unsigned char *p,*q;
  fp = FIOOpen(name);
  if((fp & 0x80000000)==0) {    
    while (my_fgets((char*)EDT.text_start, 81, fp)) {
      if (EDT.text_start[0]=='\n' || EDT.text_start[0]=='#') continue;
      /* skip blank lines. */
      p=EDT.text_start;
      p=parse_word(p);
      q=p+strlen((char*)p)+1;
      q=parse_word(q);
      //VDUWriteString("%s %s\r\n",(char*)p,(char*)q);
      if (strcmp((char*)p,"mode")==0) {
	if (strcmp((char*)q,"2") == 0) {
	  EDT.scr_rows=60;
	} else if (strcmp((char*)q,"0") == 0) {
	  EDT.scr_rows=30;
	}
      } else if (strcmp((char*)p,"tabstop")==0) {
	if (strcmp((char*)q,"4") == 0) {
	  EDT.tab_stop=4;
	} else if (strcmp((char*)q,"8") == 0) {
	  EDT.tab_stop=8;
	}
      } else if (strcmp((char*)p,"eoln") == 0) {
	if (strcmp((char*)q,"crlf") == 0) {
	  EDT.crlf_flag = 1;
	} else if (strcmp((char*)q,"keep") == 0) {
	  EDT.crlf_flag = 2;
	} else if (strcmp((char*)q,"lf") == 0) {
	  EDT.crlf_flag = 0;
	}
      }
    }
    //EDT_GetKey();
    FIOClose(fp);
  }
}
