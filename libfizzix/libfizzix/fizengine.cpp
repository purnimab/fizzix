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
//REPLACE ALL INSTANCES OF FOO AND BAR.
//BAR is the superclass if the class has one

#ifndef FIZENGINE_CPP
#define FIZENGINE_CPP

#include "fizengine.h"

/* Constructor for FizEngine
 */
FizEngine::FizEngine() 
{
	thisStep = NULL;
	nextStep = NULL;
	forces = NULL;
	props = NULL;
	ccache = NULL;	
}

/* Step and compute
 */
void FizEngine::step(	std::map<std::string, FizObject*>  * thisStep,
		       	std::map<std::string, FizObject*>  * nextStep,
		       	std::map<std::string, FizForce*>   * forces,
		       	std::map<std::string, FizFormula*> * macros,
			std::map<std::string, fizdatum>    * ccache,
		      	double dt)
{
	this->thisStep = thisStep;
	this->nextStep = nextStep;
	this->forces = forces;
	this->props = macros; // Eventually props should be renamed to macros
	this->ccache = ccache;
	this->dt = dt;
	evalForces();
}

/* Step thorugh objects
 */
void FizEngine::evalForces()
{
	if(thisStep == NULL) 
		throw std::logic_error("Nothing to work on??");
	
	std::map<std::string, FizObject*>::iterator outer_iter = thisStep->begin();
	std::map<std::string, FizObject*>::iterator inner_iter;
	std::map<std::string,FizForce*>::iterator force_iter;
	while(outer_iter != thisStep->end())
	{
		//inner_iter = thisStep->begin();
		inner_iter = outer_iter;
		inner_iter++;
		while(!(inner_iter == thisStep->end()))
		{
			if(inner_iter == outer_iter) continue;
			FizObject& obj1 = *(outer_iter->second);
			FizObject& obj2 = *(inner_iter->second);
			//TODO
			//Anton, we don't need this is you keep names up to date ...
			obj1.setName(outer_iter->first);
			obj2.setName(inner_iter->first);

			collisions(obj1, obj2);
			std::vector<triangle>& tris1 = obj1.rgetVertices(); //actually, triangles, not vertices
			std::vector<triangle>& tris2 = obj2.rgetVertices();
			vec3 dforce;
			//evalForce(forces[i], *outer_iter, *inner_iter);
			//need a COM triangle in each object
			if (obj1.comApprox()) //if object 1 can be approximated as at its COM
			{
				if (obj2.comApprox())
				{
					force_iter = forces->begin();
					while(!(force_iter == forces->end()))
					{
						FizForce* force = force_iter->second;
						std::string forcename = force_iter->first;
						if(!(*forceEvaled)[forcename])
						{
							dforce = force->getForce(obj1, obj1.rgetCOMTriangle(), obj2, obj2.rgetCOMTriangle());
							(*evaluatedForces)[&obj1].first += dforce;
							if (force->isSymmetric()) (*evaluatedForces)[&obj2].first -= dforce; //opposite direction
							else
							{
								clearNonsymmetricCaches();
								dforce = force->getForce(obj2, obj2.rgetCOMTriangle(), obj1, obj1.rgetCOMTriangle());
								((*evaluatedForces)[&obj2]).first += dforce;
							}
						}
						//no torque for obj1 nor obj2
						force_iter++;
					}
				}
				else
				{
					for (int j = 0; j < tris2.size(); j++)
					{
						force_iter = forces->begin();
						while(!(force_iter == forces->end()))
						{
							FizForce* force = force_iter->second;
							std::string forcename = force_iter->first;
							if(!(*forceEvaled)[forcename])
							{
								if (!force->isDistributed()) clearDistributedCaches();
								dforce = force->getForce(obj1, obj1.rgetCOMTriangle(), obj2, tris2[j]);
								(*evaluatedForces)[&obj1].first += dforce;
								//no torque for obj1
								if (force->isSymmetric()) (*evaluatedForces)[&obj2].first -= dforce;
								else
								{
									clearNonsymmetricCaches();
									dforce = force->getForce(obj2, tris2[j], obj1, obj1.rgetCOMTriangle());
									(*evaluatedForces)[&obj2].first += dforce;
								}
								vec3 radius = (tris2[j].vertices[0]->p + tris2[j].vertices[1]->p + tris2[j].vertices[2]->p)/3; //vector from center of object to center of triangle
								(*evaluatedForces)[&obj2].second += dforce.cross(radius); //T = F x r
							}
							force_iter++;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < tris1.size(); i++)
				{
					if (obj2.comApprox())
					{
						force_iter = forces->begin();
						while(!(force_iter == forces->end()))
						{
							FizForce* force = force_iter->second;
							std::string forcename = force_iter->first;
							if(!(*forceEvaled)[forcename])
							{
								if (!force->isDistributed()) clearDistributedCaches();
								dforce = force->getForce(obj1, tris1[i], obj2, obj2.rgetCOMTriangle());
								(*evaluatedForces)[&obj1].first += dforce;
								vec3 radius = (tris1[i].vertices[0]->p + tris1[i].vertices[1]->p + tris1[i].vertices[2]->p)/3;
								(*evaluatedForces)[&obj1].second += dforce.cross(radius);
								if (force->isSymmetric()) (*evaluatedForces)[&obj2].first -= dforce;
								else
								{
									clearNonsymmetricCaches();
									dforce = force->getForce(obj2, obj2.rgetCOMTriangle(), obj1, tris1[i]);
									(*evaluatedForces)[&obj2].first += dforce;
								}
								//no torque for obj2
							}
							force_iter++;
						}
					}
					else
					{
						for (int j = 0; j < tris2.size(); j++)
						{
							force_iter = forces->begin();
							while(!(force_iter == forces->end()))
							{
								FizForce* force = force_iter->second;
								std::string forcename = force_iter->first;
								if(!(*forceEvaled)[forcename])
								{
									if (!force->isDistributed()) clearDistributedCaches();
									dforce = force->getForce(obj1, tris1[i], obj2, tris2[j]);
									(*evaluatedForces)[&obj1].first += dforce;
									vec3 radius = (tris1[i].vertices[0]->p + tris1[i].vertices[1]->p + tris1[i].vertices[2]->p)/3;
									(*evaluatedForces)[&obj1].second += dforce.cross(radius);
									if (force->isSymmetric()) (*evaluatedForces)[&obj2].first -= dforce;
									else
									{
										clearNonsymmetricCaches();
										dforce = force->getForce(obj2, tris2[j], obj1, tris1[i]);
										(*evaluatedForces)[&obj2].first += dforce;
									}
									radius = (tris2[j].vertices[0]->p + tris2[j].vertices[1]->p + tris2[j].vertices[2]->p)/3;
									(*evaluatedForces)[&obj2].second += dforce.cross(radius);
								}
								force_iter++;
							}
						}
					}
				}
			}
			
			
			inner_iter++;
			fcache.clear();
			mcache.clear();
		}

		//DO NOT CHANGE BELOW THIS POINT
		outer_iter++;
		//DONE
	}
	std::map<FizObject*, std::pair<vec3,vec3> >::iterator iter = evaluatedForces->begin();
	while(iter!= evaluatedForces->end())
	{
		this->applyForceAndTorque(iter->second.first,iter->second.second, iter->first, this->dt);
	}
}

/** Helps by calculating L x w + Torque
 */
vec3 torque_helper(vec3 w, std::vector<double> i, vec3 t)
{
	vec3 L( (i[0] * w[0] + i[3] * w[1] + i[5] * w[2]),
		(i[3] * w[0] + i[1] * w[1] + i[4] * w[2]),
		(i[5] * w[0] + i[4] * w[1] + i[2] * w[2]));
	return (L.cross(w) + t);
} 

/** Applys the force and torque on ob1 using a Runge-Kutta foruth order solver
 */

void FizEngine::applyForceAndTorque(vec3 force, vec3 torque, FizObject * ob1, double dt)
{
	FizObject * new_object = new FizObject(*ob1);
	vec3 new_pos, new_vel, new_w;
	Quaternion new_quat;
	
	//F = dp/dt = m * dv/dt = m * a
	vec3 dvdt = force / ob1->getMass();
	//Inertia tensor invese in the order xx,yy,zz,xy,yz,xz (symmetric)
	//TODO
	//I^-1 = R * I^-1 * R^T, not I^-1
	std::vector<double> i = ob1->getInertiaTensorInv();
	vec3 t = torque_helper(ob1->getOme(),ob1->getInertiaTensor(),torque); // just for convenience
	//T = dL/dt = I * dw/dt = I * alpha
	vec3 dwdt = vec3(i[0] * t[0] + i[3] * t[1] + i[5] * t[2],
		       	 i[3] * t[0] + i[1] * t[1] + i[4] * t[2], 
			 i[5] * t[0] + i[4] + t[1] + i[2] + t[2]);
	//Step 1 
	vec3 dxdt1 = ob1->getVel();
	vec3 omega = ob1->getOme();
	Quaternion dqdt1 = (Quaternion(0.0, omega[0],omega[1], omega[2]) * ob1->getQuaternion()) * 0.5;
	new_pos = ob1->getPos() + (dxdt1 * (dt/2.0));
	new_quat = ob1->getQuaternion() + (dqdt1 * (dt/2.0));
	new_vel = ob1->getVel() + (dvdt * (dt/2.0));
	new_w = omega + (dwdt * (dt/2.0));
	
	//Step 2
  	vec3 dxdt2 = new_vel;
	Quaternion dqdt2 = (Quaternion(0.0, new_w[0],new_w[1], new_w[2]) * new_quat) * 0.5;
	new_pos = ob1->getPos() + (dxdt2 * (dt/2.0));
	new_quat = ob1->getQuaternion() + (dqdt2 * (dt/2.0));
	new_vel = ob1->getVel() + (dvdt * (dt/2.0));
	new_w = omega + (dwdt * (dt/2.0));
	
	//Step 3

  	vec3 dxdt3 = new_vel;
	Quaternion dqdt3 = (Quaternion(0.0, new_w[0],new_w[1], new_w[2]) * new_quat) * 0.5;
	new_pos = ob1->getPos() + (dxdt3 * (dt));
	new_quat = ob1->getQuaternion() + (dqdt3 * (dt));
	new_vel = ob1->getVel() + (dvdt * (dt));
	new_w = omega + (dwdt * (dt));
	
	//Step 4
  	vec3 dxdt4 = new_vel;
	Quaternion dqdt4 = (Quaternion(0.0, new_w[0],new_w[1], new_w[2]) * new_quat) * 0.5;
	new_object->setPos(ob1->getPos() + ((dxdt1 + (dxdt2 + dxdt3)*2.0 + dxdt4)*(dt/6.0)));
	new_object->setQuaternion(ob1->getQuaternion() + ((dqdt1 + (dqdt2 + dqdt3)*2.0 + dqdt4)*(dt/6.0)));
	new_object->setVel(ob1->getVel() + ((dvdt + (dvdt + dvdt)*2.0 + dvdt)*(dt/6.0)));
	new_object->setOme(ob1->getOme() + ((dwdt + (dwdt + dwdt)*2.0 + dwdt)*(dt/6.0)));
	(*nextStep)[new_object->getName()] = new_object;	
}	

void FizEngine::collisions(FizObject& obj1, FizObject& obj2)
{
	vec3 radius = obj1.getPos() - obj2.getPos();
	double distance = radius.mag();
	vec3 direction = radius/radius.mag();
	if (distance <= obj1.getMaxRad() + obj2.getMaxRad()) //if within their bounding spheres
	{
		//TODO: Check if actually colliding and where
		//TODO: Apply collision forces
	}
}

// Get from cache or evaluate:	
fizdatum FizEngine::getForceVal(const std::string& force, const FizObject& obj1, const triangle& tri1, const FizObject& obj2, const triangle& tri2)
{	
	if(contains(fcache,force))
	{		
		fizdatum& cachedVal=fcache[force];
		if(cachedVal.type==INPROGRESS)
		{
			throw std::logic_error("No circular references, please");
		}
		return cachedVal;
	}
	if(!contains(*forces,force))
	{
		throw std::logic_error("Invalid force");
	}
	fizdatum& cachedVal=fcache[force];
	cachedVal.type=INPROGRESS;
	(*forceEvaled)[force] = true;
	return cachedVal=(*forces)[force]->getForce(obj1, tri1, obj2, tri2);
}

fizdatum FizEngine::getMacroVal(const std::string& macro, const FizObject& obj1, const triangle& tri1, const FizObject& obj2, const triangle& tri2)
{	
	if(contains(mcache,macro))
	{		
		fizdatum& cachedVal=mcache[macro];
		if(cachedVal.type==INPROGRESS)
		{
			throw std::logic_error("No circular references, please");
		}
		return cachedVal;
	}
	if(!contains(*props,macro))
	{
		throw std::logic_error("Invalid macro");
	}
	fizdatum& cachedVal=mcache[macro];
	cachedVal.type=INPROGRESS;
	return cachedVal=(*props)[macro]->eval(obj1, tri1, obj2, tri2);
}

fizdatum FizEngine::getConstVal(const std::string& constant)
{
	if(!contains(*ccache,constant))
	{
		throw std::logic_error("Invalid constant");
	}
	return (*ccache)[constant];
}

void FizEngine::clearDistributedCaches()
{
	for(std::set<std::string>::const_iterator it = macrodist.begin(); it != macrodist.end(); ++it)
	{
		mcache.erase(*it);
	}
	for(std::set<std::string>::const_iterator it = forcedist.begin(); it != forcedist.end(); ++it)
	{
		fcache.erase(*it);
	}
}

void FizEngine::clearNonsymmetricCaches()
{
	for(std::set<std::string>::const_iterator it = macrosymmetric.begin(); it != macrosymmetric.end(); ++it)
	{
		mcache.erase(*it);
	}

	for(std::set<std::string>::const_iterator it = forcesymmetric.begin(); it != forcesymmetric.end(); ++it)
	{
		fcache.erase(*it);
	}
}
#endif
