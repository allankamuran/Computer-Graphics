#ifndef _MESH_H_
#define _MESH_H_

#include "algebra.h"

typedef struct _Triangle 
{
	int vInds[3]; //vertex indices

} Triangle;

typedef struct _Material {
	Vector diffuse;
	Vector ambient;
	Vector specular;
	float shininess;
} Material;

typedef struct _Mesh 
{ 
	int nv;				
	Vector *vertices;
	Vector *vnorms;
	int nt;
	Triangle *triangles;
	struct _Mesh *next;

	/// Lab 1 Del 1.4 
	/// Egenskapade vektorer för currentMesh till del 1.4
	Vector T, R, S;

	/// Lab 2 Del 1.1 
	Vector center;
	float radius;

	Material material;

	unsigned int vbo, ibo, vao; // OpenGL handles for rendering

} Mesh;

/// Lab 2 Del 1.2 
/// Skapar en Plane struct och sätter in float variabler som vi använder i mesh.cpp och main.cpp
typedef struct Plane
{
	float a, b, c, d;

}Plane;

typedef struct _Camera 
{
	Vector position;
	Vector rotation;
	double fov; 
	double nearPlane; 
	double farPlane; 

} Camera;

void insertModel(Mesh ** objlist, int nv, float * vArr, int nt, int * tArr, float scale, Material material);

/// Lab 2 Del 1.2  
float DistanceToPoint(Plane plane, Vector p);
void NormalizePlane(Plane *plane);
void ExtractPlanesGL(Plane * p_planes, Matrix comboMatrix, bool normalize);
bool cullingFrustum(float radius, HomVector center, Plane *p_planes); 
Material createMaterial(Vector KD, Vector KA, Vector KS);

#endif
