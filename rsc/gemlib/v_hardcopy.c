#include "gem_vdiP.h"

/** The device generates a hard copy with this
 *	escape. The escape is device-specific and
 *	copies the physical screen to a printer or
 *	other attached hard copy device.
 *
 *  @param handle Device handle
 *
 *  @since all VDI versions
 *
 */

void
v_hardcopy (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_dummy, vdi_dummy );
	
	VDI_TRAP_ESC (vdi_params, handle, 5,17, 0,0);
}
