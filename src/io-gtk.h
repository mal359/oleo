#ifndef IO_X11H
#define IO_X11H

#ifdef HAVE_LIBGTK

#define _io_open_display gio_open_display
#define _io_redisp gio_redisp
#define _io_repaint gio_repaint
#define _io_repaint_win gio_repaint_win
#define _io_close_display gio_close_display
#define _io_input_avail gio_input_avail
#define _io_scan_for_input gio_scan_for_input
#define _io_wait_for_input gio_wait_for_input
#define _io_read_kbd gio_read_kbd
#define _io_nodelay gio_nodelay
#define _io_getch gio_getch
#define _io_bell gio_bell
#define _io_get_chr gio_get_chr
#define _io_update_status gio_update_status
#define _io_info_msg gio_info_msg
#define _io_error_msg gio_error_msg
#define _io_fix_input gio_fix_input
#define _io_move_cursor gio_move_cursor
#define _io_erase gio_erase
#define _io_insert gio_insert
#define _io_over gio_over
#define _io_flush gio_flush
#define _io_clear_input_before gio_clear_input_before
#define _io_clear_input_after gio_clear_input_after
#define _io_pr_cell_win gio_pr_cell_win
#define _io_hide_cell_cursor gio_io_hide_cell_cursor
#define _io_display_cell_cursor gio_display_cell_cursor
#define _io_cellize_cursor gio_cellize_cursor
#define _io_inputize_cursor gio_inputize_cursor

#endif /* HAVE_LIBGTK */

#endif /* IO_X11H */
