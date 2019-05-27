#include "gem_vdiP.h"

/** 
 *
 *  @param handle Device handle
 *  @param xres 
 *  @param yres 
 *  @param xset  \n
 *         [option CHECK_NULLPTR] xset may be NULL
 *  @param yset  \n
 *         [option CHECK_NULLPTR] yset may be NULL
 *
 *  @since 
 *
 *  @sa
 *
 *
 *
 */

void
vt_resolution (short handle, short xres, short yres, short *xset, short *yset)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[2];   
	short vdi_intout[2];
	
	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);
	
	vdi_intin[0] = xres;
	vdi_intin[1] = yres;
	
	VDI_TRAP_ESC (vdi_params, handle, 5,81, 0,2);
	
#if CHECK_NULLPTR
	if (xset)
#endif
	*xset = vdi_intout[0];
#if CHECK_NULLPTR
	if (yset)
#endif
	*yset = vdi_intout[1];
}
