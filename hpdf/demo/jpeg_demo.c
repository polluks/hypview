/*
 * << Haru Free PDF Library 2.0.0 >> -- jpeg_demo.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"

#if defined(__WIN32__) || defined(__atarist__)
#define FILE_SEPARATOR "\\"
#else
#define FILE_SEPARATOR "/"
#endif
#ifndef SRCDIR
#define SRCDIR "."
#endif

static jmp_buf env;

static void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
	(void) user_data;
	printf("ERROR: error_no=%04X (%s), detail_no=%u\n", (unsigned int) error_no, HPDF_ErrorStr(error_no), (unsigned int) detail_no);
	longjmp(env, 1);
}

static void draw_image(HPDF_Doc pdf, const char *filename, float x, float y, const char *text)
{
	char filename1[255];

	HPDF_Page page = HPDF_GetCurrentPage(pdf);
	HPDF_Image image;

	strcpy(filename1, SRCDIR FILE_SEPARATOR "images" FILE_SEPARATOR);
	strcat(filename1, filename);

	image = HPDF_LoadJpegImageFromFile(pdf, filename1);

	/* Draw image to the canvas. */
	HPDF_Page_DrawImage(page, image, x, y, HPDF_Image_GetWidth(image), HPDF_Image_GetHeight(image));

	/* Print the text. */
	HPDF_Page_BeginText(page);
	HPDF_Page_SetTextLeading(page, 16);
	HPDF_Page_MoveTextPos(page, x, y);
	HPDF_Page_ShowTextNextLine(page, filename);
	HPDF_Page_ShowTextNextLine(page, text);
	HPDF_Page_EndText(page);
}


int main(int argc, char **argv)
{
	HPDF_Doc pdf;
	HPDF_Font font;
	HPDF_Page page;
	char fname[256];
	HPDF_Destination dst;

	(void) argc;
	strcpy(fname, argv[0]);
	strcat(fname, ".pdf");

	pdf = HPDF_New(error_handler, 0, 0, 0, NULL);
	if (!pdf)
	{
		printf("error: cannot create PdfDoc object\n");
		return 1;
	}

	/* error-handler */
	if (setjmp(env))
	{
		HPDF_Free(pdf);
		return 1;
	}

	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

	/* create default-font */
	font = HPDF_GetFont(pdf, "Helvetica", NULL);

	/* add a new page object. */
	page = HPDF_AddPage(pdf);

	HPDF_Page_SetWidth(page, 650);
	HPDF_Page_SetHeight(page, 500);

	dst = HPDF_Page_CreateDestination(page);
	HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
	HPDF_SetOpenAction(pdf, dst);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 20);
	HPDF_Page_MoveTextPos(page, 220, HPDF_Page_GetHeight(page) - 70);
	HPDF_Page_ShowText(page, "JpegDemo");
	HPDF_Page_EndText(page);

	HPDF_Page_SetFontAndSize(page, font, 12);

	draw_image(pdf, "rgb.jpg", 70, HPDF_Page_GetHeight(page) - 410, "24bit color image");
	draw_image(pdf, "gray.jpg", 340, HPDF_Page_GetHeight(page) - 410, "8bit grayscale image");

	/* save the document to a file */
	HPDF_SaveToFile(pdf, fname);

	/* clean up */
	HPDF_Free(pdf);

	return 0;
}
