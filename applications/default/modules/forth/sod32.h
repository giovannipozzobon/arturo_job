/* SOD32 the Stack Oriented Design computer.
   Copyright 1994 L.C. Benschop, Eindhoven, The Netherlands.
   The program is released under the GNU General Public License version 2.
   There is NO WARRANTY.

   Changes: 2025-01-11: Put parentheses around macro expansion.
*/

#define UNS8  unsigned char
#define INT32 int
#define UNS32 unsigned int

#define MEMSIZE (1*64*1024) /* must be a power of two */
#define MEMMASK (MEMSIZE-1) /* mask for addresses to force them into range */

struct _ForthState {
   UNS8 *mem;
   UNS32 save_sp,save_ip,save_rp,interrupt;
   char *filename;
};

extern struct _ForthState FTH;

#define CELL(reg) (*(UNS32*)(FTH.mem+((reg)&MEMMASK)))
#define CELLMASK (~3)

#ifdef BIG_ENDIAN
 #define BYTEMASK 0
#else
 #define BYTEMASK 3
#endif

#define BYTE(reg) FTH.mem[((reg)&MEMMASK)^BYTEMASK]

void FTH_swap_mem(UNS32,UNS32);
void FTH_do_os(void);
void FTH_do_special(UNS32);
void FTH_virtual_machine(void);
void FTH_putch(int);
int FTH_getch(void);
int FTH_kbhit(void);
void FTH_set_alarm(unsigned int);
void FTH_check_timer(void);
