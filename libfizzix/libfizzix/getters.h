/***********************************************************************************************
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
#ifndef GETTERS_H
#define GETTERS_H

/*
???
#include <vector>
#include <stack>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cstdlib>
*/

#include "fizengine.h" //already forward declared in fizformnode
#include "fizformget.h"

class FizFormGetMacro : public FizFormGet
{
	public:
		FizFormGetMacro(std::string id);
		const fizdatum eval(fizstack &stack, const FizObject &obj1, const triangle &tri1, const FizObject &obj2, const triangle &tri2);
		const std::string toString(fizstack& stack);
};

class FizFormGetForce : public FizFormGet
{
	public:
		FizFormGetForce(std::string id);
		const fizdatum eval(fizstack &stack, const FizObject &obj1, const triangle &tri1, const FizObject &obj2, const triangle &tri2);
		const std::string toString(fizstack& stack);
};

class FizFormGetConst : public FizFormGet
{
	public:
		FizFormGetConst(std::string id);
		const fizdatum eval(fizstack &stack, const FizObject &obj1, const triangle &tri1, const FizObject &obj2, const triangle &tri2);
		const std::string toString(fizstack& stack);
};

class FizFormGetProp : public FizFormGet
{
	private:
		std::string objectname;
		bool firstobject;
	public:
		FizFormGetProp(std::string id);
		FizFormGetProp(std::string id, bool first);
		FizFormGetProp(std::string id, std::string name);
		const fizdatum eval(fizstack &stack, const FizObject &obj1, const triangle &tri1, const FizObject &obj2, const triangle &tri2);
		const std::string toString(fizstack& stack);
};

#endif
