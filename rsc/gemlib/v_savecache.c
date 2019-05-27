#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param filename
 *
 *  @return 
 *
 *  @since 
 *
 *  @sa
 *
 *
 *
 */

short
v_savecache (short handle, const char *filename)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[VDI_INTINMAX];
	short vdi_intout[2]; /* todo (doc wanted) */
	short n = vdi_str2arrayn(filename, (vdi_wchar_t *)vdi_intin, VDI_INTINMAX);

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy );
	
	VDI_TRAP (vdi_params, handle, 249, 0,n);
	
	return vdi_intout[0];
}
