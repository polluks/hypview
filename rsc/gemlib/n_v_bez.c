#include "gem_vdiP.h"
#include "mt_gemx.h"
#include "v_bez.h"

/** This function draws an unfilled bezier curve.
 *
 *  @param handle Device handle
 *  @param count 
 *  @param xyarr xyarr[0..2count-1] = coordinates
 *  @param bezarr bezarr[0..count-1] = point-type flags defined as follow:
 *         - bit 0:   first point in a 4-point bezier curve (two anchor points and two
 *                  direction points). The last point of a bezier segment can be the 
 *                  first point of the next bezier curve (or it can be a jump point).
 *         - bit 1:   jump point - this point and the previous one will not be connected.
 *         - Bit 2-7 are reserved. If bit 0 is 0, v_bez() works like v_pline() and draws 
 *                   a straight line between two points.
 *  @param extent coordinates of the bounding box
 *  @param totpts number of points in the resulting polygon
 *  @param totmoves number of moves in the polygon
 *
 *  @since since NVDI 2.10
 *
 *
 *
 */

void
v_bez (short handle, short count, short *xyarr, char *bezarr,
       short *extent, short *totpts, short *totmoves)
{
	short vdi_control[VDI_CNTRLMAX]; 
	short vdi_intin[VDI_INTINMAX];   
	short vdi_ptsin[VDI_PTSINMAX];   
	short vdi_intout[6]; 

	VDI_PARAMS( vdi_control, vdi_intin, vdi_ptsin, vdi_intout, extent);
	if(count>VDI_INTINMAX) return;

	_v_bez (count, xyarr, bezarr, vdi_intin, vdi_ptsin);
	
	VDI_TRAP_ESC (vdi_params, handle, 6,13, count,(count +1) /2);
	
	*totpts   = vdi_intout[0];
	*totmoves = vdi_intout[1];
}

