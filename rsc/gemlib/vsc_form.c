#include "gem_vdiP.h"

/** modifies the appearance of the mouse cursor
 *
 *  @param handle Device handle
 *  @param form  cursor data:
 *         - form[0]:       x-coordinate of hot spot
 *         - form[1]:       y-coordinate of hot spot
 *         - form[2]:       must be 1 (REPLACE)
 *         - form[3]:       background color
 *         - form[4]:       foreground color
 *         - form[5..20]:   background mask
 *         - form[21..36]:  foreground mask
 *  
 *  @since all VDI versions
 *
 *  @note Use mt_graf_mouse() to change the mouse cursor !
 *
 *
 */

void
vsc_form (short handle, short form[])
{
	short vdi_control[VDI_CNTRLMAX]; 
	
	VDI_PARAMS(vdi_control, form, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 111, 0,37);
}
