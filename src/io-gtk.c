/* Copywrite 1997 Free Software Foundation, Inc.

This file is part of Oleo, the GNU Spreadsheet.

Oleo is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later verion.

oleo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Oleo; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBGTK
#include "gtk/gtk.h"
#include "io-abstract.h"
#include "io-gtk.h"

static GdkPixmap *pixmap = NULL;

void
gio_destroy (void)
{
	gtk_exit(0);
}

void
gio_redraw_input (GtkWidget *widget)
{
	gdk_draw_string(widget->window,
			widget->style->font,
			widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			0,
			10,
			"Hi\0");
}

gint
gio_expose_backing (GtkWidget *widget, GdkEventExpose *event)
{
	gdk_draw_pixmap(widget->window,
			widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			pixmap,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);

	gio_redraw_input((GtkWidget*)widget);

	return FALSE;
}

static gint
gio_configure_backing (GtkWidget *widget, GdkEventConfigure *event)
{
	if (pixmap) {
		gdk_pixmap_destroy (pixmap);
	}
	
	pixmap = gdk_pixmap_new(widget->window,
				widget->allocation.width,
				widget->allocation.height,
				-1);

	gdk_draw_rectangle (pixmap,
			    widget->style->white_gc,
			    TRUE,
			    0, 0,
			    widget->allocation.width,
			    widget->allocation.height);

	return TRUE;
}

void
gio_open_display(void)
{
	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *menu_bar;
	GtkWidget *root_menu;
	GtkWidget *menu_items;
	GtkWidget *box1;
	GtkWidget *box2;
	GtkWidget *box3;
	GtkWidget *scrollbar;
	GtkWidget *drawing_area;
	GtkObject *adjustment;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW (window), GNU_PACKAGE);
	gtk_container_border_width (GTK_CONTAINER (window), 0);
	gtk_signal_connect (GTK_OBJECT (window), "destroy",
			    GTK_SIGNAL_FUNC (gio_destroy), NULL);

	box1 = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), box1);
	gtk_widget_show (box1);

	box2 = gtk_vbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (box2), 0);
	gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
	gtk_widget_show (box2);

	menu_bar = gtk_menu_bar_new ();
	gtk_box_pack_start (GTK_BOX (box2), menu_bar, FALSE, TRUE, 0);
	gtk_widget_show (menu_bar);

	menu = gtk_menu_new ();
	root_menu = gtk_menu_item_new_with_label("File");
	gtk_widget_show (root_menu);
	menu_items = gtk_menu_item_new_with_label("New");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Open");
	gtk_menu_append(GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Close");
	gtk_menu_append(GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Save");
	gtk_menu_append(GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Save As");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Print");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label("Exit");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
	gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), root_menu);

	menu = gtk_menu_new ();
	root_menu = gtk_menu_item_new_with_label ("Edit");
	gtk_widget_show (root_menu);
	menu_items = gtk_menu_item_new_with_label ("Copy");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label ("Paste");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show(menu_items);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
	gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), root_menu);

	menu = gtk_menu_new ();
	root_menu = gtk_menu_item_new_with_label ("Help");
	gtk_widget_show (root_menu);
	menu_items = gtk_menu_item_new_with_label ("Help");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	menu_items = gtk_menu_item_new_with_label ("About");
	gtk_menu_append (GTK_MENU (menu), menu_items);
	gtk_widget_show (menu_items);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (root_menu), menu);
	gtk_menu_item_right_justify (GTK_MENU_ITEM (root_menu));
	gtk_menu_bar_append (GTK_MENU_BAR (menu_bar), root_menu);

	box3 = gtk_hbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (box3), 0);
	gtk_box_pack_start (GTK_BOX (box2), box3, TRUE, TRUE, 0);
	gtk_widget_show (box3);

	drawing_area = gtk_drawing_area_new ();
	gtk_drawing_area_size (GTK_DRAWING_AREA (drawing_area), 675, 350);
	gtk_box_pack_start (GTK_BOX (box3), drawing_area, TRUE, TRUE, 0);
	gtk_widget_show (drawing_area);
	
	gtk_signal_connect (GTK_OBJECT (drawing_area), "configure_event",
			    (GtkSignalFunc) gio_configure_backing, NULL);
	gtk_signal_connect (GTK_OBJECT (drawing_area), "expose_event",
			    (GtkSignalFunc) gio_expose_backing, NULL);

	gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			       | GDK_PROXIMITY_OUT_MASK);

	adjustment = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
	scrollbar = gtk_vscrollbar_new (GTK_ADJUSTMENT (adjustment));
	gtk_range_set_update_policy (GTK_RANGE (scrollbar), GTK_UPDATE_CONTINUOUS);
	gtk_box_pack_start (GTK_BOX (box3), scrollbar, FALSE, FALSE, 0);
	gtk_widget_show (scrollbar);

	adjustment = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
	scrollbar = gtk_hscrollbar_new (GTK_ADJUSTMENT (adjustment));
	gtk_range_set_update_policy (GTK_RANGE (scrollbar), GTK_UPDATE_CONTINUOUS);
	gtk_box_pack_start (GTK_BOX (box2), scrollbar, FALSE, FALSE, 0);
	gtk_widget_show (scrollbar);

	gtk_widget_show (window);

	gtk_main ();

	exit (0);
}

void
gio_inputize_cursor(void)
{
}

void
gio_cellize_cursor(void)
{
}

void
gio_io_hide_cell_cursor(void)
{
}

void
gio_pr_cell_win(void)
{
}

void
gio_clear_input_after(void)
{
}

void
gio_clear_input_before(void)
{
}

void
gio_flush(void)
{
}

void
gio_over(void)
{
}

void
gio_insert(void)
{
}

void
gio_erase(void)
{
}

void
gio_move_cursor(void)
{
}

void
gio_fix_input(void)
{
}

void
gio_update_status(void)
{
}

void
gio_get_chr(void)
{
}

void
gio_bell(void)
{
}

void
gio_getch(void)
{
}

void
gio_nodelay(void)
{
}

void
gio_read_kbd(void)
{
}

void
gio_wait_for_input(void)
{
}

void
gio_scan_for_input(void)
{
}

void
gio_input_avail(void)
{
}

void
gio_close_display(void)
{
}

void
gio_repaint_win(void)
{
}

void
gio_repaint(void)
{
}

void
gio_redisp(void)
{
}

void
gio_display_cell_cursor(void)
{
}

void
gtk_graphics(void)
{
	IO_SETUP;
}

#endif /* Have HAVE_LIBGTK */

