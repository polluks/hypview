#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param base_handle Device handle
 *  @param bitmap pointer to GCBITMAP
 *  @param color_flags treatment of the color tables
 *  @param unit_flags measurement unit of pixel size
 *  @param pixel_width pixel width, or 0
 *  @param pixel_height pixel height, or 0
 *
 *  @return handle of the bitmap
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

short
v_open_bm (short base_handle, GCBITMAP * bitmap, short color_flags,
           short unit_flags, short pixel_width, short pixel_height)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[4];   
	short vdi_intout[1]; 

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	vdi_intin[0] = color_flags;
	vdi_intin[1] = unit_flags;
	vdi_intin[2] = pixel_width;
	vdi_intin[3] = pixel_height;

	vdi_control_ptr(0, GCBITMAP *) = bitmap;
	VDI_TRAP_ESC (vdi_params, base_handle, 100,3, 0,4);

	return VDI_HANDLE;
}
