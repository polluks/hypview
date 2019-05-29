#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *
 *  @return -1 if scaling did not take place, else fix31 value of scaling factor
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

int32_t
vq_prn_scaling (short handle)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[2] = {-1,-1};   
	short vdi_intout[2]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 5,39, 0,2);

	return VDI_N_INTOUT == 2 ? vdi_intout_long(0) : -1L;
}
