/*
 *  $Id: fallback.c,v 1.15 1999/06/04 08:01:59 danny Exp $
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
#include <stdio.h>
#include <Xm/Xm.h>

char *fallback[] = {
/* Global */
	"Oleo.title:			Oleo",
	"Oleo.geometry:			800x600",
	"*tearOffModel:			XmTEAR_OFF_ENABLED",
#if 0							/* FIX ME */
	"oleo.rows:			65536",
	"oleo.columns:			65536",
#else
	"oleo.rows:			1000",
	"oleo.columns:			1000",
#endif
	"*XmMainWindow.showSeparator:	true",
	"*background:			#AAAAAA",
	"*bae.background:		#CCCCCC",
//	"*bae.cellShadowType:		shadow_etched_in",
	"*bae.gridType:			grid_shadow_in",
	"*formula.columns:		80",
	"*splash*fontList:		-b&h-lucidatypewriter-bold-r-normal-sans-24-*-*-*-*-*-iso8859-1=large,-b&h-lucida-medium-i-normal-sans-18-*-*-*-*-*-iso8859-1=small",
	"*splash*background:		#AAAAAA",
	"*splash*foreground:		red",
	"*messagesSW.height:		70",
/* BaeMatrix */
	"*bae.allowColumnResize:	True",
	"*bae.columnLabelColor:		dark slate blue",
	"*bae.rowLabelColor:		dark slate blue",
	"*bae.translations:	#override\\n"
//		"<Btn1Down>:			SelectCell(start)\\n"
		"<Btn1Up>:			SelectCell(end)\\n"
		"<Btn1Motion>:			SelectCell(move)",
/* Menu System */
/* File */
	"*filecascade.labelString:	File",
	"*open.labelString:		Open",
	"*close.labelString:		Close",
	"*close.sensitive:		no",
	"*save.labelString:		Save",
	"*saveas.labelString:		Save As",
	"*print.labelString:		Print",
	"*quit.labelString:		Quit",
/* Edit */
	"*editcascade.labelString:	Edit",
	"*undo.labelString:		Undo",
	"*undo.sensitive:		no",
	"*copy.labelString:		Copy",
	"*copy.sensitive:		no",
	"*cut.labelString:		Cut",
	"*cut.sensitive:		no",
	"*paste.labelString:		Paste",
	"*paste.sensitive:		no",
	"*insert.labelString:		Insert",
	"*delete.labelString:		Delete",
	"*move.labelString:		Move",
	"*sort.labelString:		Sort",
	"*sort.sensitive:		no",
	"*copyregion.labelString:	Copy Region",
	"*copyvalues.labelString:	Copy Values",
	"*recalculate.labelString:	Recalculate",
	"*setregion.labelString:	Set Region",
	"*setregion.sensitive:		no",
	"*markcell.labelString:		Mark Cell",
	"*markregion.labelString:	Mark Region",
	"*setmark.labelString:		Set Mark",
	"*gotocell.labelString:		Goto Cell",
	"*gotocell.sensitive:		no",
/* Style */
	"*stylecascade.labelString:	Style",
	"*format.labelString:		Format",
	"*format.sensitive:		no",
	"*alignment.labelString:	Alignment",
	"*alignment.sensitive:		no",
	"*font.labelString:		Font",
	"*font.sensitive:		no",
	"*width.labelString:		Width",
	"*width.sensitive:		no",
	"*height.labelString:		Height",
	"*height.sensitive:		no",
	"*protection.labelString:	Protection",
	"*protection.sensitive:		no",
/* Options */
	"*optionscascade.labelString:		Options",
	"*a0.labelString:			Represent Columns as A0",
	"*edges.labelString:			Edges",
	"*autorecalc.labelString:		Automatically Recalculate",
	"*autorecalc.sensitive:			no",
	"*loadhooks.labelString:		Run load hooks",
	"*statusline.labelString:		Status line on top",
	"*backup.labelString:			Backup old file when saving",
	"*backupcopy.labelString:		Backup by copy, not rename",
	"*formats.labelString:			Formats",
	"*defaultformat.labelString:		Set Default Format/Precision",
	"*printoptions.labelString:		Print Options",
	"*defaultfileformat.labelString:	Default File Format",
	"*setvariable.labelString:		Set Variable",
	"*setvariable.sensitive:		no",
	"*showvariable.labelString:		Show Variable",
	"*showvariable.sensitive:		no",
	"*listvariables.labelString:		List Variables",
	"*listvariables.sensitive:		no",
	"*showoptions.labelString:		Show Options",
	"*showoptions.sensitive:		no",
/* Graph */
	"*graphcascade.labelString:		Graph",
	"*graphmenu.define.labelString:		Define Graph",
	"*graphmenu.show.labelString:		Show SciPlot Graph",
	"*graphmenu.print.labelString:		Print SciPlot Graph",
	"*graphmenu.pulabel.labelString:	GNU Plotutils",
	"*graphmenu.pushowpie.labelString:	Show Pie Chart",
	"*graphmenu.pushowbar.labelString:	Show Bar Chart",
	"*graphmenu.pushowxy.labelString:	Show XY Chart",
	"*graphmenu.puprintpie.labelString:	Print Pie Chart",
	"*graphmenu.puprintbar.labelString:	Print Bar Chart",
	"*graphmenu.puprintxy.labelString:	Print XY Chart",
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
/* Dialog titles */
	"*selectfile_popup.title:	Select a file to load or save",
	"*versionD_popup.title:		Oleo Version Dialog",
	"*helpShell.title:		Oleo Help Window",
