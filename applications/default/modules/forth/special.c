/* SOD32 the Stack Oriented Design computer.
   Copyright 1994, L.C. Benschop, Eindhoven, The Netherlands.
   The program is released under the GNU General Public License version 2.
   There is NO WARRANTY.
*/

#include "common.h"
#include "sod32.h"

#define SWAP() FTH_swap_mem(addr&CELLMASK,len+3);
#define CLIP() do {addr&=MEMMASK;len&=MEMMASK;if(addr+len>MEMSIZE)len=MEMSIZE-addr;} while(0)
#define FILEID(n) do{if (((n) & 0x80000000)==0)fp=(n);else fp=0;}while(0);

int make_name(char *addr,UNS32 len)
{
 int i;
 for(i=0;i<len && i<256;i++) {
  FTH.filename[i]=addr[i];
 }
 FTH.filename[i]='\0';
 return 1; 
}

int lastkey;
int FTH_getch(void)
{
  int k;
  if (lastkey != 0) {
    k=lastkey;
    lastkey = 0;
  } else {
    do {
      FTH_check_timer();
      if (FTH.interrupt == 100) return 0;
      k = KBDGetKey();
    } while (k==0);
  }
  return k;
}

int FTH_kbhit(void)
{
  if (lastkey != 0)
    return -1;
  else {
    lastkey = KBDGetKey();
    return lastkey != 0;
  };
}

void FTH_putch(int c)
{
    VDUWrite(c);
}

void FTH_check_timer(void)
{
    if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
      TICK50_RESET();                                                         // Reset the tick flag
      if (USBUpdate() == 0) {
	FTH.interrupt = 100; return;	
      }// Update USB
      KBDCheckTimer();
      if (KBDEscapePressed(true)) {                                               // Escaped ?
	FTH.interrupt = 12;
      } else {
	FTH.interrupt = 16;
      }
    // Check for keyboard repeat
    }
}
  

/* Replacement for fgets as the one in AgDev appears to be broken,
   does not detect EOF on real machine */
static char * my_fgets(char *s, unsigned int maxlen, UNS32 f)
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


