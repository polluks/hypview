#include "gem_vdiP.h"

/** If a function key has been pressed "INPUT CHOICE" returns its number.
 *
 *  @param handle Device handle
 *  @param choice key number (1-10)
 *
 *  @return 0  (no key) or 1 (key pressed)
 *
 *  @since all VDI versions
 *
 */

short
vsm_choice (short handle, short *choice)
{
	short vdi_control[VDI_CNTRLMAX]; 
	
	VDI_PARAMS(vdi_control, 0L, 0L, choice, vdi_dummy );
	
	VDI_TRAP_00 (vdi_params, handle, 30);
	
	return VDI_N_INTOUT;
}
