#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param index color table index
 *
 *  @return pixel value
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

uint32_t v_ctab_idx2value (short handle, short index)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[2]; 

	VDI_PARAMS(vdi_control, &index, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 206,5, 0,1);

	return vdi_intout_long(0);
}