void FTH_do_os(void)
{
 UNS32 addr,len,ior=0;
 int res;
 UNS32 fp;
 UNS32 code=CELL(FTH.save_sp);FTH.save_sp+=4;
 switch(code) {
  case 0: /*exit*/ FTH.interrupt = 101; break;
  case 1: /*read char */ FTH.save_sp-=4;CELL(FTH.save_sp)=FTH_getch();break; 
  case 2: /*print char*/ FTH_putch(CELL(FTH.save_sp));FTH.save_sp+=4;break;
  case 3: /*check key */ FTH.save_sp-=4;CELL(FTH.save_sp)=-FTH_kbhit();break;
  case 4: /* set-term */ FTH.save_sp+=4;break;
  case 7: /*open-file*/  FTH.save_sp+=4;len=CELL(FTH.save_sp);addr=CELL(FTH.save_sp+4);
    CLIP(); SWAP(); if(!make_name((char*)(FTH.mem+addr),len)) {
      ior=-202;
      SWAP(); goto end;
    } SWAP();
    {
      if(CELL(FTH.save_sp-4)>5) {
	ior=-203;
	goto end;
      }
      if ((CELL(FTH.save_sp-4) & 2)!= 0)
	FIOCreateFile(FTH.filename);
      //CONWriteString("FTH.filename: %s\n",FTH.filename);
      fp=FIOOpen(FTH.filename);
      //ONWriteString("Open res: %d\n",fp);
      if(fp & 0x80000000)ior=200;
      CELL(FTH.save_sp+4)=fp;                         
    } goto end;   
 case 8: /*close-file*/  FILEID(CELL(FTH.save_sp));FIOClose(fp); 
  			 goto end;;
 case 9: /*write-line*/ FTH.save_sp+=8;FILEID(CELL(FTH.save_sp-8)); len=CELL(FTH.save_sp-4);
    addr=CELL(FTH.save_sp);CLIP(); SWAP();
    FIOWrite(fp,(char*)(FTH.mem+addr),len);SWAP();
    FIOWrite(fp,"\r\n",2);
    goto end; 
 case 10: /*read-line*/ FILEID(CELL(FTH.save_sp)); len=CELL(FTH.save_sp+4)+2;
   addr=CELL(FTH.save_sp+8);CLIP(); SWAP();
   if (my_fgets((char*)(FTH.mem+addr),len,fp)==NULL) {
     SWAP();
     //CONWriteString("READ-LINE returns NULL\n");
     res = -1;
   } else {
     res = strlen((char*)(FTH.mem+addr));
     if(*((char*)(FTH.mem+addr)+res-1)=='\n') res--; 
     //CONWriteString("READ-LINE returns string length %d\n",res);
     SWAP();
   }
   if(res<0) {
     res=0;
     CELL(FTH.save_sp+4)=0; 
   } else {
     CELL(FTH.save_sp+4)=0xffffffff;
   }
   CELL(FTH.save_sp+8)=res;
   goto end;
 case 11: /*write-file*/FTH.save_sp+=8;FILEID(CELL(FTH.save_sp-8)); len=CELL(FTH.save_sp-4);
    addr=CELL(FTH.save_sp);CLIP(); SWAP();
    res = FIOWrite(fp,(char*)(FTH.mem+addr),len);SWAP(); if(res<0)ior=-200; 
    goto end; 
 case 12: /*read-file*/ FTH.save_sp+=4;FILEID(CELL(FTH.save_sp-4)); len=CELL(FTH.save_sp);
   addr=CELL(FTH.save_sp+4);CLIP(); SWAP();
   res=FIORead(fp,(char*)(FTH.mem+addr),len);SWAP(); 
   CELL(FTH.save_sp+4)=res;if(res<0) ior=-200;goto end;
  case 13: /*delete-file*/ FTH.save_sp+=4;len=CELL(FTH.save_sp-4);addr=CELL(FTH.save_sp);
    CLIP(); SWAP(); if(!make_name((char*)(FTH.mem+addr),len)) {ior=-202;
      SWAP(); goto end;} SWAP(); FIODeleteFile(FTH.filename); goto end;
#if 0			 
 case 14: /*reposition-file*/ save_sp+=8;FILEID(CELL(FTH.save_sp-8)); 
                          fseek(fp,CELL(FTH.save_sp),SEEK_SET);goto end;
  
 case 15: /*file-position*/ save_sp-=8;FILEID(CELL(FTH.save_sp+8));
                          CELL(FTH.save_sp+8)=ftell(fp);CELL(FTH.save_sp+4)=0;
                          goto end;
 case 17: /* file-size*/ save_sp-=8;FILEID(CELL(FTH.save_sp+8));
                          {long oldpos;
                           oldpos=ftell(fp); 
                           fseek(fp,0L,SEEK_END);
                           CELL(FTH.save_sp+8)=ftell(fp);CELL(FTH.save_sp+4)=0;
                           fseek(fp,oldpos,SEEK_SET);
                          }
                          goto end;
#endif			  
 } return;
 end: CELL(FTH.save_sp)=ior;
}

void FTH_do_special(UNS32 code) /* execute a special instruction */
{
 switch(code) {
  case 0: /* sp@ */ FTH.save_sp-=4;CELL(FTH.save_sp)=FTH.save_sp+4;break;
  case 1: /* sp! */ FTH.save_sp=CELL(FTH.save_sp)&CELLMASK;break;
  case 2: /* rp@ */ FTH.save_sp-=4;CELL(FTH.save_sp)=FTH.save_rp;break;
  case 3: /* rp! */ FTH.save_rp=CELL(FTH.save_sp)&CELLMASK;FTH.save_sp+=4;break;
  case 32: /* trap0 */ FTH_do_os();break; 
  case 50: /* FTH_set_alarm */ FTH.save_sp+=4;break;
  case 51: /* getmstimer */ FTH.save_sp-=4;CELL(FTH.save_sp)=TMRReadTimeMS();break;
 }
}

