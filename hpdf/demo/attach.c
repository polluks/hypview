/*
 * << Haru Free PDF Library 2.0.0 >> -- attach.c
 *
 * Copyright (c) 1999-2006 Takeshi Kanno <takeshi_kanno@est.hi-ho.ne.jp>
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.
 * It is provided "as is" without express or implied warranty.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"

const char *text = "This PDF should have an attachment named attach.c";

static jmp_buf env;

static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	(void) user_data;
	printf("ERROR: error_no=%04X (%s), detail_no=%u\n", (unsigned int) error_no, HPDF_ErrorStr(error_no), (unsigned int) detail_no);
	longjmp(env, 1);
}


int main(int argc, char **argv)
{
	HPDF_Doc pdf;
	HPDF_Font font;
	HPDF_Page page;
	char fname[256];
	HPDF_REAL tw;

	(void) argc;
	strcpy(fname, argv[0]);
	strcat(fname, ".pdf");

	pdf = HPDF_New(error_handler, 0, 0, 0, NULL);
	if (!pdf)
	{
		printf("error: cannot create PdfDoc object\n");
		return 1;
	}

	if (setjmp(env))
	{
		HPDF_Free(pdf);
		return 1;
	}

	/* create default-font */
	font = HPDF_GetFont(pdf, "Helvetica", NULL);

	/* add a new page object. */
	page = HPDF_AddPage(pdf);

	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 20);
	tw = HPDF_Page_TextWidth(page, text);
	HPDF_Page_MoveTextPos(page, (HPDF_Page_GetWidth(page) - tw) / 2, (HPDF_Page_GetHeight(page) - 20) / 2);
	HPDF_Page_ShowText(page, text);
	HPDF_Page_EndText(page);

	/* attach a file to the document */
	HPDF_AttachFile(pdf, "attach.c");
	HPDF_AttachFile(pdf, "jpeg_demo.c");

	/* save the document to a file */
	HPDF_SaveToFile(pdf, fname);

	/* clean up */
	HPDF_Free(pdf);

	return 0;
}
