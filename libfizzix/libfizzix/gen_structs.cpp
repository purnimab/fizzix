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
#ifndef GEN_STRUCTS_CPP
#define GEN_STRUCTS_CPP

#include "gen_structs.h"
#include <stdexcept>
#include <iostream>

/******************** VEC3 ********************/
vec3::vec3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}

vec3::vec3() : x(0.0), y(0.0), z(0.0) {}
const double vec3::operator[](int index) const
{
	if (index == 0) return x;
	if (index == 1) return y;
	if (index == 2) return z;
}

double & vec3::operator[](int index)
{
	if (index == 0) return x;
	if (index == 1) return y;
	if (index == 2) return z;
}

vec3 vec3::operator*(const vec3 & other) const
{
	return vec3(x * other[0], y * other[1], z * other[2]);
}

vec3 vec3::operator*(const double & other) const
{
	return vec3(x * other, y * other, z * other);
}

vec3 vec3::operator/(const vec3 & other) const
{
	return vec3(x / other[0], y / other[1], z / other[2]);
}

vec3 vec3::operator/(const double & other) const
{
	return vec3(x / other, y / other, z / other);
}

vec3 vec3::operator+(const vec3 & other) const
{
	return vec3(x + other[0], y + other[1], z + other[2]);
}

vec3 vec3::operator-(const vec3 & other) const
{
	return vec3(x - other[0], y - other[1], z - other[2]);
}

vec3 vec3::operator*=(const vec3 & other)
{
	x *= other[0];
	y *= other[1];
	z *= other[2];
	return *this;
}

vec3 vec3::operator*=(const double & other)
{
	x *= other;
	y *= other;
	z *= other;
	return *this;
}

vec3 vec3::operator/=(const vec3 & other)
{
	x /= other[0];
	y /= other[1];
	z /= other[2];
	return *this;
}

vec3 vec3::operator/=(const double & other)
{
	x /= other;
	y /= other;
	z /= other;
	return *this;
}

vec3 vec3::operator+=(const vec3 & other)
{
	x += other[0];
	y += other[1];
	z += other[2];
	return *this;
}

vec3 vec3::operator-=(const vec3 & other)
{
	x -= other[0];
	y -= other[1];
	z -= other[2];
	return *this;
}

double vec3::dot(const vec3& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

vec3 vec3::cross(const vec3& other) const
{
	return vec3(y * other.z - z * other.y,
		    z * other.x - x * other.z,
		    x * other.y - y * other.x);
}

double vec3::mag() 
{
	return sqrt(x * x + y * y + z * z);
}

/******************** VEC3 ********************/

/******************** VEC4 ********************/ 
vec4::vec4(double _x, double _y, double _z, double _w) : x(_x), y(_y), z(_z), w(_w) {}
vec4::vec4() : x(0.0), y(0.0), z(0.0), w(0.0) {}

const double vec4::operator[](int index) const
{
	if (index == 0) return w;
	if (index == 1) return x;
	if (index == 2) return y;
	if (index == 3) return z;
}

double & vec4::operator[](int index)
{
	if (index == 0) return w;
	if (index == 1) return x;
	if (index == 2) return y;
	if (index == 3) return z;
}

vec4 vec4::dot(const vec4& other) 
{
	return vec4(x * other.x, y * other.y, z * other.z, w * other.w);
}

vec4 vec4::cross(const vec4& other) {}
/* Cross products in 4space require 3 vectors to not have infinite results
 * How to deal with this ...
{
	return vec4(y * other.z - z * other.y,
		    z * other.x - x * other.z,
		    x * other.y - y * other.x)
}
*/

double vec4::mag()
{
	return sqrt(x * x + y * y + z * z + w * w);
}
/******************** VEC4 ********************/

/******************* VERTEX *******************/
vertex::vertex(double _x, double _y, double _z) : p(vec3(_x, _y, _z)) {};

vertex::vertex() : p(vec3()) {}

const double vertex::operator[](int index) const
{
	return p[index];
}

double& vertex::operator[](int index)
{
	return p[index];
}

const triangle vertex::operator()(int index) const
{
	if (triangles.size() > index)
	{
		return *triangles[index];
	}
	else
	{
		throw std::out_of_range("Index out of range");
	}
}

triangle& vertex::operator()(int index)
{
	if (triangles.size() > index)
	{
		return *triangles[index];
	}
	else
	{
		throw std::out_of_range("Index out of range");
	}
}

void vertex::add_triangle(triangle * t) 
{
	triangles.push_back(t);
}

void vertex::clear_triangles()
{
	triangles.clear();
}

/******************* VERTEX *******************/

/****************** TRIANGLE ******************/
triangle::triangle(vertex* v1, vertex* v2, vertex* v3, bool add)
{
	vertices[0] = v1; vertices[1] =  v2; vertices[2] = v3;
	vertex& vr1 = *v1;vertex& vr2 = *v2;vertex& vr3 = *v3;
	massp = 0.0;
	vec3 e1 = vr2.p - vr1.p;
	vec3 e2 = vr3.p - vr1.p;

	normal = e1.cross(e2);
	double mag = normal.mag();
	unit_normal = normal/mag;
	if (add)
	{
		v1->add_triangle(this);
		v2->add_triangle(this);
		v3->add_triangle(this);
	}
}

triangle::triangle()
{
	vertices[0] = new vertex(); vertices[1] = new vertex(); vertices[2] = new vertex();
	massp = 0.0;
}

const vertex triangle::operator[](int index) const
{
	if (index < 3)
	{
		return *vertices[index];
	}
	else
	{
		throw std::out_of_range("Index out of range");
	}
}

vertex& triangle::operator[](int index)
{
	if (index < 3)
	{
		return *vertices[index];
	}
	else
	{
		throw std::out_of_range("Index out of range");
	}
}
/****************** TRIANGLE ******************/

/****************** FIZDATUM ******************/
fizdatum::fizdatum(double s, vec3 v, Type t) : scalar(s), vector(v), type(t) {}
fizdatum::fizdatum(double s) : scalar(s), type(SCALAR) {}
fizdatum::fizdatum(vec3 v) : vector(v), type(VECTOR) {}
fizdatum::fizdatum() : scalar(0), vector(vec3()), type(NIL) {}
/****************** FIZDATUM ******************/

/****************** FIZDATUM ******************/
collision::collision(point pt,vec3 vec) : collision_point(pt),vector(vec) {}
collision::collision() {}
/****************** FIZDATUM ******************/

#endif
