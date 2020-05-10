#define _USE_MATH_DEFINES // To get M_PI defined
#include <math.h>
#include <stdio.h>
#include "algebra.h"

Vector CrossProduct(Vector a, Vector b) 
{
	Vector v = { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
	return v;
}

float DotProduct(Vector a, Vector b) 
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector Subtract(Vector a, Vector b) 
{
	Vector v = { a.x-b.x, a.y-b.y, a.z-b.z };
	return v;
}    

Vector Add(Vector a, Vector b) 
{
	Vector v = { a.x+b.x, a.y+b.y, a.z+b.z };
	return v;
}    

float Length(Vector a) 
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

float Distance(Vector a, Vector b)
{
	return sqrt(pow((b.x - a.x), 2.0) + pow((b.y - a.y), 2.0) + pow((b.z - a.z), 2.0));
}

Vector Normalize(Vector a) 
{
	float len = Length(a);
	Vector v = { a.x/len, a.y/len, a.z/len };
	return v;
}

Vector ScalarVecMul(float t, Vector a) 
{
	Vector b = { t*a.x, t*a.y, t*a.z };
	return b;
}

HomVector MatVecMul(Matrix a, Vector b) 
{
	HomVector h;
	h.x = b.x*a.e[0] + b.y*a.e[4] + b.z*a.e[8] + a.e[12];
	h.y = b.x*a.e[1] + b.y*a.e[5] + b.z*a.e[9] + a.e[13];
	h.z = b.x*a.e[2] + b.y*a.e[6] + b.z*a.e[10] + a.e[14];
	h.w = b.x*a.e[3] + b.y*a.e[7] + b.z*a.e[11] + a.e[15];
	return h;
}

Vector Homogenize(HomVector h) 
{
	Vector a;
	if (h.w == 0.0) 
	{
		fprintf(stderr, "Homogenize: w = 0\n");
		a.x = a.y = a.z = 9999999;
		return a;
	}
	a.x = h.x / h.w;
	a.y = h.y / h.w;
	a.z = h.z / h.w;
	return a;
}

Matrix MatMatMul(Matrix a, Matrix b) 
{
	Matrix c;
	int i, j, k;

	for (i = 0; i < 4; i++) 
	{
		for (j = 0; j < 4; j++) 
		{
			c.e[j*4+i] = 0.0;

			for (k = 0; k < 4; k++)
			{
				c.e[j * 4 + i] += a.e[k * 4 + i] * b.e[j * 4 + k];
			}
		}
	}
	return c;
}

void PrintVector(char *name, Vector a) 
{
	printf("%s: %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z);
}

void PrintHomVector(char *name, HomVector a) 
{
	printf("%s: %6.5lf %6.5lf %6.5lf %6.5lf\n", name, a.x, a.y, a.z, a.w);
}

void PrintMatrix(char *name, Matrix a) 
{ 
	int i,j;

	printf("%s:\n", name);
	for (i = 0; i < 4; i++) 
	{
		for (j = 0; j < 4; j++) 
		{
			printf("%6.5lf ", a.e[j*4+i]);
		}
		printf("\n");
	}
}

/// ********************************** MY OWN CODE ********************************** /// 

/// Lab 1 Del 1.1
/// Den här funktionen har en translations matris 
Matrix TranslationMatrix(float x, float y, float z)
{
	Matrix V;

	V.e[0] = 1.0f; V.e[4] = 0.0f; V.e[8 ] = 0.0f; V.e[12] = x;
	V.e[1] = 0.0f; V.e[5] = 1.0f; V.e[9 ] = 0.0f; V.e[13] = y;
	V.e[2] = 0.0f; V.e[6] = 0.0f; V.e[10] = 1.0f; V.e[14] = z;
	V.e[3] = 0.0f; V.e[7] = 0.0f; V.e[11] = 0.0f; V.e[15] = 1.0f;

	return V;
}

/// Lab 1 Del 1.1
/// Den här funktionen har en roations matris
Matrix RotationMatrix(float x, float y, float z)
{
	Matrix X;
	Matrix Y;
	Matrix Z;

	X.e[0] = 1.0f; X.e[4] = 0.0f;           X.e[8] = 0.0f;           X.e[12] = 0.0f; 
	X.e[1] = 0.0f; X.e[5] = (float)cos(x);  X.e[9] = (float)sin(x);  X.e[13] = 0.0f; 
	X.e[2] = 0.0f; X.e[6] = (float)-sin(x); X.e[10] = (float)cos(x); X.e[14] = 0.0f; 
	X.e[3] = 0.0f; X.e[7] = 0.0f;           X.e[11] = 0.0f;          X.e[15] = 1.0f;

	Y.e[0] = (float)cos(y); Y.e[4] = 0.0f; Y.e[8] = (float)-sin(y); Y.e[12] = 0.0f;
	Y.e[1] = 0.0f;          Y.e[5] = 1.0f; Y.e[9] = 0.0f;           Y.e[13] = 0.0f;
	Y.e[2] = (float)sin(y); Y.e[6] = 0.0f; Y.e[10] = (float)cos(y); Y.e[14] = 0.0f;
	Y.e[3] = 0.0f;          Y.e[7] = 0.0f; Y.e[11] = 0.0f;          Y.e[15] = 1.0f;

	Z.e[0] = (float)cos(z);  Z.e[4] = (float)sin(z); Z.e[8] = 0.0f;  Z.e[12] = 0.0f;
	Z.e[1] = (float)-sin(z); Z.e[5] = (float)cos(z); Z.e[9] = 0.0f;  Z.e[13] = 0.0f;
	Z.e[2] = 0.0f;           Z.e[6] = 0.0f;          Z.e[10] = 1.0f; Z.e[14] = 0.0f;
	Z.e[3] = 0.0f;           Z.e[7] = 0.0f;          Z.e[11] = 0.0f; Z.e[15] = 1.0f;

	return MatMatMul(X, MatMatMul(Y, Z));
}

/// Lab 1 Del 1.1
/// Den här funktionen har en scaling matris
Matrix ScalingMatrix(float x, float y, float z)
{
	Matrix V;

	V.e[0] = x;			V.e[4] = 0.0f;		V.e[8 ] = 0.0f;		V.e[12] = 1.0f;
	V.e[1] = 0.0f;		V.e[5] = y;			V.e[9 ] = 0.0f;		V.e[13] = 1.0f;
	V.e[2] = 0.0f;		V.e[6] = 0.0f;		V.e[10] = z;		V.e[14] = 1.0f;
	V.e[3] = 0.0f;		V.e[7] = 0.0f;		V.e[11] = 0.0f;		V.e[15] = 1.0f;

	return V;
}

/// Lab 1 Del 1.2
/// Den här funktionen gör att den visat verkligen i 3D och den är mycket bättre än den första funktionen 
Matrix PerpectiveMatrix(float fov, float near, float far, float aspect)
{
	Matrix V;

	double cotan = fov / 2.0f;
	
	V.e[0] = cotan/aspect; V.e[4] = 0.0f;  V.e[8 ] = 0.0f;                  V.e[12] = 0.0f;
	V.e[1] = 0.0f;         V.e[5] = cotan; V.e[9 ] = 0.0f;                  V.e[13] = 0.0f;
	V.e[2] = 0.0f;         V.e[6] = 0.0f;  V.e[10] = (far+near)/(near-far); V.e[14] = (2.0f*far*near)/(near-far);
	V.e[3] = 0.0f;         V.e[7] = 0.0f;  V.e[11] = -1.0f;                 V.e[15] = 0.0f;

	return V;
}

/// Lab 1 Del 1.2
/// Den här visar objektet i 2D som att man skulle rita på "papper"
Matrix ParallellMatrix(float near, float far, float top, float bottom, float left, float right)
{
	Matrix V;

	V.e[0] = 2 / (right - left); V.e[4] = 0.0f;               V.e[8] = 0.0f;               V.e[12] = -((right + left) / (right - left));
	V.e[1] = 0.0f;               V.e[5] = 2 / (top - bottom); V.e[9] = 0.0f;		       V.e[13] = -((top + bottom) / (top - bottom));
	V.e[2] = 0.0f;               V.e[6] = 0.0f;               V.e[10] = -2 / (near - far); V.e[14] = -((far + near) / (far - near));
	V.e[3] = 0.0f;               V.e[7] = 0.0f;               V.e[11] = 0.0f;              V.e[15] = 1.0f;

	return V; 
}