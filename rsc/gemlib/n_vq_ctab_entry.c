#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param index 
 *  @param color  \n
 *         [option CHECK_NULLPTR] color may be NULL
 *
 *  @return 
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

int32_t
vq_ctab_entry (short handle, short index, COLOR_ENTRY * color)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[6]; 

	VDI_PARAMS(vdi_control, &index, 0L, vdi_intout, vdi_dummy);

	VDI_TRAP_ESC (vdi_params, handle, 206,1, 0,1);

#if CHECK_NULLPTR
	if (color)
#endif
	*color = *(COLOR_ENTRY*)&vdi_intout[2];
	
	return vdi_intout_long(0);
}
