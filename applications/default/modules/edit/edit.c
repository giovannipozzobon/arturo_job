/* Nano-style editor
   Copyright 2025, L.C. Benschop, Vught, The Netherlands.
   MIT license
*/

#include "common.h"
#include "edit.h"
#include <string.h>

struct _EditState EDT;


void EDT_ApplicationRun(char *params)
{
  EDT.mem_start=MEMGetMemory();
  EDT.mem_end=EDT.mem_start+VAR_END_OFFS+EDIT_BUF_SIZE+CUT_BUF_SIZE;
  strncpy((char*)EDT.mem_start + FILENAME_OFFS, params, MAX_NAME_LENGTH);
  EDT_EditCore();
}
