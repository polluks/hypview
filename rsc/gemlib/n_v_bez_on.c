#include "gem_vdiP.h"
#include "mt_gemx.h"

/** After calling v_bez_on(), you can call v_bez()/v_bez_fill() without a 
 *  sub-opcode in contrl[5]. This function exists only for compatibility reasons 
 *  and usually it is only used in old GEM/3 metafiles.
 *
 *  @param handle Device handle
 *
 *  @return bezier depth, is a value between 0 (no beziers) and 7 (maximum quality) and 
 *          represents the bezier depth. You should only use this value to determine if 
 *          the driver is able to draw bezier curves.
 *
 *  @since NVDI 2.10
 *
 *  Applications should only use this call to determine, if the driver is able
 *  to draw bezier curves (check 
 *  if the return value indicates bezier support).
 *  
 *
 */

short
v_bez_on (short handle)
{
	return v_bez_con(handle, 1);
}
