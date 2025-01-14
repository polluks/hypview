/*
 * HypView - (c)      - 2019 Thorsten Otto
 *
 * A replacement hypertext viewer
 *
 * This file is part of HypView.
 *
 * HypView is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HypView is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HypView; if not, see <http://www.gnu.org/licenses/>.
 */

#include "hv_gtk.h"

#ifdef HAVE_PNG /* almost whole file */

#undef G_ICON /* conflict from gio */
#include <gem.h>
#include <errno.h>
#include <ctype.h>
#include "stat_.h"
#include "portvdi.h"
#include "../rsc/src/fileio.h"
#include "rsc.h"
#include "nls.h"
#include "ws.h"
#include "debug.h"
#ifdef _WIN32
#include <direct.h>
#endif
#include "picture.h"


static _WORD gl_hchar;
static _WORD gl_wchar;
static _WORD gl_wbox;
static _WORD gl_hbox;							/* system sizes */
static GRECT desk, screen;
static _WORD phys_handle;						/* physical workstation handle */
static _WORD vdi_handle;						/* virtual screen handle */
static WS ws;
static _WORD xworkout[57];

/*
 * program options
 */
#if 0
static _BOOL use_timestamps = TRUE;
static const char *pngdir;
#endif

typedef struct {
	WINDOW_DATA *parent;
	RSCFILE *file;
	RSCTREE *rsctree;
	_UWORD treenr;
	DOCUMENT *doc;
	HYP_DOCUMENT *hyp;
	HYP_NODE *node;
	GtkWidget *dialog;
} IMAGE_INFO;

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

