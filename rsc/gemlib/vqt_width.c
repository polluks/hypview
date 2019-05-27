#include "gem_vdiP.h"

/** This call reports back the cell width for the specified character. If you 
 *  have selected a vector font, the returned cell width is a rounded value. 
 *  You can use this advance width for determing a character position when you 
 *  use v_gtext() but you cannot use it for v_ftext() output.
 *
 *  @param handle Device handle
 *  @param chr character index
 *  @param cw character cell width \n
 *         [option CHECK_NULLPTR] cw may be NULL
 *  @param ldelta left character alignment delta \n
 *         [option CHECK_NULLPTR] ldelta may be NULL
 *  @param rdelta right character alignment delta \n
 *         [option CHECK_NULLPTR] rdelta may be NULL
 *
 *  @return character index or -1 (error)
 *
 *  @since all VDI versions
 *
 *  Note:\n
 *  To determine the width of a string, you should always call vqt_extent(), 
 *  vqt_f_extent() or vqt_real_extent().
 *  If you try to check whether a font is monospaced or proportional, you should 
 *  test if vqt_name() returns extend information.
 *
 */

short
vqt_width (short handle, short chr, short *cw, short *ldelta, short *rdelta)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intout[1];
	short vdi_ptsout[6];
	
	VDI_PARAMS(vdi_control, &chr, 0L, vdi_intout, vdi_ptsout );
	
	VDI_TRAP (vdi_params, handle, 117, 0,1);

#if CHECK_NULLPTR
	if (cw)
#endif
	*cw     = vdi_ptsout[0];
#if CHECK_NULLPTR
	if (ldelta)
#endif
	*ldelta = vdi_ptsout[2];
#if CHECK_NULLPTR
	if (rdelta)
#endif
	*rdelta = vdi_ptsout[4];
	
	return vdi_intout[0];
}
