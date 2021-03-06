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
#include <map>
#include <stdexcept>
#include <algorithm>
#include <set>

#include "fizforce.h"
//class FizForce;
//#include "fizformula.h"
#include "fizobject.h"
#include "gen_structs.h"

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
		std::map<std::string, FizObject*> * thisStep;
		
		//Vector to store new object information in
		std::map<std::string, FizObject*> * nextStep;
		
		//Map of object pointers to their forces and torques
		std::map<FizObject*, std::pair<vec3,vec3> > * evaluatedForces;
		
		//Map of forces to be applied to the objects
		std::map<std::string, FizForce *> * forces;
		
		//Whether a force has already been applied by another force or by the engine
		std::map<std::string, bool> * forceEvaled;
		
		//Map of properties
		std::map<std::string, FizFormula *> * props;

		//How much time does this step represent
		double dt;

		//TODO: Make sure that 
		/** Loop through every object, and then
		 *  triangles, and then forces calling evalForce on it
		 *  @note The forces looped through are those passed into step
		 */
		void evalForces();
		
		/** Evaluate a force on every possible permutation (size 2) of FizObjects 
		 *  @param force A pointer to the force to be evaluated
		 *  @note The forces and objects must be inititalized via step
		 */
		//void evalForce(FizForce * force, FizObject * o1, FizObject * o2);
		
		/** Checks for collisions between two objects and if it exists, applies a collision reaction force between whatever triangles of theirs have collided
		*/
		void collisions(FizObject& obj1, FizObject& obj2);
		void collide(FizObject& obj1, triangle& tri1, FizObject& obj2, triangle& tri2, vec3 normal, point p);
		void collisionDetect(FizObject& obj1, FizObject& obj2, vec3 direction, triangle& tri1, triangle& tri2);

		/** Applys the force and torque on ob1 using a Runge-Kutta foruth order solver
		* 
		*  Some math/physics notes:

		*  accel = Force / mass = dp/dt = m * dv/dt
		*  alpha = I_inverse * (L x w + Torque) (where L is the angular momentum = Iw , w is the angular velocity,
		*                                         and Torque is a 3x1 representation of a vector)
		*  ^^^ Derivation is not fun for alpha
		*  
		*  I_inverse is a 3x3 symmetric matrix stored as a double[6] of the form:
		*  [0 3 5]
		*  [3 1 4] where the number indicates the array index
		*  [5 4 2]
		*
		*  Linear Motion: simple really

		*  position = integral(velocity), velocity = integral(acceleration)   
		*
		*  Rotational Motion: not so simple really

		*  quaternion = 1/2 * omega * current_quaternion
		*  where omega can be seen as a quaternion (w,x,y,z) of the form (0,wx,wy,wz)
		*  omega = integral(alpha)  ... simple eh ... oh wait, look at how to get alpha
		*
		*  Runga Kutta:

		*  t = time
		*  dt = delta t
		*  xi = initial position
		*  xf = final position
		*
		*  k1 = f'(t,xi)
		*  k2 = f'(t + dt/2, xi + k1/2)
		*  k3 = f'(t + dt/2, xi + k2/2)
		*  k4 = f'(t + dt, xi + k3)
		*
		*  xf = xi + (k1 + 2k2 + 2k3 + k4) / 6
		*
		*  For this simulation a(t) = a(t+.5dt) = a(t+dt);
		
		*  @param force The force to apply
		*  @param torque The torque to apply
		*  @param ob1 The object to modify
		*  @param dt The time period over which this force/torque is applied
		*/
		void applyForceAndTorque(vec3 force, vec3 torque, FizObject * ob1, double dt);

		//Cache of Forces and Macros
		std::map<std::string, fizdatum> fcache;
		std::map<std::string, fizdatum> mcache;
	public:
		/** Steps the physics engine, calculating and storing the next step
		 *  @param thisStep A pointer to a vector of FizObjects representing the current state
		 *  @param nextStep A pointer to a vector of FizObjects to be used to store the next step
		 *  @param forces A list of forces to be applied between FizObjects
		 *  @param macros A list of macros (formulas) that are used in evaluation
		 *  @param ccache A list of all constants used in calculation 
		 *  @param dt The time step this step represents
		 *  
		 *  @note This function modifies the nextStep vector IN PLACE
		 */
		void step(
			std::map<std::string, FizObject*> * thisStep,
		       	std::map<std::string, FizObject*> * nextStep,
		       	std::map<std::string, FizForce*> * forces,
		       	std::map<std::string, FizFormula*> * macros,
			std::map<std::string, fizdatum> * ccache,
		      	double dt);		
		
		/** Constructor
		 */
		FizEngine();

		//List of current sums of forces and torques
		std::map<std::string, vec3[2]> forcesums;
	
		//Constant cache
		std::map<std::string, fizdatum> * ccache;

		// Sets of important things
		std::set<std::string> propdist; //Distributed object properties
		std::set<std::string> macrodist; // All distributed macros
		std::set<std::string> forcedist; // All distributed forces
		std::set<std::string> macrosymmetric; // All symmetric macros
		std::set<std::string> forcesymmetric; // All symmetric forces

		// These either retreive from cache or eval
		fizdatum getForceVal(const std::string& force, const FizObject& obj1, const triangle& tri1, const FizObject& obj2, const triangle& tri2);
		fizdatum getMacroVal(const std::string& macro, const FizObject& obj1, const triangle& tri1, const FizObject& obj2, const triangle& tri2);
		fizdatum getConstVal(const std::string& constant);
		fizdatum getPropVal(const std::string& objectname, const std::string& identifier);

		// True = Thing is cached, false = Thing is not cached
		template< class T >	
		bool contains(const std::map<std::string, T>& cache, const std::string& key)
		{





			return cache.count(key) != 0;
		}

		void clearDistributedCaches(); // Clears distributed properties from cache
		void clearNonsymmetricCaches();
};

#endif




