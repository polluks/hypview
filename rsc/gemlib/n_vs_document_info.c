#include "gem_vdiP.h"
#include "mt_gemx.h"

/** 
 *
 *  @param handle Device handle
 *  @param type type of information
 *  @param s information to set
 *  @param wchar 0=8 bits per character, 1=16 bits per character
 *
 *  @return 0=function not present, 1=all ok
 *
 *  @since NVDI 5 ?
 *
 *
 *
 */

short
vs_document_info (short handle, short type, const char *s, short wchar)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[VDI_INTINMAX];
	short vdi_intout[1]; 
	short n = 1;

	VDI_PARAMS(vdi_control, vdi_intin, 0L, vdi_intout, vdi_dummy);

	vdi_intout[0] = 0;
	vdi_intin[0]  = type;
	if (wchar) {
		const short *ws = (const short *)s;
		short i;
		
		for (i = 0; i < (VDI_INTINMAX - 1) && ws[i]; i++)
		  vdi_intin[n + i] = ws[i];
		n += i;
	} else {
		n += vdi_str2arrayn(s, (vdi_wchar_t *)vdi_intin + 1, VDI_INTINMAX-1);
	}
	VDI_TRAP_ESC (vdi_params, handle, 5,2103, 0,n);
	
	return vdi_intout[0];
}
