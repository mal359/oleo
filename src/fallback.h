/*
 *  $Id: fallback.h,v 1.1 1998/08/19 07:45:07 danny Exp $
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
	"*splash.fontList:		-b&h-lucidatypewriter-bold-r-normal-sans-64-*-*-*-*-*-iso8859-1",
	"*splash.background:		#AAAAAA",
	"*splash.foreground:		red",
/* Menu System */
/* File */
	"*filecascade.labelString:	File",
	"*open.labelString:		Open",
	"*quit.labelString:		Quit",
	"*close.labelString:		Close",
	"*save.labelString:		Save",
	"*saveas.labelString:		Save As",
/* Edit */
	"*editcascade.labelString:	Edit",
	"*undo.labelString:		Undo",
	"*copy.labelString:		Copy",
	"*cut.labelString:		Cut",
	"*paste.labelString:		Paste",
/* Options */
	"*optionscascade.labelString:	Options",
	"*a0.labelString:		Represent Columns as A0",
	"*configure.labelString:	Configure Graph",
/* Graph */
	"*graphcascade.labelString:	Graph",
/* Help */
	"*helpcascade.labelString:	Help",
	"*about.labelString:		About",
	"*version.labelString:		Version",
	"*using.labelString:		Using Oleo",
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

	"*editcascade.mnemonic:		E",
	"*undo.mnemonic:		U",
	"*copy.mnemonic:		C",
	"*cut.mnemonic:			t",
	"*paste.mnemonic:		P",

	"*optionscascade.mnemonic:	O",
	"*graphcascade.mnemonic:	G",
	"*helpcascade.mnemonic:		H",
/* Accelerators */
	"*open.accelerator:		Ctrl<Key>o",
	"*quit.accelerator:		Ctrl<Key>q",
	"*close.accelerator:		Ctrl<Key>x",
	"*save.accelerator:		Ctrl<Key>s",
	"*saveas.accelerator:		Ctrl<Key>a",
	"*undo.accelerator:		Ctrl<Key>u",
	"*copy.accelerator:		Ctrl<Key>osfInsert",
#ifndef LESSTIF_VERSION
	"*cut.accelerator:		Shift<Key>osfDelete",
#endif
	"*paste.accelerator:		Ctrl<Key>osfInsert",
/* Accelerator Texts */
	"*open.acceleratorText:		Ctrl-O",
	"*quit.acceleratorText:		Ctrl-Q",
	"*close.acceleratorText:	Ctrl-X",
	"*save.acceleratorText:		Ctrl-S",
	"*saveas.acceleratorText:	Ctrl-A",
	"*undo.acceleratorText:		Ctrl-U",
	"*copy.acceleratorText:		Ctrl-Insert",
	"*cut.acceleratorText:		Shift-Delete",
	"*paste.acceleratorText:	Ctrl-Insert",
/* End */
	NULL
};
