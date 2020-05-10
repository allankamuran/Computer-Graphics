#ifndef _ALGEBRA_H_
#define _ALGEBRA_H_

typedef struct 
{ 
	float x, y, z; 

} Vector;

typedef struct 
{ 
	float x, y, z, w;

} HomVector;

/* Column-major order are used for the matrices here to be compatible with OpenGL.
** The indices used to access elements in the matrices are shown below.
**  _                _
** |                  |
** |   0   4   8  12  |
** |                  |
** |   1   5   9  13  |
** |                  |
** |   2   6  10  14  |
** |                  |
** |   3   7  11  15  |
** |_                _|
*/

typedef struct matrix 
{ 
	float e[16]; 

} Matrix;

Vector Add(Vector a, Vector b);
Vector Subtract(Vector a, Vector b);
Vector CrossProduct(Vector a, Vector b);
Vector Normalize(Vector a);
Vector ScalarVecMul(float t, Vector a);
Vector Homogenize(HomVector a);
HomVector MatVecMul(Matrix a, Vector b);
Matrix MatMatMul(Matrix a, Matrix b);
float DotProduct(Vector a, Vector b);
float Length(Vector a);
void PrintMatrix(char *name, Matrix m);
void PrintVector(char *name, Vector v);
void PrintHomVector(char *name, HomVector h);

/// Lab 1 Del 1.1 - 1.2
Matrix TranslationMatrix(float x, float y, float z);
Matrix RotationMatrix(float x, float y, float z);
Matrix ScalingMatrix(float x, float y, float z);
Matrix PerpectiveMatrix(float fov, float near, float far, float aspect);
Matrix ParallellMatrix(float near, float far, float top, float bottom, float left, float right);
float Distance(Vector a, Vector b);

#endif