void err_fcreate(const char *filename)
{
	char *str = g_strdup_printf(_("Can't create %s:\n%s"), filename, hyp_utf8_strerror(errno));
	show_message(NULL, _("Error"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void err_fopen(const char *filename)
{
	char *str = g_strdup_printf(_("Can't open %s:\n%s"), filename, hyp_utf8_strerror(errno));
	show_message(NULL, _("Error"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void err_fread(const char *filename)
{
	char *str = g_strdup_printf(_("Error reading %s:\n%s"), filename, hyp_utf8_strerror(errno));
	show_message(NULL, _("Error"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void err_fwrite(const char *filename)
{
	char *str = g_strdup_printf(_("Error writing %s:\n%s"), filename, hyp_utf8_strerror(errno));
	show_message(NULL, _("Error"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void err_rename(const char *oldname, const char *newname)
{
	char *str = g_strdup_printf(_("Can't rename %s to %s:\n%s"), oldname, newname, hyp_utf8_strerror(errno));
	show_message(NULL, _("Error"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void err_nota_rsc(const char *filename)
{
	char *str = g_strdup_printf(_("Not a resource file:\n%s"), filename);
	show_message(NULL, _("Error"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void warn_damaged(const char *filename, const char *where)
{
	char *str = g_strdup_printf(_("problems in %s while scanning %s"), filename, where);
	show_message(NULL, _("Warning"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void warn_cicons(void)
{
	show_message(NULL, _("Warning"), _("I couldn't find any color icons although\nthe flag is set the header"), FALSE);
}

/*** ---------------------------------------------------------------------- ***/

void warn_crc_mismatch(const char *filename, RSC_RSM_CRC header_crc, RSC_RSM_CRC file_crc)
{
	char *str = g_strdup_printf(_("%s: CRC $%04x does not match resource file $%04x"), filename, header_crc, file_crc);
	show_message(NULL, _("Warning"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void warn_crc_string_mismatch(const char *filename)
{
	char *str = g_strdup_printf(_("%s: embedded string CRC does not match resource file"), filename);
	show_message(NULL, _("Warning"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void warn_def_damaged(const char *filename)
{
	char *str = g_strdup_printf(_("%s: illegal definition file"), filename);
	show_message(NULL, _("Warning"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void warn_names_truncated(_WORD maxlen)
{
	char *str = g_strdup_printf(_("Names truncated (maxlen = %d)"), maxlen);
	show_message(NULL, _("Warning"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

void warn_interface_flags(const char *filename)
{
	char *str = g_strdup_printf(_("%s: some flags have been interpreted as being written by INTRFACE"), filename);
	show_message(NULL, _("Warning"), str, FALSE);
	g_free(str);
}

/*** ---------------------------------------------------------------------- ***/

_BOOL ask_tree_notfound(_WORD trindex)
{
	char *str = g_strdup_printf(_("Tree %d not found.\nContinue?"), trindex);
	_BOOL ret = ask_yesno(top_window(), str);
	g_free(str);
	return ret;
}

/*** ---------------------------------------------------------------------- ***/

_BOOL ask_object_notfound(_LONG ob_index, char *tree_name)
{
	char *str = g_strdup_printf(_("No object #%ld in tree %s.\nContinue?"), ob_index, tree_name);
	_BOOL ret = ask_yesno(top_window(), str);
	g_free(str);
	return ret;
}

/*** ---------------------------------------------------------------------- ***/

void warn_rso_toonew(void)
{
	show_message(NULL, _("Warning"), _("RSO-File created by newer Version of ORCS"), FALSE);
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static void open_screen(void)
{
	int i;
	_WORD pxy[8];
	_WORD workin[11];

	vdi_handle = phys_handle;
	for (i = 0; i < 10; i++)
		workin[i] = 1;
	workin[10] = 2;
	v_opnvwk(workin, &vdi_handle, &ws.ws_xres);
	vq_extnd(vdi_handle, 1, xworkout);
	vsf_interior(vdi_handle, FIS_SOLID);
	vsf_perimeter(vdi_handle, FALSE);
	vswr_mode(vdi_handle, MD_REPLACE);
	vsf_color(vdi_handle, G_GREEN);
	
	pxy[0] = 0;
	pxy[1] = 0;
	pxy[2] = ws.ws_xres;
	pxy[3] = ws.ws_yres;
	vr_recfl(vdi_handle, pxy);

	vsf_color(vdi_handle, G_WHITE);
}

/* ------------------------------------------------------------------------- */

static void close_screen(void)
{
	v_clsvwk(vdi_handle);
}

/* ------------------------------------------------------------------------- */

static void clear_screen(char *title)
{
	static char empty[1] = { 0 };
	static TEDINFO tedinfo = {
		NULL, empty, empty, IBM, 1, TE_CNTR, 0x1100, 0x0, 1, 2,1
	};
	static OBJECT desktop[] = {
		{ NIL, 1, 2, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, 0, G_BLACK, G_BLACK, TRUE, IP_SOLID, G_GREEN) }, 0, 0, 0, 0 },
		{ 2, NIL, NIL, G_BOX, OF_NONE, OS_NORMAL, { OBSPEC_MAKE(0, -1, G_BLACK, G_BLACK, FALSE, IP_HOLLOW, G_WHITE) }, 0, 0, 0, 0 },
		{ 0, NIL, NIL, G_TEXT, OF_LASTOB, OS_NORMAL, { (_LONG_PTR)&tedinfo }, 0, 0, 0, 0 },
	};
	int i;
	
	for (i = 0; i < 3; i++)
		desktop[i].ob_width = desk.g_x + desk.g_w;
	desktop[ROOT].ob_height = desk.g_y + desk.g_h;
	desktop[1].ob_height = gl_hchar + 2;
	desktop[2].ob_height = gl_hchar + 3;
	tedinfo.te_ptext = title;
	
	objc_draw(desktop, ROOT, MAX_DEPTH, 0, 0, desk.g_x + desk.g_w, desk.g_y + desk.g_h);
}

/* ------------------------------------------------------------------------- */

#if 0
static void str_lwr(char *name)
{
	while (*name)
	{
		*name = tolower(*name);
		name++;
	}
}

/* ------------------------------------------------------------------------- */

static _WORD write_png(RSCTREE *tree, _WORD x, _WORD y, _WORD w, _WORD h)
{
	_WORD pxy[4];
	char basename[PATH_MAX];
	char filename[PATH_MAX];
	_WORD err;
	char *p;
	
	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = x + w - 1;
	pxy[3] = y + h - 1;
	vs_clip(vdi_handle, 1, pxy);
	if (use_timestamps)
	{
		time_t t = time(NULL);
		struct tm *tp = gmtime(&t);
		sprintf(basename, "%s_%04d%02d%02d%02d%02d%02d",
			tree->rt_name,
			tp->tm_year + 1900,
			tp->tm_mon + 1,
			tp->tm_mday,
			tp->tm_hour,
			tp->tm_min,
			tp->tm_sec);
	} else
	{
		strcpy(basename, tree->rt_name);
	}
	str_lwr(basename);
	p = filename;
	if (pngdir)
	{
		int len;
		
#ifdef _WIN32
		(void) _mkdir(pngdir);
#else
		(void) mkdir(pngdir, 0755);
#endif
		strcpy(p, pngdir);
		len = strlen(p);
		p += len;
		if (len > 0 && p[-1] != '/')
			*p++ = '/';
	}
	sprintf(p, "%03ld_%s.png", tree->rt_number, basename);
	err = v_write_png(vdi_handle, filename);
	if (err != 0)
	{
		KINFO(("write_png: %s: %s\n", filename, strerror(err)));
	}
	return err;
}
#endif

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static void check_parent(IMAGE_INFO *info)
{
	GList *tops, *l;

	if (info->parent == NULL)
		return;
	tops = gtk_window_list_toplevels();
	for (l = tops; l != NULL; l = l->next)
	{
		if (l->data == info->parent)
		{
			g_list_free(tops);
			return;
		}
	}
	g_list_free(tops);
	info->parent = NULL;
}

static void image_destroyed(GtkWidget *w, gpointer user_data)
{
	IMAGE_INFO *info = (IMAGE_INFO *)user_data;
	GList *tops, *l;
	
	UNUSED(w);
	rsc_file_delete(info->file, FALSE);
	xrsrc_free(info->file);
	hypdoc_unref(info->doc);

	tops = gtk_window_list_toplevels();
	for (l = tops; l != NULL; l = l->next)
	{
		if (info->parent && l->data == info->parent)
		{
			info->parent->rscfile = NULL;
		}
	}
	g_list_free(tops);
	g_free(info);
	check_toplevels(NULL);
}

/*** ---------------------------------------------------------------------- ***/

static GdkPixbuf *get_pixbuf(GRECT *gr)
{
	int width = gr->g_w;
	int height = gr->g_h;
	GdkPixbuf *pixbuf;
	guint8 *dst;
	int dststride;
	_WORD pxy[4];
	
	pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
	if (pixbuf == NULL)
	{
		return NULL;
	}
	
	dststride = gdk_pixbuf_get_rowstride(pixbuf);
	dst = gdk_pixbuf_get_pixels(pixbuf);
	pxy[0] = gr->g_x;
	pxy[1] = gr->g_y;
	pxy[2] = gr->g_x + gr->g_w - 1;
	pxy[3] = gr->g_y + gr->g_h - 1;

	v_hardcopy_ex(vdi_handle, pxy, PX_RGBA, dststride, dst);
	
	return pixbuf;
}

/*** ---------------------------------------------------------------------- ***/

static gboolean image_clicked(GtkWidget *w, GdkEventButton *event, gpointer user_data)
{
	IMAGE_INFO *info = (IMAGE_INFO *)user_data;
	OBJECT *tree;
	_WORD obj;
	hyp_nodenr dest;
	hyp_lineno line;
	gint x, y;
	GdkModifierType mask;

	UNUSED(w);
	if (event->type != GDK_BUTTON_PRESS || event->button != GDK_BUTTON_PRIMARY)
		return FALSE;

	check_parent(info);
	if (info->parent && info->parent->popup)
		gtk_widget_destroy(GTK_WIDGET(info->parent->popup));

	tree = info->rsctree->rt_objects.dial.di_tree;
		
	obj = objc_find(tree, ROOT, MAX_DEPTH, (int)event->x, (int)event->y);
	if (obj < 0)
	{
		gtk_widget_destroy(info->dialog);
		return FALSE;
	}
	dest = hyp_node_find_objref(info->node, info->treenr, obj, &line);
	if (dest == HYP_NOINDEX)
	{
		if (!ask_yesno(info->dialog, _("No node defined for this object.\nContinue?")))
		{
			gtk_widget_destroy(info->dialog);
			return FALSE;
		}
	} else
	{
		HYP_DOCUMENT *hyp = info->hyp;
		hyp_indextype dst_type = (hyp_indextype)hyp->indextable[dest]->type;
		
		switch (dst_type)
		{
		case HYP_NODE_POPUP:
			gdk_display_get_pointer(gtk_widget_get_display(info->dialog), NULL, &x, &y, &mask);
			if (info->parent && info->parent->data == info->doc)
				OpenPopup(info->parent, dest, line, x, y);
			break;
		case HYP_NODE_INTERNAL:
			if (info->parent && info->parent->data == info->doc)
			{
				AddHistoryEntry(info->parent, info->doc);
				GotoPage(info->parent, dest, line, TRUE);
			}
			break;
		case HYP_NODE_EXTERNAL_REF:
			if (info->parent)
			{
				char *name = hyp_conv_to_utf8(hyp->comp_charset, hyp->indextable[dest]->name, STR0TERM);
				HypOpenExtRef(info->parent, name, line, FALSE);
				g_free(name);
			}
			break;
		case HYP_NODE_REXX_COMMAND:
		case HYP_NODE_SYSTEM_ARGUMENT:
		case HYP_NODE_REXX_SCRIPT:
		case HYP_NODE_QUIT:
		case HYP_NODE_CLOSE:
		case HYP_NODE_IMAGE:
		case HYP_NODE_EOF:
		default:
			{
				char *str = g_strdup_printf(_("Link to node of type %u not implemented."), dst_type);
				show_message(info->dialog, _("Error"), str, FALSE);
				g_free(str);
			}
			break;
		}
	}
	return TRUE;
}

/*** ---------------------------------------------------------------------- ***/

static void show_image(WINDOW_DATA *parent, RSCFILE *file, RSCTREE *tree, _UWORD treenr, GRECT *gr)
{
	GtkWidget *dialog, *vbox, *hbox, *image, *event_box;
	GtkWidget *scroll;
	char *title;
	gboolean big = FALSE;
	GdkPixbuf *pixbuf;
	DOCUMENT *doc;
	HYP_DOCUMENT *hyp;
	IMAGE_INFO *info;

	if (parent == NULL)
		return;
	if (parent->rscfile)
		gtk_widget_destroy(parent->rscfile);
	
	doc = parent->data;
	hyp = (HYP_DOCUMENT *)doc->data;

	info = g_new(IMAGE_INFO, 1);
	if (info == NULL)
		return;
	
	hypdoc_ref(doc);

	info->parent = parent;
	info->file = file;
	info->rsctree = tree;
	info->treenr = treenr;
	info->doc = doc;
	info->hyp = hyp;
	info->node = parent->displayed_node;
	
	dialog = info->dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	parent->rscfile = dialog;
	gtk_window_set_screen(GTK_WINDOW(dialog), gtk_widget_get_screen(GTK_WIDGET(parent)));
	g_object_set_data(G_OBJECT(dialog), "hypview_window_type", NO_CONST("resource"));
	g_signal_connect(G_OBJECT(dialog), "destroy", G_CALLBACK(image_destroyed), info);
	title = g_strdup_printf("%s/%u", rsc_basename(file->rsc_rsxname), treenr);
	gtk_window_set_title(GTK_WINDOW(dialog), title);
	g_free(title);
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), gl_profile.viewer.text_yoffset);
	gtk_container_add(GTK_CONTAINER(dialog), vbox);
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), gl_profile.viewer.text_xoffset);
	gtk_container_add(GTK_CONTAINER(vbox), hbox);

	pixbuf = get_pixbuf(gr);
	
	image = gtk_image_new_from_pixbuf(pixbuf);
	gdk_pixbuf_unref(pixbuf);
	event_box = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(event_box), image);
	gtk_widget_add_events(event_box, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
	g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(image_clicked), info);
	g_signal_connect(G_OBJECT(event_box), "button-release-event", G_CALLBACK(image_clicked), info);
	
	if (big)
	{
		scroll = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), event_box);
		gtk_box_pack_start(GTK_BOX(hbox), scroll, TRUE, TRUE, 0);
		gtk_widget_set_usize(image, gr->g_w, gr->g_h);
	} else
	{
		gtk_box_pack_start(GTK_BOX(hbox), event_box, TRUE, TRUE, 0);
	}
	
	if (big && 0)
		gtk_window_set_default_size(GTK_WINDOW(dialog), 640, 400);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
	gtk_widget_show_all(dialog);
	gtk_window_present(GTK_WINDOW(dialog));
}

/*****************************************************************************/
/* ------------------------------------------------------------------------- */
/*****************************************************************************/

static gboolean draw_dialog(WINDOW_DATA *parent, RSCFILE *file, RSCTREE *tree, _UWORD treenr)
{
	OBJECT *ob;
	GRECT gr, draw_gr;
	
	ob = tree->rt_objects.dial.di_tree;
	if (ob == NULL)
		return FALSE;
	form_center_grect(ob, &gr);

	wind_update(BEG_UPDATE);
	form_dial_grect(FMD_START, &gr, &gr);
	
	clear_screen(tree->rt_name);
	
	objc_draw_grect(ob, ROOT, MAX_DEPTH, &gr);
	
	/*
	 * now move it to the top-left corner,
	 * to simplify objc_find() later
	 */
	ob[ROOT].ob_x -= gr.g_x;
	ob[ROOT].ob_y -= gr.g_y;

#if 0
	err = write_png(tree, gr.g_x, gr.g_y, gr.g_w, gr.g_h);
#else
	draw_gr = gr;
	show_image(parent, file, tree, treenr, &draw_gr);
#endif

	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);
	
	return TRUE;
}

/* ------------------------------------------------------------------------- */

static gboolean draw_menu(WINDOW_DATA *parent, RSCFILE *file, RSCTREE *tree, _UWORD treenr)
{
	OBJECT *ob;
	_WORD thebar;
	_WORD theactive;
	_WORD themenus;
	_WORD title, menubox;
	_WORD x;
	GRECT gr, draw_gr;
	_WORD maxx, maxy;
	
	ob = tree->rt_objects.menu.mn_tree;
	if (ob == NULL)
		return FALSE;
	/*
	 * just in case, put the menu at top-left corner
	 */
	ob[ROOT].ob_x = 0;
	ob[ROOT].ob_y = 0;

	/*
	 * set the width of the root object, the titlebar, and the background box
	 * to screen width
	 */
	ob[ROOT].ob_width = ob[menu_the_bar(ob)].ob_width = ob[menu_the_menus(ob)].ob_width = desk.g_x + desk.g_w;
	/*
	 * adjust the height of the root object and the
	 * background box to fill up the screen
	 */
	ob[ROOT].ob_height = desk.g_y + desk.g_h;
	ob[menu_the_menus(ob)].ob_height = ob[ROOT].ob_height - ob[menu_the_menus(ob)].ob_y;

	objc_offset(ob, ROOT, &gr.g_x, &gr.g_y);
	gr.g_w = ob[ROOT].ob_width;
	gr.g_h = ob[ROOT].ob_height;

	wind_update(BEG_UPDATE);
	
	/*
	 * draw the menu titles
	 */
	clear_screen(tree->rt_name);
	menu_bar(ob, TRUE);

	/*
	 * reposition the submenu boxes so that they don't overlap
	 */
	thebar = menu_the_bar(ob);
	if (thebar == NIL)
		return FALSE;
	themenus = ob[thebar].ob_next;
	if (themenus == thebar)
		return FALSE;
	theactive = ob[thebar].ob_head;
	if (theactive == NIL)
		return FALSE;
	title = ob[theactive].ob_head;
	if (title == NIL)
		return FALSE;
	menubox = ob[themenus].ob_head;
	if (menubox == NIL)
		return FALSE;
	x = ob[menubox].ob_x;
	do
	{
		ob[menubox].ob_x = x;
		/* ob[title].ob_x = x + gl_wchar; */
		x += ob[menubox].ob_width + 1;
		title = ob[title].ob_next;
		menubox = ob[menubox].ob_next;
	} while (title != theactive && menubox != themenus);
	
	/*
	 * draw the boxes
	 */
	maxx = maxy = 0;
	menubox = ob[themenus].ob_head;
	do
	{
		_WORD mx, my, mw, mh;
		
		objc_offset(ob, menubox, &mx, &my);
		mw = ob[menubox].ob_width;
		mh = ob[menubox].ob_height;
		mx -= 1;
		my -= 1;
		mw += 2;
		mh += 2;
		objc_draw(ob, menubox, MAX_DEPTH, mx, my, mw, mh);
		menubox = ob[menubox].ob_next;
		mx = mx + mw;
		my = my + mh;
		if (mx > maxx)
			maxx = mx;
		if (my > maxy)
			maxy = my;
	} while (menubox != themenus);
	
#if 0
	err = write_png(tree, 0, 0, maxx, maxy);
#else
	draw_gr.g_x = 0;
	draw_gr.g_y = 0;
	draw_gr.g_w = maxx;
	draw_gr.g_h = maxy;
	show_image(parent, file, tree, treenr, &draw_gr);
#endif

	menu_bar(ob, FALSE);
	form_dial_grect(FMD_FINISH, &gr, &gr);
	wind_update(END_UPDATE);
	
	return TRUE;
}

/* ------------------------------------------------------------------------- */

void ShowResource(WINDOW_DATA *win, const char *path, _UWORD treenr)
{
	char *dir;
	char *filename;
	DOCUMENT *doc = win->data;
	RSCFILE *file;
	_UWORD load_flags = XRSC_SAFETY_CHECKS;

	dir = hyp_path_get_dirname(doc->path);
	filename = g_build_filename(dir, path, NULL);

	appl_init();
	
	menu_register(-1, gl_program_name);
	phys_handle = graf_handle(&gl_wchar, &gl_hchar, &gl_wbox, &gl_hbox);
	wind_get(DESK, WF_WORKXYWH, &desk.g_x, &desk.g_y, &desk.g_w, &desk.g_h);
	wind_get(DESK, WF_CURRXYWH, &screen.g_x, &screen.g_y, &screen.g_w, &screen.g_h);

	file = load_all(filename, gl_wchar, gl_hchar, load_flags);
	if (file != NULL)
	{
		RSCTREE *tree;
		
		tree = rsc_tree_index(file, treenr, RT_DIALOG);
		if (tree == NULL)
		{
			char *str = g_strdup_printf(_("Tree %u not found!"), treenr);
			show_message(GTK_WIDGET(win), _("Error"), str, FALSE);
			g_free(str);
		
			rsc_file_delete(file, FALSE);
			xrsrc_free(file);
		} else
		{
			open_screen();
			vst_font(vdi_handle, file->fontset);
			vst_font(phys_handle, file->fontset);
	
			if (tree->rt_type == RT_MENU)
				draw_menu(win, file, tree, treenr);
			else
				draw_dialog(win, file, tree, treenr);
			
			vst_font(phys_handle, 1);
			close_screen();
		}
	}
	
	appl_exit();
}

#else

void ShowResource(WINDOW_DATA *win, const char *path, _UWORD treenr)
{
	char *str = g_strdup_printf(_("Display of resource files not available."));
	UNUSED(path);
	UNUSED(treenr);
	show_message(GTK_WIDGET(win), _("Error"), str, FALSE);
	g_free(str);
}

#endif /* HAVE_PNG */
