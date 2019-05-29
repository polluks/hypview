#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *
 *  @return id of the color table 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

int32_t v_get_ctab_id (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[2]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 206,6, 0,0);

	return vdi_intout_long(0);
}
