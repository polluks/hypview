#include "gem_vdiP.h"
#include "mt_gemx.h"

short vs_load_disp_list(short handle, const char *name)
{
	short vdi_control[VDI_CNTRLMAX];
	short vdi_intout[2];
	short vdi_intin[N_PTRINTS];

	VDI_PARAMS (vdi_control, vdi_intin, vdi_dummy, vdi_intout, vdi_dummy);
	
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
	vdi_intin_ptr(0, const char *) = name;
	vdi_intout[0] = 0;
	VDI_TRAP_ESC (vdi_params, handle, 5,2107, 0, 2);
	
	return vdi_intout[0];
}
