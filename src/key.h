#ifndef KEYH
#define KEYH
/*
 * $Id: key.h,v 1.8 2000/08/10 21:02:50 danny Exp $
 *
 * Copyright © 1993 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "global.h"
#include "line.h"

/*
 * Keymaps and keybinds.
 * Within a keymap, keys are bound to a `struct key'.
 *
 * Normally,  `the_funcs[akey.vector][akey.code]' is the command binding of a
 * key.  However, the akey.vector == -1, then the binding is to another
 * keymap, found by the_maps[akey.code].  The final exception, is that if
 * both vector and code are -1, then the key is unbound.
 */

struct cmd_func;

struct key
{
  short vector;
  short code;
};

/* For commands that take a keysequence argument: */

struct key_sequence
{
  int top_map;
  struct line * keys;		/* char_to_string name of the key sequence. */
  struct key cmd;		/* The mapping of the sequence. */
};


struct keymap
{
  struct keymap *map_next;
  int id;
  struct key keys[256];
};

extern int num_maps;
extern struct keymap **the_maps;
extern char **map_names;
extern char **map_prompts;

#ifndef CTRL
#define CTRL(x) (x&037)
#endif
#ifndef META
#define META(X) ((X)|0200)
#endif

/* String convention conversion. */

#define map_id(NAME)  map_idn(NAME, strlen(NAME))

/*
 * Keymap names.  They can be used as indices into the_maps and map_names.
 */
#define MAIN_MAP map_id("main")
#define EDIT_MAP map_id("edit")
#define DIGIT_MAP map_id("digit")
#define NAVIGATE_MAP map_id("navigate")


extern int search_map_for_cmd (struct line * line, int map, int vec, int code);
extern void bind_key (char * keymap, char * function, int ch);
extern void unbind_key (char * keymap, int ch);
extern void bind_or_unbind_set (char * keymap, char * command, char * keyset);
extern void unbind_set (char *keymap, char *keyset);
extern void bind_all_keys (char * keymap, char * function);
extern void write_keys_cmd (FILE *fp);
extern void clear_keymap (struct keymap *m);
extern int map_idn (char *name, int n);
extern void create_keymap (char * mapname, char * parentname);
extern void set_map_prompt (char * map, char * str);

#endif  /* KEYH */
