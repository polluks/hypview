#include "gem_vdiP.h"
#include "mt_gemx.h"

/** see vqt_f_extent()
 *
 *  @param handle Device handle
 *  @param wstr string (16 bits per character)
 *  @param extent extent[0..7] : coordinates of the rectangle
 *
 *  @since NVDI 3.00
 *
 *
 *
 */

void
vqt_f_extent16 (short handle, const vdi_wchar_t * wstr, short extent[])
{
	short vdi_control[VDI_CNTRLMAX]; 
	short n = vdi_wstrlen (wstr);

	VDI_PARAMS(vdi_control, (short *)NO_CONST(wstr), 0L, vdi_dummy, extent);

	VDI_TRAP (vdi_params, handle, 240, 0,n);
}
