/*
 *  $Id: fallback.h,v 1.3 1998/08/28 15:30:42 danny Exp $
 *
 *  This file is part of Oleo, a free spreadsheet.
 *
 *  Modifications from oleo-1.6 are copyright (C) 1998 by Danny Backx
 *  <danny.backx@advalvas.be>.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
static
char *fallback[] = {
/* Global */
	"Oleo.title:			Oleo",
	"Oleo.geometry:			800x600",
	"*tearOffModel:			XmTEAR_OFF_ENABLED",
#if 0
	"oleo.rows:			65536",
	"oleo.columns:			65536",
#else
	"oleo.rows:			1000",
	"oleo.columns:			1000",
#endif
	"*XmMainWindow.showSeparator:	true",
	"*background:			#AAAAAA",
	"*bae.background:		#CCCCCC",
	"*formula.columns:		40",
	"*splash.fontList:		-b&h-lucidatypewriter-bold-r-normal-sans-64-*-*-*-*-*-iso8859-1=large,-b&h-lucida-medium-i-normal-sans-18-*-*-*-*-*-iso8859-1=small",
#if 0
	"*splash.fontList:		-b&h-lucidatypewriter-bold-r-normal-sans-64-*-*-*-*-*-iso8859-1=large,-b&h-lucidatypewriter-medium-r-normal-sans-18-*-*-*-*-*-iso8859-1=small",
#endif
	"*splash.background:		#AAAAAA",
	"*splash.foreground:		red",
	"*messagesSW.height:		70",
/* Menu System */
/* File */
	"*filecascade.labelString:	File",
	"*open.labelString:		Open",
	"*close.labelString:		Close",
	"*save.labelString:		Save",
	"*saveas.labelString:		Save As",
	"*print.labelString:		Print",
	"*quit.labelString:		Quit",
/* Edit */
	"*editcascade.labelString:	Edit",
	"*undo.labelString:		Undo",
	"*copy.labelString:		Copy",
	"*cut.labelString:		Cut",
	"*paste.labelString:		Paste",
	"*insert.labelString:		Insert",
	"*delete.labelString:		Delete",
	"*move.labelString:		Move",
	"*recalculate.labelString:	Recalculate",
	"*setregion.labelString:	Set Region",
	"*setmark.labelString:		Set Mark",
	"*gotocell.labelString:		Goto Cell",
/* Style */
	"*stylecascade.labelString:	Style",
	"*format.labelString:		Format",
	"*alignment.labelString:	Alignment",
	"*font.labelString:		Font",
	"*width.labelString:		Width",
	"*height.labelString:		Height",
	"*protection.labelString:	Protection",
/* Options */
	"*optionscascade.labelString:	Options",
	"*a0.labelString:		Represent Columns as A0",
	"*edges.labelString:		Edges",
	"*autorecalc.labelString:	Automatically Recalculate",
	"*loadhooks.labelString:	Run load hooks",
	"*statusline.labelString:	Status line on top",
	"*backup.labelString:		Backup old file when saving",
	"*backupcopy.labelString:	Backup by copy, not rename",
	"*formats.labelString:		Formats",
	"*printoptions.labelString:	Print Options",
	"*defaultfileformat.labelString:	Default File Format",
	"*setvariable.labelString:		Set Variable",
	"*showvariable.labelString:		Show Variable",
	"*listvariables.labelString:		List Variables",
	"*showoptions.labelString:		Show Options",
/* Graph */
	"*graphcascade.labelString:	Graph",
	"*define.labelString:		Define Graph",
	"*show.labelString:		Show Graph",
	"*graphmenu.print.labelString:	Print Graph",
/* Help */
	"*helpcascade.labelString:	Help",
	"*about.labelString:		About Oleo",
	"*version.labelString:		Version",
	"*using.labelString:		Using Oleo",
	"*math.labelString:		Math Functions",
	"*trig.labelString:		Trigonometric Functions",
	"*stats.labelString:		Statistical Functions",
	"*bool.labelString:		Boolean Functions",
	"*string.labelString:		String Functions",
	"*struct.labelString:		Structure Functions",
	"*search.labelString:		Search Functions",
	"*bus.labelString:		Business Functions",
	"*date.labelString:		Date Functions",
	"*expr.labelString:		Expressions",
	"*error.labelString:		Error Values",
	"*helpmenu.format.labelString:	Formats",
	"*helpmenu.option.labelString:	Options",
/* BaeMatrix */
	"*bae.allowColumnResize:	True",
	"*bae.columnLabelColor:		dark slate blue",
	"*bae.rowLabelColor:		dark slate blue",
/* Help Dialog */
	"*versionD*Help*labelString:	GPL",
/* Configuration Dialogs */
	"*configureGraph_popup*.title:			Configure the Graph",
	"*configureGraphFrame*cap1.labelString:		X Axis",
	"*configureGraphFrame*cap2.labelString:		Data 1",
	"*configureGraphFrame*cap3.labelString:		Data 2",
	"*configureGraphFrame*cap4.labelString:		Data 3",
	"*configureGraphFrame*cap5.labelString:		Data 4",
	"*configureGraph*ok.labelString:		OK",
	"*configureGraph*cancel.labelString:		Cancel",
	"*configureGraph*help.labelString:		Help",
/* Mnemonics */
	"*filecascade.mnemonic:		F",
	"*open.mnemonic:		O",
	"*quit.mnemonic:		Q",
	"*close.mnemonic:		C",
	"*save.mnemonic:		S",
	"*saveas.mnemonic:		A",
	"*filemenu.print.mnemonic:	P",

	"*editcascade.mnemonic:		E",
	"*undo.mnemonic:		U",
	"*copy.mnemonic:		C",
	"*cut.mnemonic:			t",
	"*paste.mnemonic:		P",

	"*stylecascade.mnemonic:	S",

	"*optionscascade.mnemonic:	O",

	"*graphcascade.mnemonic:	G",

	"*helpcascade.mnemonic:		H",

/* Accelerators */
	"*open.accelerator:		Ctrl<Key>o",
	"*close.accelerator:		Ctrl<Key>x",
	"*save.accelerator:		Ctrl<Key>s",
	"*saveas.accelerator:		Ctrl<Key>a",
	"*filemenu.print.accelerator:	Ctrl<Key>p",
	"*quit.accelerator:		Ctrl<Key>q",

	"*undo.accelerator:		Ctrl<Key>u",
	"*copy.accelerator:		Ctrl<Key>osfInsert",
#ifndef LESSTIF_VERSION
	"*cut.accelerator:		Shift<Key>osfDelete",
#endif
	"*paste.accelerator:		Ctrl<Key>osfInsert",
	"*insert.accelerator:		Ctrl<Key>plus",
	"*gotocell.accelerator:		<Key>F5",
	"*format.accelerator:		Ctrl<Key>F",
/* Accelerator Texts */
	"*open.acceleratorText:		Ctrl-O",
	"*save.acceleratorText:		Ctrl-S",
	"*saveas.acceleratorText:	Ctrl-A",
	"*close.acceleratorText:	Ctrl-X",
	"*print.acceleratorText:	Ctrl-P",
	"*quit.acceleratorText:		Ctrl-Q",

	"*undo.acceleratorText:		Ctrl-U",
	"*copy.acceleratorText:		Ctrl-Insert",
	"*cut.acceleratorText:		Shift-Delete",
	"*paste.acceleratorText:	Ctrl-Insert",
	"*insert.acceleratorText:	Ctrl-+",
	"*gotocell.acceleratorText:	F5",

	"*format.acceleratorText:	Ctrl-F",
/* End */
	NULL
};
