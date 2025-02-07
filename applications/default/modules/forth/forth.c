/* SOD32 the Stack Oriented Design computer.
   Copyright 1994, L.C. Benschop, Eindhoven, The Netherlands.
   The program is released under the GNU General Public License version 2.
   There is NO WARRANTY.
*/

#include "common.h"
#include "sod32.h"

#include "__image_data.h"

struct _ForthState FTH;

void load_image(void)
{
  UNS32 len = sizeof(FTH_image_data);
  memcpy(FTH.mem, FTH_image_data,len);
  swap_mem(0,len); 
}
 
void ApplicationRun(void)
{
  FTH.mem = MEMGetMemory();
  do {
    load_image();
    virtual_machine();
  } while (FTH.interrupt != 100);
}