/* Help Dialog */
	"*versionD*Help*labelString:	GPL",
/* Configuration Dialogs */
	"*configureGraph_popup*.title:			Configure the Graph",
	"*configureGraph*xlog.labelString:		Logarithmic Axes",
	"*configureGraph*ylog.labelString:		Logarithmic Axes",
	"*configureGraph*l1.labelString:		X Axis",
	"*configureGraph*l2.labelString:		Data 1",
	"*configureGraph*l3.labelString:		Data 2",
	"*configureGraph*l4.labelString:		Data 3",
	"*configureGraph*l5.labelString:		Data 4",
	"*configureGraph*s1.labelString:		",
	"*configureGraph*s2.labelString:		",
	"*configureGraph*s3.labelString:		",
	"*configureGraph*s4.labelString:		",
	"*configureGraph*s5.labelString:		",
	"*configureGraph*l0.labelString:		",
	"*configureGraph*r0.labelString:		Data Range",
	"*configureGraph*t0.labelString:		Data Set Name",
	"*configureGraph*s0.labelString:		Dotted Lines",
	"*configureGraph*xl.labelString:		X Axis Title",
	"*configureGraph*yl.labelString:		Y Axis Title",
/* Copy dialog */
	"*copyDialog*cap1.labelString:	From :",
	"*copyDialog*cap2.labelString:	To :",
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
/* Print Dialog */
	"*printDestinationFrameTitle.labelString:	Print Destination",
	"*printPaperFrameTitle.labelString:	Paper Size",
	"*printForm*printer.labelString:	Printer",
	"*printForm*file.labelString:	File",
	"*printForm*program.labelString:	Program",
	"*printForm*fileTFBrowse.labelString:	Browse ...",
	"*printForm*printRangeFrameTitle.labelString:	Print Range",
	"*printShell.title:		Print Dialog",
/* Default File Dialog */
	"*listSeparatorFrameTitle.labelString:	List File Separator",
	"*defaultFileShell.title:	Default File Format Dialog",
	"*defaultFormatFrameTitle.labelString: Default File Format",
/* Generic dialog buttons */
	"*ok.labelString:		OK",
	"*cancel.labelString:		Cancel",
	"*help.labelString:		Help",
/* Formats Dialog */
	"*formatsDialog_popup.title:		Formats Dialog",
	"*formatsFrameTitle.labelString:	Cell Format",
	"*formatsFrame*formatsL.labelString:	Cell Range",
	"*formatsFrame*precisionL.labelString:	Precision",
/* Print Options Dialog */
	"*showLabels.labelString:		Show Labels",
	"*showBorders.labelString:		Show Border",
	"*showRaster.labelString:		Show Raster",
/* Quit Dialog */
	"*quitMB_popup.title:			Quit Oleo ?",
	"*quitMB*OK.labelString:		Quit",
	"*quitMB*Cancel.labelString:	Don't quit",
/* XY Chart Dialog */
	"*datatab.labelString:				General Chart Data",
/* XY Chart Dialog */
	"*XYTab.labelString:				XY Chart",
	"*configureXYChartFrameTitle.labelString:	Configure XY Charts",
	"*xAutoToggle.labelString:			Automatic X Axis",
	"*xMinLabel.labelString:			Minimum X :",
	"*xMaxLabel.labelString:			Maximum X :",
	"*yAutoToggle.labelString:			Automatic Y Axis",
	"*yMinLabel.labelString:			Minimum Y :",
	"*yMaxLabel.labelString:			Maximum Y :",
	"*lineToOffscreen.labelString:			Draw line to offscreen data points",
/* Bar Chart Dialog */
	"*BarTab.labelString:				Bar Chart",
	"*configureBarChartFrameTitle.labelString:	Configure Bar Charts",
	"*stackToggle.labelString:			Stacking Bar Chart",
/* Pie Chart Dialog */
	"*PieTab.labelString:				Pie Chart",
	"*configurePieChartFrameTitle.labelString:	Configure Pie Charts",
	"*pie3d.labelString:				3D Pie Chart",
/* GNU Plotutils : list of known plotters */
	"*puPlotters:			PostScript,ps,ps;"
				"GNU MetaPlot,gmp,meta;"
				"Adobe Illustrator,ai,ai;"
				"Fig,fig,fig;"
				"PCL,pcl,pcl;"
				"HP-GL,hpgl,hpgl;"
				"Tektronix,tek,tek",
/* Graph Print Dialog */
	"*printDialog_popup.title:		Print Dialog",
	"*puselectfile_popup.title:		Select a file to print to",
/* End */
	NULL
};
