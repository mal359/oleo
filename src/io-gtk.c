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

void
gio_main(void)
{
	GtkWidget *window;
	GtkWidget *menu;
	GtkWidget *menu_bar;
	GtkWidget *root_menu;
	GtkWidget *menu_items;
	GtkWidget *box1;
	GtkWidget *box2;
	GtkWidget *scrollbar;
	GtkObject *adjustment;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW (window), GNU_PACKAGE);
	gtk_container_border_width (GTK_CONTAINER (window), 0);

	box1 = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (window), box1);
	gtk_widget_show (box1);

	box2 = gtk_vbox_new (FALSE, 10);
	gtk_container_border_width (GTK_CONTAINER (box2), 10);
	gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
	gtk_widget_show (box2);

	menu_bar = gtk_menu_bar_new ();
	gtk_box_pack_start (GTK_BOX (box2), menu_bar, FALSE, TRUE, 0);
	gtk_widget_show (menu_bar);

	menu = gtk_menu_new ();
	root_menu = gtk_menu_item_new_with_label("File");
	gtk_widget_show (root_menu);
	menu_items = gtk_menu_item_new_with_label("Open");
	gtk_menu_append(GTK_MENU (menu), menu_items);
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

	adjustment = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
	scrollbar = gtk_vscrollbar_new (GTK_ADJUSTMENT (adjustment));
	gtk_range_set_update_policy (GTK_RANGE (scrollbar), GTK_UPDATE_CONTINUOUS);
	gtk_box_pack_start (GTK_BOX (box2), scrollbar, TRUE, TRUE, 0);
	gtk_widget_show (scrollbar);

	adjustment = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
	scrollbar = gtk_hscrollbar_new (GTK_ADJUSTMENT (adjustment));
	gtk_range_set_update_policy (GTK_RANGE (scrollbar), GTK_UPDATE_CONTINUOUS);
	gtk_box_pack_start (GTK_BOX (box2), scrollbar, TRUE, TRUE, 0);
	gtk_widget_show (scrollbar);

	gtk_widget_show (window);

	gtk_main ();

	exit (0);
}
#endif /* Have HAVE_LIBGTK */
