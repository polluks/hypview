#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param itab reference to inverse color table
 *
 *  @return 0=error, 1=all ok
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

short
v_delete_itab (short handle, ITAB_REF itab)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[1]; 

	VDI_PARAMS(vdi_control, (short*)&itab, 0L, vdi_intout, vdi_dummy);

	VDI_TRAP_ESC (vdi_params, handle, 208,1, 0,2);

	return vdi_intout[0];
}
