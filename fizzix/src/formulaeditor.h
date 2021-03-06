/************************************************************************************************
This file is a part of libfizzix backend to the Fizzix project.
The website for this project is http://code.google.com/p/fizzix .
Copyright (C) 2009
	Joseph Lynch
	Anton Frolenkov
	Purnima Balakrishnan
	Daniel Stiles
	Eric Van Albert

This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
************************************************************************************************/
#ifndef FORMULAEDITOR_H
#define FORMULAEDITOR_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <libfizzix/fizformula.h>
#include "databackend/databackend.h"

// This is very similar to the ForceEditor, but is for creating macros instead.
class FormulaEditor:public QWidget
{	Q_OBJECT
	private:
		DataBackend * db;
		bool hChanges;
		QTextEdit * formEdit;
		FizFormula tempData;
	public:
		//Creates a widget to edit a formula.
		FormulaEditor(DataBackend * _db);
		//Returns true if there are changes.
		bool hasChanges();
		//Returns the data.
		FizFormula * getData();
	public slots:
		//User input response.
		void changes();
		//Loads a formula.
		void setData(FizFormula f);
		//Shows a help dialog with formulas. Currently unimplemented.
		void showAvailableSymbolsDialog();
};

#endif
