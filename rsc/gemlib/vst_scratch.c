#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param mode type of buffer
 *
 *  @since SpeedoGDOS 4.0
 *
 *
 *
 */

void
vst_scratch (short handle, short mode)
{
	short vdi_control[VDI_CNTRLMAX]; 
	/* todo (doc wanted) */
	VDI_PARAMS(vdi_control, &mode, 0L, vdi_dummy, vdi_dummy);
	
	VDI_TRAP (vdi_params, handle, 244, 0,1);
}
