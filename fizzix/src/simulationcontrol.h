/************************************************************************************************
This file is a part of the Fizzix project, which provides a physics engine and the visualization.
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


#ifndef SIMULATIONCONTROL_H
#define SIMULATIONCONTROL_H
#include <QPushButton>
#include <QLabel>
#include <QDockWidget>
#include <QDesktopWidget>
#include <QDoubleSpinBox>
#include <QPalette>
#include <QGridLayout>
#include <QColor>
#include "databackend/stepengine.h"

/*@class SimulationControl
 @brief Provides a panel to control Simulation running
 *  This dockable panel allows control of the simulation. It interfaces with the
 *  backend and can start, stop, reset, and change the dt. It provides a box
 *  that shows the status of the fizengine.
*/

class SimulationControl:public QDockWidget
{	Q_OBJECT
	private:
		bool running;
		DataBackend * db;
		StepEngine * eng; 
		QLabel * status; //Displays syntax and engine errors.
		QDoubleSpinBox * dt;
		QPushButton * start;
		QPushButton * stop;
		QPushButton * reset;
	public:
		//Constructs a widget to control the StepEngine, which contains the FizEngine.
		SimulationControl (DataBackend * _db);
		//Retrieves the StepEngine.
		StepEngine * getStepEngine();
	public slots:
		//Displays and colors the status QLabel based on whether the new String is an error or not.
		void statusChanged(QString newString, int errorSource);
		//Disconnects the reset button, which should not notice that new data is being added until the simulation stops running.
		void connectReset(bool disconn);
};

#endif
