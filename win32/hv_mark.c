#include "hv_defs.h"
#include "hypdebug.h"


#define MAX_MARKEN		10
#define UNKNOWN_LEN		10
#define PATH_LEN		128
#define NODE_LEN		40


typedef struct
{
	hyp_nodenr node_num;
	short line;
	char unknown[UNKNOWN_LEN];
	char path[PATH_LEN];				/* full path */
	char node_name[NODE_LEN];			/* display title */
} MARKEN;

static gboolean marken_change;
static MARKEN marken[MAX_MARKEN];

/******************************************************************************/
/*** ---------------------------------------------------------------------- ***/
/******************************************************************************/

static void MarkerDelete(short num)
{
	char *dst;
	
	memset(&marken[num], 0, sizeof(MARKEN));
	marken[num].node_num = HYP_NOINDEX;
	dst = marken[num].node_name;
	strcpy(dst, _("free"));
}

/*** ---------------------------------------------------------------------- ***/

void MarkerSave(WINDOW_DATA *win, short num)
{
	DOCUMENT *doc = win->data;
	const char *src;
	char *dst, *end;

	/* avoid illegal parameters */
	if (num < 0 || num >= MAX_MARKEN)
		return;

	marken[num].node_num = doc->getNodeProc(win);
	marken[num].line = hv_win_topline(win);
	strncpy(marken[num].path, doc->path, PATH_LEN - 1);
	marken[num].path[PATH_LEN - 1] = 0;

	/* copy marker title */
	src = win->title;
	dst = marken[num].node_name;
	end = &marken[num].node_name[NODE_LEN - 1];
	while (dst < end)
	{
		if (*src)
			*dst++ = *src++;
		else
			break;
	}
	if (dst < end)
		*dst++ = ' ';
	if (dst < end)
		*dst++ = ' ';
	*dst = 0;

	{
		char ZStr[255];
		long len;

		strcpy(ZStr, "(");
		src = hyp_basename(marken[num].path);
		strcat(ZStr, src);
		strcat(ZStr, ") ");
		len = strlen(ZStr);
		if (strlen(marken[num].node_name) + len > NODE_LEN)
			marken[num].node_name[NODE_LEN - len] = '\0';
		strcat(marken[num].node_name, ZStr);
	}

	marken_change = TRUE;
}

/*** ---------------------------------------------------------------------- ***/

void MarkerShow(WINDOW_DATA *win, short num, gboolean new_window)
{
	/* avoid illegal parameters */
	if (num < 0 || num >= MAX_MARKEN)
		return;

	if (marken[num].node_num != HYP_NOINDEX)
	{
		win = OpenFileInWindow(win, marken[num].path, NULL, marken[num].node_num, TRUE, new_window, FALSE);
		if (win != NULL)
		{
			GotoPage(win, marken[num].node_num, marken[num].line, FALSE);
		}
	}
}

/*** ---------------------------------------------------------------------- ***/

void on_bookmark_selected(WINDOW_DATA *win, int sel)
{
	gboolean shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	gboolean alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
	gboolean ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
	
	if (sel >= 0 && sel < MAX_MARKEN)
	{
		if (shift)
		{
			MarkerSave(win, sel);
		} else if (marken[sel].node_num == HYP_NOINDEX)
		{
			char *buff;

			buff = g_strdup_printf(_("Do you want to add\n%s\nto your bookmarks?"), win->title);
			if (ask_yesno(win ? win->hwnd : NULL, buff))
				MarkerSave(win, sel);
			g_free(buff);
		} else
		{
			if (alt)
			{
				char *buff;

				buff = g_strdup_printf(_("Do you want to remove\n%s\nfrom your bookmarks?"), marken[sel].node_name);
				if (ask_yesno(win ? win->hwnd : NULL, buff))
				{
					MarkerDelete(sel);
					marken_change = TRUE;
				}
				g_free(buff);
			} else
			{
				MarkerShow(win, sel, ctrl);
			}
		}
	}
}

/*** ---------------------------------------------------------------------- ***/

void MarkerUpdate(WINDOW_DATA *win)
{
	UNUSED(win);
	/* NYI */
}

/*** ---------------------------------------------------------------------- ***/

void MarkerPopup(WINDOW_DATA *win, int button)
{
	HMENU menu;
	struct popup_pos popup_pos;
	int x, y;
	
	if (!win->m_buttons[TO_BOOKMARKS])
		return;
	
	MarkerUpdate(win);
	menu = win->bookmarks_menu;
	
	popup_pos.window = win;
	popup_pos.obj = TO_BOOKMARKS;
	position_popup(menu, &popup_pos, &x, &y);
	/* NYI */
	UNUSED(button);
}

/*** ---------------------------------------------------------------------- ***/

void MarkerSaveToDisk(void)
{
	char *filename;
	
	if (!marken_change)
		return;

	if (!empty(gl_profile.viewer.marker_path))
	{
		int ret;

		if (gl_profile.viewer.marken_save_ask)
		{
			if (ask_yesno(top_window(), _("Save bookmarks?")) == FALSE)
				return;
		}
		filename = path_subst(gl_profile.viewer.marker_path);
		ret = open(filename, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY, 0644);
		if (ret >= 0)
		{
			write(ret, marken, sizeof(MARKEN) * MAX_MARKEN);
			close(ret);
		} else
		{
			HYP_DBG(("Error %ld: saving %s", ret, printnull(filename)));
		}
		g_free(filename);
	}
}

/*** ---------------------------------------------------------------------- ***/

void MarkerInit(void)
{
	short i;
	int ret;
	char *filename;
	
	/* initialize markers */
	for (i = 0; i < MAX_MARKEN; i++)
	{
		MarkerDelete(i);
	}

	/* load file if it exists */
	if (!empty(gl_profile.viewer.marker_path))
	{
		filename = path_subst(gl_profile.viewer.marker_path);
		ret = open(filename, O_RDONLY | O_BINARY);
		if (ret >= 0)
		{
			read(ret, marken, sizeof(MARKEN) * MAX_MARKEN);
			for (i = 0; i < MAX_MARKEN; i++)
			{
				if (marken[i].node_name[0] == 0)
					MarkerDelete(i);
			}
			close(ret);
		}
		g_free(filename);
	}

	marken_change = FALSE;
}