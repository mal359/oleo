/*
 * $Id: init.c,v 1.10 2001/02/14 19:22:57 danny Exp $
 *
 * Copyright � 1993, 2000, 2001 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include "proto.h"
#include "init.h"
#include "global.h"
#include "io-term.h"
#include "utils.h"
#include "cmd.h"
/*
 * These commands are run before we are ready to do output.  They
 * should not do any kind of io or cause errors.  If they do, you
 * will experience core dumps.
 */
static char *init_cmds[] =
{
  "",
  "# The keymap tree.",
  "",
  "# The map `universal' already exists, and is always the keymap of last",
  "# resort.  It is almost always a bad idea to add new bindings there.",
  "",
  "# This is the generic top-level keymap.   It should contain bindings that",
  "# apply at the top level keymap regardless of what mode the editor is in.",
  "# All keymaps that are used at the top level should inherit from this.",
  "create-keymap generic-main universal",
  "",
  "# This is the keymap in force at the top level when no command arguments are",
  "# being edited.",
  "create-keymap main generic-main",
  "",
  "# The keymap after a M- or ESC prefix in main:",
  "create-keymap generic-meta universal",
  "create-keymap meta generic-meta",
  "",
  "# The keymap for ESC [, Arrow keys are bound in this map.",
  "create-keymap generic-ansi universal",
  "create-keymap ansi generic-ansi",
  "",
  "create-keymap generic-trolx universal",
  "create-keymap trolx generic-trolx",
  "",
  "# Bindings that define prefix keys.",
  "bind-key generic-main generic-meta ^[",
  "bind-key main meta ^[",
  "",
  "create-keymap mouse universal",
  "bind-key generic-main mouse ^\\\\",
  "bind-key mouse mouse-goto 0",
  "bind-key mouse mouse-mark 1",
  "bind-key mouse mouse-mark-and-goto 2",
  "",
  "",
  "bind-key generic-main generic-trolx ^x",
  "bind-key main trolx ^x",
  "",
  "bind-key generic-meta generic-ansi [",
  "bind-key meta ansi [",
  "",
  "# Misc generic commands",
  "",
  "bind-key universal break ^g",
  "bind-key universal redraw-screen ^l",
  "bind-key generic-main universal-argument ^u",
  "bind-key generic-main suspend-oleo ^z",
  "bind-key generic-trolx kill-oleo ^c",
  "bind-key generic-trolx recalculate !",
  "bind-key generic-meta exec x",
  "",
  "# Prefix argument handling",
  "# This keymap is magicly invoked within command_loop in cmd.c.",
  "create-keymap prefix universal",
  "bind-set prefix universal-argument 0-9",
  "bind-key prefix universal-argument -",
  "",
  "#",
  "# Cursor motion",
  "#",
  "",
  "# For EMACS users",
  "# Notice that these motion commands are not bound genericly.  They don't",
  "# in general apply if the input area is active.",
  "",
  "bind-key main    up-cell  ^p",
  "bind-key main right-cell  ^f",
  "bind-key main  left-cell  ^b",
  "bind-key main  down-cell  ^n",
  "",
  "# Ansi motion.",
  "bind-key generic-ansi up-cell A",
  "bind-key generic-ansi down-cell B",
  "bind-key generic-ansi right-cell C",
  "bind-key generic-ansi left-cell D",
  "",
  "# These are only useful under X, but there, they are quite useful.",
  "name-macro-string up-cell-and-edit \\",
  "{exit-minibuffer}{up-cell}{edit-cell}",
  "name-macro-string down-cell-and-edit \\",
  "{exit-minibuffer}{down-cell}{edit-cell}",
  "name-macro-string left-cell-and-edit \\",
  "{exit-minibuffer}{left-cell}{edit-cell}",
  "name-macro-string right-cell-and-edit \\",
  "{exit-minibuffer}{right-cell}{edit-cell}",
  "",
  "create-keymap edit-ansi generic-ansi",
  "bind-key edit-ansi up-cell-and-edit A",
  "bind-key edit-ansi down-cell-and-edit B",
  "bind-key edit-ansi forward-char C",
  "bind-key edit-ansi backward-char D",
  "",
  "create-keymap meta-edit-ansi universal",
  "bind-key edit-ansi meta-edit-ansi ^[",
  "bind-key meta-edit-ansi up-cell-and-edit A",
  "bind-key meta-edit-ansi down-cell-and-edit B",
  "bind-key meta-edit-ansi right-cell-and-edit C",
  "bind-key meta-edit-ansi left-cell-and-edit D",
  "",
  "bind-key trolx goto-cell j",
  "bind-key meta goto-region j",
  "",
  "# Going to extremes",
  "bind-key meta upper-left <",
  "bind-key meta lower-right >",
  "",
  "# Marking time",
  "bind-key trolx exchange-point-and-mark ^X",
  "bind-key main mark-cell ^@",
  "",
  "# Scrolling commands.",
  "bind-key meta scroll-up v",
  "bind-key main scroll-down ^V",
  "bind-key trolx scroll-right >",
  "bind-key trolx scroll-left <",
  "",
  "# Other ways of moving the cell cursor.",
  "",
  "bind-key meta scan-up p",
  "bind-key meta scan-down n",
  "bind-key meta scan-left b",
  "bind-key meta scan-right f",
  "",
  "bind-key main beginning-of-row ^a",
  "bind-key main end-of-row ^e",
  "bind-key meta beginning-of-col ^a",
  "bind-key meta end-of-col ^e",
  "",
  "# Macros",
  "bind-key generic-trolx start-entering-macro (",
  "bind-key generic-trolx stop-entering-macro )",
  "bind-key generic-trolx call-last-kbd-macro e",
  "bind-key generic-trolx store-last-macro =",
  "",
  "# The keymap used when prompting for a key-sequence: ",
  "create-keymap read-keyseq universal",
  "bind-set read-keyseq self-map-command \\000-\\377",
  "",
  "# Help commands",
  "create-keymap generic-help universal",
  "create-keymap unprompted-help generic-help",
  "create-keymap help generic-help",
  "bind-key generic-main unprompted-help ^h",
  "bind-key main help ^h",
  "bind-key unprompted-help help ^h",
  "bind-key unprompted-help help ?",
  "",
  "set-map-prompt help \\",
  "Help keys (type ? for more explanation): C c f F k o v w W ^W",
  "",
  "bind-key generic-help help-with-command C",
  "bind-key generic-help describe-key-briefly c",
  "bind-key generic-help describe-function f",
  "bind-key generic-help describe-formula F",
  "bind-key generic-help describe-key k",
  "bind-key generic-help show-options o",
  "bind-key generic-help show-variable v",
  "bind-key generic-help show-all-variables ^v",
  "bind-key generic-help where-is w",
  "",
  "name-macro-string view-wallchart \\",
  "{make-wallchart-info}{with-keymap view-info}{set-info wallchart}",
  "bind-key generic-help view-wallchart W",
  "",
  "name-macro-string write-wallchart \\",
  "{make-wallchart-info}{write-info wallchart}",
  "bind-key generic-help write-wallchart ^w",
  "",
  "create-keymap verbose-help-map generic-help",
  "set-map-prompt verbose-help-map \\",
  "Help (+,-,ESC or one of the help characters): ",
  "",
  "",
  "name-macro-string verbose-help \\",
  "  {with-keymap verbose-help-map}{builtin-help help-with-help}",
  "",
  "bind-key help verbose-help ^h",
  "bind-key help verbose-help ?",
  "",
  "bind-key verbose-help-map exit-minibuffer ^[",
  "bind-key verbose-help-map page-info +",
  "bind-key verbose-help-map page-info-backwards -",
  "",
  "name-macro-string verbose-hwc {exit-minibuffer}{help-with-command}",
  "name-macro-string verbose-dkb {exit-minibuffer}{describe-key-briefly}",
  "name-macro-string verbose-df {exit-minibuffer}{describe-function}",
  "name-macro-string verbose-dk {exit-minibuffer}{describe-key}",
  "name-macro-string verbose-so {exit-minibuffer}{show-options}",
  "name-macro-string verbose-sv {exit-minibuffer}{show-variable}",
  "name-macro-string verbose-sav {exit-minibuffer}{show-all-variables}",
  "name-macro-string verbose-w {exit-minibuffer}{where-is}",
  "",
  "bind-key verbose-help-map verbose-hwc C",
  "bind-key verbose-help-map verbose-dkb c",
  "bind-key verbose-help-map verbose-df  f",
  "bind-key verbose-help-map verbose-dk  k",
  "bind-key verbose-help-map verbose-so  o",
  "bind-key verbose-help-map verbose-sv  v",
  "bind-key verbose-help-map verbose-sav ^v",
  "bind-key verbose-help-map verbose-w   w",
  "",
  "",
  "# Variables",
  "bind-key trolx set-var :",
  "",
  "# Files",
  "",
  "bind-key generic-trolx save-spreadsheet ^s",
  "bind-key trolx find-alternate-spreadsheet ^v",
  "bind-key trolx merge-spreadsheet i",
  "",
  "bind-key meta copy-region c",
  "bind-key meta copy-values-in-region ^c",
  "bind-key meta move-region m",
  "bind-key main insert-row ^o",
  "bind-key meta insert-col o",
  "bind-key main delete-row ^k",
  "bind-key meta delete-col k",
  "",
  "# Changing cell formulas",
  "bind-key main set-cell =",
  "",
  "# Some characters start editing the current cell and then are reinterpreted",
  "# as editing commands:",
  "bind-set main goto-edit-cell a-z",
  "bind-set main goto-edit-cell A-Z",
  "bind-set main goto-edit-cell 0-9",
  "bind-key main goto-edit-cell \\",
  "bind-key main goto-edit-cell +",
  "bind-key main goto-edit-cell -",
  "bind-key main goto-edit-cell *",
  "bind-key main goto-edit-cell /",
  "bind-key main goto-edit-cell (",
  "bind-key main goto-edit-cell )",
  "bind-key main goto-edit-cell @",
  "bind-key main goto-edit-cell ^",
  "bind-key main goto-edit-cell %",
  "bind-key main goto-edit-cell >",
  "bind-key main goto-edit-cell <",
  "bind-key main goto-edit-cell !",
  "bind-key main goto-edit-cell ?",
  "bind-key main goto-edit-cell :",
  "bind-key main goto-edit-cell #",
  "bind-key main goto-edit-cell _",
  "bind-key main goto-edit-cell .",
  "bind-key main goto-edit-cell ,",
  "bind-key main goto-edit-cell ;",
  "bind-key main goto-edit-cell  \"",
  "bind-key meta set-region-formula =",
  "bind-key main delete-cell ^?",
  "bind-key meta delete-region ^?",
  "",
  "",
  "# Window commands",
  "",
  "bind-key meta recenter-window ^l",
  "bind-key trolx split-window-horizontally 5",
  "bind-key trolx split-window-vertically 2",
  "bind-key trolx delete-window 0",
  "bind-key trolx delete-other-windows 1",
  "bind-key meta goto-minibuffer ^",
  "bind-key trolx other-window o",
  "",
  "# Command argument editing",
  "",
  "create-keymap read-string generic-main",
  "create-keymap meta-read-string generic-meta",
  "create-keymap trolx-read-string generic-trolx",
  "",
  "bind-key read-string meta-read-string ^[",
  "bind-set read-string self-insert-command \\ -~",
  "bind-set read-string self-insert-command �-�",
  "bind-key read-string exit-minibuffer ^m",
  "bind-key read-string beginning-of-line ^a",
  "bind-key read-string end-of-line ^e",
  "bind-key read-string backward-char ^b",
  "bind-key meta-read-string backward-word b",
  "bind-key read-string backward-delete-char ^?",
  "bind-key meta-read-string backward-delete-word ^?",
  "bind-key read-string forward-char ^f",
  "bind-key meta-read-string forward-word f",
  "bind-key read-string delete-char ^d",
  "bind-key meta-read-string delete-word d",
  "bind-key read-string kill-line ^k",
  "bind-key meta-read-string insert-cell-expression e",
  "bind-key meta-read-string insert-cell-value v",
  "bind-key meta-read-string insert-rel-ref r",
  "bind-key meta-read-string insert-abs-ref a",
  "bind-key meta-read-string toggle-overwrite o",
  "",
  "bind-key read-string trolx-read-string ^x",
  "bind-key trolx-read-string other-window o",
  "",
  "# Reading various argument types specificly",
  "create-keymap read-symbol read-string",
  "create-keymap read-word read-string",
  "create-keymap read-range read-string",
  "create-keymap read-filename read-string",
  "create-keymap read-integer read-string",
  "create-keymap read-float read-string",
  "create-keymap read-formula read-string",
  "",
  "# Special arrow key meanings for edit mode",
  "create-keymap meta-read-formula meta-read-string",
  "bind-key read-formula meta-read-formula ^[",
  "bind-key meta-read-formula edit-ansi [",
  "",
  "# auto-motion",
  "create-keymap set-auto-motion universal",
  "set-map-prompt set-auto-motion \\",
  "Set auto-motion direction [^v<>`'/\\ or SPC for none] ",
  "",
  "bind-key generic-trolx set-auto-motion m",
  "",
  "bind-key set-auto-motion auto-move-up ^",
  "bind-key set-auto-motion auto-move-down v",
  "bind-key set-auto-motion auto-move-down V",
  "bind-key set-auto-motion auto-move-left <",
  "bind-key set-auto-motion auto-move-left ,",
  "bind-key set-auto-motion auto-move-right >",
  "bind-key set-auto-motion auto-move-right .",
  "bind-key set-auto-motion auto-move-up-right '",
  "bind-key set-auto-motion auto-move-up-left `",
  "bind-key set-auto-motion auto-move-down-right \\\\   ",
  "bind-key set-auto-motion auto-move-down-left  /",
  "bind-key set-auto-motion auto-move-no-motion \\ ",
  "",
  "bind-key read-formula next-edit ^i",
  "bind-key read-formula next-edit-set ^j",
  "",
  "create-keymap read-most-chars universal",
  "",
  "bind-set read-most-chars exit-self-inserting \\000-\\377",
  "bind-key read-most-chars break ^G",
  "bind-key read-most-chars redraw-screen ^L",
  "",
  "create-keymap press-any universal",
  "set-map-prompt press-any \\",
  "Press any key to continue. ",
  "bind-set press-any exit-minibuffer \\000-\\377",
  "bind-key press-any break ^G",
  "bind-key press-any redraw-screen ^L",
  "",
  "create-keymap read-char read-most-chars",
  "bind-key read-char exit-self-inserting ^G",
  "",
  "",
  "# Menu driven commands use this keymap to read menu selections",
  "create-keymap read-menu read-most-chars",
  "bind-key read-menu help-with-command ^h",
  "",
  "create-keymap read-format read-string",
  "",
  "# These maps are used to manipulate cell attributes.",
  "# They reimplement the old format-cell, format-region and",
  "# set-default commands.",
  "",
  "create-keymap set-cell-attr 	universal",
  "create-keymap set-region-attr 	universal",
  "create-keymap set-default-attr 	universal",
  "",
  "set-map-prompt set-cell-attr \\",
  "Set cell [A]lignment [F]ormat f[O]nt [P]rotection [H]eight [W]idth",
  "set-map-prompt set-region-attr \\",
  "Set region [A]lignment [F]ormat f[O]nt [P]rotection [H]eight [W]idth",
  "set-map-prompt set-default-attr \\",
  "Set default [A]lignment [F]ormat f[O]nt [P]rotection [H]eight [W]idth",
  "",
  "bind-key meta set-default-attr d",
  "bind-key meta set-region-attr r",
  "bind-key meta set-cell-attr a",
  "",
  "bind-key set-cell-attr	set-cell-alignment	a",
  "bind-key set-cell-attr	set-cell-format		f",
  "bind-key set-cell-attr	set-cell-font		o",
  "bind-key set-cell-attr	set-cell-protection	p",
  "bind-key set-cell-attr	set-cell-height		h",
  "bind-key set-cell-attr	set-cell-width		w",
  "bind-key set-cell-attr	set-cell-alignment	A",
  "bind-key set-cell-attr	set-cell-format		F",
  "bind-key set-cell-attr	set-cell-font		O",
  "bind-key set-cell-attr	set-cell-protection	P",
  "bind-key set-cell-attr	set-cell-height		H",
  "bind-key set-cell-attr	set-cell-width		W",
  "",
  "bind-key set-region-attr	set-region-alignment		a",
  "bind-key set-region-attr	set-region-format		f",
  "bind-key set-region-attr	set-region-font			o",
  "bind-key set-region-attr	set-region-protection		p",
  "bind-key set-region-attr	set-region-height		h",
  "bind-key set-region-attr	set-region-width		w",
  "bind-key set-region-attr	set-region-alignment		A",
  "bind-key set-region-attr	set-region-format		F",
  "bind-key set-region-attr	set-region-font			O",
  "bind-key set-region-attr	set-region-protection		P",
  "bind-key set-region-attr	set-region-height		H",
  "bind-key set-region-attr	set-region-width		W",
  "",
  "bind-key set-default-attr	set-default-alignment		a",
  "bind-key set-default-attr	set-default-format		f",
  "bind-key set-default-attr	set-default-font		o",
  "bind-key set-default-attr	set-default-protection		p",
  "bind-key set-default-attr	set-default-height		h",
  "bind-key set-default-attr	set-default-width		w",
  "bind-key set-default-attr	set-default-alignment		A",
  "bind-key set-default-attr	set-default-format		F",
  "bind-key set-default-attr	set-default-font		O",
  "bind-key set-default-attr	set-default-protection		P",
  "bind-key set-default-attr	set-default-height		H",
  "bind-key set-default-attr	set-default-width		W",
  "",
  "create-keymap view-info universal",
  "bind-key view-info exit-minibuffer ^[",
  "bind-key view-info page-info \\ ",
  "bind-key view-info page-info +",
  "bind-key view-info page-info-backwards -",
  "",
  "set-map-prompt view-info \\",
  "+ (forward page)   - (backward page)   ESC (return to spreadsheet editing)",
  "",
  "",
  "",
  "",
  "",
  "# Verbose menu type keymaps use exit-minibuffer to move closer",
  "# to the root of the menu.",
  "",
  "create-keymap generic-menu universal",
  "bind-key generic-menu exit-minibuffer ^[",
  "bind-key generic-menu exit-minibuffer ^m",
  "bind-key generic-menu exit-minibuffer ^j",
  "",
#ifdef	HAVE_LIBPLOT
  "# Graphing with GNU Plotutils",
  "",
  "create-keymap plotutils-commands generic-menu",
  "name-macro-string plotutils-setup \\",
  " {with-keymap plotutils-commands}{builtin-help Plotutils-graphs}",
  "bind-key generic-meta plotutils-setup g",
  "set-map-prompt plotutils-commands \\",
  "(R C o d v, p x y t s f, X P B Esc)? ",
  "",
  "bind-key plotutils-commands plotutils-plot p",
  "bind-key plotutils-commands plotutils-set-data-title t",
  "bind-key plotutils-commands plotutils-set-data d",
  "bind-key plotutils-commands plotutils-set-style s",
  "bind-key plotutils-commands plotutils-set-filename f",
  "",
  "bind-key plotutils-commands plotutils-xy X",
  "bind-key plotutils-commands plotutils-pie P",
  "bind-key plotutils-commands plotutils-bar B",
  "",
  "name-macro-string plotutils-preset-msg {display-msg Graph reset}",
  "name-macro-string plotutils-presets-verbosely \\",
  "{plotutils-presets}{plotutils-preset-msg}{set-info _expanded__info_graphing}",
  "bind-key plotutils-commands plotutils-presets-verbosely R",
  "",
  "name-macro-string plotutils-clear-msg {display-msg Graph data reset}",
  "name-macro-string plotutils-clear-verbosely \\",
  "{plotutils-clear-datasets}{plotutils-clear-msg}{set-info _expanded__info_graphing}",
  "bind-key plotutils-commands plotutils-clear-verbosely C",
  "",
  "create-keymap plotutils-terminal-types universal",
  "name-macro-string plotutils-select-output \\",
  "{one-command-with-keymap plotutils-terminal-types}{builtin-help plotutils-output-types}",
  "bind-key plotutils-commands plotutils-select-output o",
  "set-map-prompt plotutils-terminal-types \\",
  "Graph output type (p t r m i f P h g n x X): ",
  "bind-key plotutils-terminal-types plotutils-postscript p",
  "bind-key plotutils-terminal-types plotutils-tek t",
  "bind-key plotutils-terminal-types plotutils-regis r",
  "bind-key plotutils-terminal-types plotutils-metaplot m",
  "bind-key plotutils-terminal-types plotutils-illustrator i",
  "bind-key plotutils-terminal-types plotutils-fig f",
  "bind-key plotutils-terminal-types plotutils-pcl P",
  "bind-key plotutils-terminal-types plotutils-hpgl h",
  "bind-key plotutils-terminal-types plotutils-gif g",
  "bind-key plotutils-terminal-types plotutils-png n",
  "bind-key plotutils-terminal-types plotutils-x-color X",
  "bind-key plotutils-terminal-types plotutils-x-mono x",
  "",
  "create-keymap view-plotutils-parameters view-info",
  "name-macro-string plotutils-verify \\",
  "{plotutils-make-info}{with-keymap view-plotutils-parameters}{set-info graphing-parameters}",
  "bind-key plotutils-commands plotutils-verify v",
  "set-map-prompt view-plotutils-parameters \\",
  "+ (forward page)   - (backward page)   p (plot)  or  ESC",
  "bind-key view-plotutils-parameters plotutils-plot p",
  "",
  "create-keymap plotutils-x-axis-commands generic-menu",
  "name-macro-string plotutils-x-axis \\",
  "{with-keymap plotutils-x-axis-commands}{builtin-help plotutils-x-axis-help}",
  "bind-key plotutils-commands plotutils-x-axis x",
  "set-map-prompt plotutils-x-axis-commands X-axis (s [ ] l L Esc)",
  "",
  "name-macro-string plotutils-set-x-range-low {plotutils-set-axis-low x}",
  "name-macro-string plotutils-set-x-range-high {plotutils-set-axis-high x}",
  "bind-key plotutils-x-axis-commands plotutils-set-x-range-low [",
  "bind-key plotutils-x-axis-commands plotutils-set-x-range-high ]",
  "name-macro-string plotutils-set-x-labels {plotutils-set-axis-labels x}",
  "bind-key plotutils-x-axis-commands plotutils-set-x-labels l",
  "name-macro-string plotutils-default-x-labels {plotutils-default-axis-labels x}",
  "bind-key plotutils-x-axis-commands plotutils-default-x-labels L",
  "",
  "create-keymap plotutils-y-axis-commands generic-menu",
  "name-macro-string plotutils-y-axis \\",
  "{with-keymap plotutils-y-axis-commands}{builtin-help plotutils-y-axis-help}",
  "bind-key plotutils-commands plotutils-y-axis y",
  "set-map-prompt plotutils-y-axis-commands Y-axis (s [ ] l L Esc)",
  "",
  "name-macro-string plotutils-set-y-range-low {plotutils-set-axis-low y}",
  "name-macro-string plotutils-set-y-range-high {plotutils-set-axis-high y}",
  "bind-key plotutils-y-axis-commands plotutils-set-y-range-low [",
  "bind-key plotutils-y-axis-commands plotutils-set-y-range-high ]",
  "name-macro-string plotutils-set-y-labels {plotutils-set-axis-labels y}",
  "bind-key plotutils-y-axis-commands plotutils-set-y-labels l",
  "name-macro-string plotutils-default-y-labels {plotutils-default-axis-labels y}",
  "bind-key plotutils-y-axis-commands plotutils-default-y-labels L",
  "",
#else
/* Don't use gnuplot if you also have plotutils */
# ifdef	WITH_GNUPLOT
  "# Graphing with gnuplot",
  "",
  "create-keymap graph-commands generic-menu",
  "name-macro-string graph-setup \\",
  " {with-keymap graph-commands}{builtin-help graphing}",
  "bind-key generic-meta graph-setup g",
  "set-map-prompt graph-commands \\",
  "(R C o x y t d s p Esc)? ",
  "",
  "name-macro-string graph-preset-msg {display-msg Graph reset}",
  "name-macro-string graph-presets-verbosely \\",
  "{graph-presets}{graph-preset-msg}{set-info _expanded__info_graphing}",
  "bind-key graph-commands graph-presets-verbosely R",
  "",
  "name-macro-string graph-clear-msg {display-msg Graph data reset}",
  "name-macro-string graph-clear-verbosely \\",
  "{graph-clear-datasets}{graph-clear-msg}{set-info _expanded__info_graphing}",
  "bind-key graph-commands graph-clear-verbosely C",
  "",
  "bind-key graph-commands graph-set-data-title t",
  "bind-key graph-commands graph-set-data d",
  "bind-key graph-commands graph-set-style s",
  "bind-key graph-commands graph-plot p",
  "",
  "create-keymap view-graphing-parameters view-info",
  "name-macro-string graph-verify \\",
  "{graph-make-info}{with-keymap view-graphing-parameters}{set-info graphing-parameters}",
  "bind-key graph-commands graph-verify v",
  "set-map-prompt view-graphing-parameters \\",
  "+ (forward page)   - (backward page)   p (plot)  or  ESC",
  "bind-key view-graphing-parameters graph-plot p",
  "",
  "create-keymap graph-terminal-types universal",
  "name-macro-string graph-select-output \\",
  "{one-command-with-keymap graph-terminal-types}{builtin-help graph-output-types}",
  "bind-key graph-commands graph-select-output o",
  "set-map-prompt graph-terminal-types \\",
  "Graph output type (x X P): ",
  "bind-key graph-terminal-types graph-postscript p",
  "",
  "create-keymap graph-x-axis-commands generic-menu",
  "name-macro-string graph-x-axis \\",
  "{with-keymap graph-x-axis-commands}{builtin-help graph-x-axis-help}",
  "bind-key graph-commands graph-x-axis x",
  "set-map-prompt graph-x-axis-commands X-axis (s [ ] l L Esc)",
  "",
  "name-macro-string graph-set-x-range-low {graph-set-axis-low x}",
  "name-macro-string graph-set-x-range-high {graph-set-axis-high x}",
  "bind-key graph-x-axis-commands graph-set-x-range-low [",
  "bind-key graph-x-axis-commands graph-set-x-range-high ]",
  "bind-key graph-x-axis-commands graph-set-x-axis-symbolic s",
  "name-macro-string graph-set-x-labels {graph-set-axis-labels x}",
  "bind-key graph-x-axis-commands graph-set-x-labels l",
  "name-macro-string graph-default-x-labels {graph-default-axis-labels x}",
  "bind-key graph-x-axis-commands graph-default-x-labels L",
  "",
  "create-keymap graph-y-axis-commands generic-menu",
  "name-macro-string graph-y-axis \\",
  "{with-keymap graph-y-axis-commands}{builtin-help graph-y-axis-help}",
  "bind-key graph-commands graph-y-axis y",
  "set-map-prompt graph-y-axis-commands Y-axis (s [ ] l L Esc)",
  "",
  "name-macro-string graph-set-y-range-low {graph-set-axis-low y}",
  "name-macro-string graph-set-y-range-high {graph-set-axis-high y}",
  "bind-key graph-y-axis-commands graph-set-y-range-low [",
  "bind-key graph-y-axis-commands graph-set-y-range-high ]",
  "name-macro-string graph-set-y-labels {graph-set-axis-labels y}",
  "bind-key graph-y-axis-commands graph-set-y-labels l",
  "name-macro-string graph-default-y-labels {graph-default-axis-labels y}",
  "bind-key graph-y-axis-commands graph-default-y-labels L",
  "",
# endif	/* WITH_GNUPLOT */
#endif	/* ~ HAVE_LIBPLOT */
  "# printing",
  "",
  "create-keymap print-commands universal",
  "bind-key meta print-commands ^p",
  "set-map-prompt print-commands \\",
  "[A]scii or [P]ostscript printing? ",
  "",
  "bind-key print-commands print-region a",
  "bind-key print-commands print-region A",
  "",
  "create-keymap psprint-commands universal",
  "bind-key print-commands psprint-commands p",
  "bind-key print-commands psprint-commands P",
  "set-map-prompt psprint-commands \\",
  "set default [F]ont   set page [S]ize   [P]rint",
  "",
  "bind-key psprint-commands set-page-size s",
  "bind-key psprint-commands set-page-size S",
  "bind-key psprint-commands set-default-ps-font f",
  "bind-key psprint-commands set-default-ps-font F",
  "bind-key psprint-commands psprint-region p",
  "bind-key psprint-commands psprint-region P",
  "",
  "",
  "# Other init commands:",
  "define-font times *times-medium-r-* Times-Roman",
  "define-font times-italic *times-medium-i-* Times-Italic",
  "define-font times-oblique *times-medium-o-* Times-Italic",
  "define-font times-bold *times-bold-r-* Times-Bold",
  "define-font times-oblique-bold *times-bold-o-* Times-BoldItalic",
  "define-font times-italic-bold *times-bold-i-* Times-BoldItalic",
  "define-font courier *courier-medium-r-* Courier",
  "define-font courier-oblique *courier-medium-o-* Courier-Italic",
  "define-font courier-bold *courier-bold-r-* Courier-Bold",
  "define-font courier-bold-oblique *courier-bold-o-* Courier-BoldItalic",
  "define-font helvetica *helvetica-medium-r-* Helvetica",
  "define-font helvetica-oblique *helvetica-medium-o-* Helvetica-Italic",
  "define-font helvetica-bold *helvetica-bold-r-* Helvetica-Bold",
  "define-font helvetica-bold-oblique *helvetica-bold-o-* Helvetica-BoldItalic",
  "define-font fixed *fixed-medium-r-* Times-Roman",

  0

};

void 
run_init_cmds (void)
{
  char **p = init_cmds;
  while (*p)
    {
      char * cmd = strdup (*p++);
      int len = strlen (cmd);
      while (len && *p && (cmd [len - 1] == '\\')
	     && ((len == 1) || cmd[len - 2] != '\\'))
	{
	  cmd [len - 1] = '\0';
	  len += strlen (*p);
	  cmd = ck_realloc (cmd, len + 1);
	  strcat (cmd, *p);
	  ++p;
	}
      execute_command (cmd);
      free (cmd);
    }
}
