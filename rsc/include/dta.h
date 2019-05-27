/*
 * dta.h - the DTA
 *
 * This file exists to centralise the definition of the DTA, which was
 * previously defined in several different places.
 *
 * Copyright (C) 2014-2015 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DTA_H
#define _DTA_H

#include "portab.h"

typedef struct
{
	char	d_reserved[21]; 	/* internal EmuTOS usage */
	char	d_attrib;			/* attributes */
	uint16_t d_time; 			/* packed time */
	uint16_t d_date; 			/* packed date */
	int32_t	d_length;			/* size */
	char	d_fname[14];		/* name */
} DTA;

#endif /* _DTA_H */
