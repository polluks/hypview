#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param fg_color  \n
 *         [option CHECK_NULLPTR] fg_color may be NULL
 *
 *  @return 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

int32_t
vqm_fg_color (short handle, COLOR_ENTRY * fg_color)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[6]; 

	VDI_PARAMS(vdi_control, 0L, 0L, vdi_intout, vdi_dummy);
	
	VDI_TRAP_ESC (vdi_params, handle, 202,3, 0,0);

#if CHECK_NULLPTR
	if (fg_color)
#endif
	*fg_color = *(COLOR_ENTRY*)&vdi_intout[2];
	
	return vdi_intout_long(0);
}
