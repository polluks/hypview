#include "hv_gtk.h"
#include "hypdebug.h"

/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

char *HypGetTextLine(WINDOW_DATA *win, HYP_NODE *node, long line)
{
	DOCUMENT *doc;
	GtkTextIter start, end;
	char *txt;
	
	if (win == NULL || (doc = win->data) == NULL || doc->data == NULL || node == NULL || line < 0 || line >= node->lines)
		return NULL;

	gtk_text_buffer_get_iter_at_line(win->text_buffer, &start, line);
	gtk_text_buffer_get_iter_at_line(win->text_buffer, &end, line + 1);
	txt = gtk_text_buffer_get_text(win->text_buffer, &start, &end, FALSE);
	if (txt && *txt)
	{
		char *end = txt + strlen(txt) - 1;
		if (*end == '\n')
			*end = '\0';
	}
	return txt;
}

/*** ---------------------------------------------------------------------- ***/

long HypAutolocator(WINDOW_DATA *win, long line, const char *search)
{
	DOCUMENT *doc = hypwin_doc(win);
	const char *src;
	HYP_NODE *node;
	char *temp;
	size_t len;
	HYP_DOCUMENT *hyp;

	hyp = (HYP_DOCUMENT *) doc->data;
	if (hyp == NULL)
		return -1;
	node = win->displayed_node;
	
	if (node == NULL)						/* no node loaded */
		return -1;

	if (empty(search))
		return -1;
	
	len = strlen(search);

	if (doc->autolocator_dir > 0)
	{
		while (line < node->lines)
		{
			temp = HypGetTextLine(win, node, line);
			if (temp != NULL)
			{
				src = temp;
				while (*src)
				{
					if (g_utf8_strncasecmp(src, search, len) == 0)
					{
						g_free(temp);
						return line;
					}
					src++;
				}
				g_free(temp);
			}
			line++;
		}
	} else
	{
		while (line > 0)
		{
			temp = HypGetTextLine(win, node, line);
			if (temp != NULL)
			{
				src = temp;
				while (*src)
				{
					if (g_utf8_strncasecmp(src, search, len) == 0)
					{
						g_free(temp);
						return line;
					}
					src++;
				}
				g_free(temp);
			}
			line--;
		}
	}
	
	return -1;
}