/* SOD32 the Stack Oriented Design computer.
   Copyright 1994, L.C. Benschop, Eindhoven, The Netherlands.
   The program is released under the GNU General Public License version 2.
   There is NO WARRANTY.
*/

#include "common.h"
#include "sod32.h"

#include "__image_data.h"

struct _ForthState FTH;

static void load_image(void)
{
    UNS32 len = sizeof(FTH_image_data);
    memset(FTH.mem, 0, MEMSIZE);
    memcpy(FTH.mem, FTH_image_data,len);
    FTH_swap_mem(0,len); 
}
 
void FTH_ApplicationRun(char *params)
{
    FTH.filename = (char *)MEMGetMemory();
    FTH.load_filename = params;
    FTH.mem = MEMGetMemory()+256;
    load_image();
    FTH_virtual_machine();
}
