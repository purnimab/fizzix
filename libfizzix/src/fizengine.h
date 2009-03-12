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
#ifndef FIZENGINE_H
#define FIZENGINE_H

#include <iostream>
#include <vector>

/* For whenever these get written
#include "fizobject.h"
#include "fizforce.h"

Forward declarations untill then
*/
class FizObject;
class FizForce;

/** @class FizEngine fizengine.h "libfizzix/src/fizengine.h"
 *  @brief This class computes object positions from the last step 
 *  
 *  The FizEngine class is in charge of managing all the objects and forces
 *  within the physics simulator.  Each time step someone calls step with the 
 *  current objects in the simulation and where to store the results of this
 *  step, the engine loops through each force and applies them to each object
 */
class FizEngine 
{
	private:
		//Vector containing current object information
		vector<FizObject*> * thisStep;
		
		//Vector to store new object information in
		vector<FizObject*> * nextStep;
		
		//Vector of forces to be applied to the objects
		vector<FizForce*> * forces;		
		
		//How much time does this step represent
		double dt;

		/** Loop through every force calling evalForce on it
		 *  @note The forces looped through are those passed into step
		 */
		void evalForces();
		
		/** Evaluate a force on every possible permutation (size 2) of FizObjects 
		 *  @param force A pointer to the force to be evaluated
		 *  @note The forces and objects must be inititalized via step
		 */
		void evalForce(FizeForce * force);
	public:
		/** Steps the physics engine, calculating and storing the next step
		 *  @param thisStep A pointer to a vector of FizObjects representing the current state
		 *  @param nextStep A pointer to a vector of FizObjects to be used to store the next step
		 *  @param forces A list of forces to be applied between FizObjects
		 *  @param dt The time step this step represents
		 *  
		 *  @note This function modifies the nextStep vector IN PLACE
		 */
		void step(vector<FizObject*> * thisStep, vector<FizObject*> * nextStep, vector<FizForce*> * forces, double dt);		
		
		/** Constructor
		 */
		FizEngine();
};

#endif



