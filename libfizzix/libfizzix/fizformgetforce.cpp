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
#ifndef FIZFORMGETFORCE_CPP
#define FIZFORMGETFORCE_CPP

#include "getters.h"

using namespace std;

FizFormGetForce::FizFormGetForce(std::string id)
{
	identifier=id;
}

const fizdatum FizFormGetForce::eval(fizstack &stack, const FizObject &obj1, const triangle &tri1, const FizObject &obj2, const triangle &tri2)
{
	return eng->getForceVal(identifier, obj1, tri1, obj2, tri2); // gets force value from engine
}

const std::string FizFormGetForce::toString(fizstack& stack)
{
	return "{"+identifier+"}";
}

#endif
