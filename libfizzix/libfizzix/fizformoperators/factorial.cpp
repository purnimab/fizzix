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
#ifndef FACTORIAL_CPP
#define FACTORIAL_CPP

#include "operators.h"
using namespace FizOper;
std::string Factorial::token = "!";

Factorial::Factorial(int numOperands)
{
	this->numOperands=numOperands;
	description = "Takes the factorial of a scalar.";
}

const fizdatum Factorial::eval(fizstack &stack, const FizObject &obj1, const triangle &tri1, const FizObject &obj2, const triangle &tri2)
{
	fizdatum a = stack.pop()->eval(stack, obj1, tri1, obj2, tri2);
	fizdatum c;
	c.type = SCALAR;
	if (numOperands == 1 && a.type == SCALAR && a.scalar >= 0)
	{
		c.scalar = 1;
		for (int i = 2; i <= a.scalar; i++) c.scalar *= i;
	}
	else if (a.type == NIL) c = fizdatum();
	else throw std::logic_error("Cannot take the factorial of multiple numbers, negative numbers, or vectors.");
	return c;
}

const std::string Factorial::toString(fizstack& stack)
{
	std::string val = "";
	for(int i=0;i<numOperands;i++)
	{
		val = " "+stack.pop()->toString(stack) + val;
	}
	val = "("+token+val+")";
	return val;
}

#endif

