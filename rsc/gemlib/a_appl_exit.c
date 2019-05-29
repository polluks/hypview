#include "gem_aesP.h"

/** Unregister an application from the AES and free its AES ID.
 *
 *  @return 0 if an error occurred or non-zero otherwise.
 *
 *  @since All AES versions.
 *
 *  @sa mt_appl_init()
 *
 *  The handling of an error (return value 0) is currently undefined.
 *
 */

short appl_exit(void)
{
	AES_PARAMS(19,0,1,0,0);

	AES_TRAP(aes_params);
	return aes_intout[0];
}
