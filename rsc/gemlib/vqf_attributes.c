#include "gem_vdiP.h"

/** reports back the current attributes for filled objects.
 *
 *  @param handle Device handle
 *  @param attrib point to a 5-shorts array which will contain:
 *         - attrib[0]           fill type
 *         - attrib[1]           fill color
 *         - attrib[2]           fill style index
 *         - attrib[3]           writing mode
 *         - attrib[4]           perimeter flag
 *
 *  @since all VDI versions
 *
 */

void
vqf_attributes (short handle, short attrib[])
{
	short vdi_control[VDI_CNTRLMAX]; 

	VDI_PARAMS(vdi_control, 0L, 0L, attrib, vdi_dummy );
	
	VDI_TRAP_00 (vdi_params, handle, 37);
}
