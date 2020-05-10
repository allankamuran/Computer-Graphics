#include <stdlib.h>
#include "mesh.h"
#include "math.h"

void insertModel(Mesh **list, int nv, float * vArr, int nt, int * tArr, float scale, Material material)
{
	Mesh * mesh = (Mesh *)malloc(sizeof(Mesh));
	Vector center = { 0, 0, 0 };
	mesh->nv = nv;
	mesh->nt = nt;
	mesh->vertices = (Vector *)malloc(nv * sizeof(Vector));
	mesh->vnorms = (Vector *)malloc(nv * sizeof(Vector));
	mesh->triangles = (Triangle *)malloc(nt * sizeof(Triangle));

	/// Lab 1 Del 1.4
	/// För vektorerna i strukten som finns i mesh.h 
	/// Man måste intialisera vektorerna annars funkar inte del 1.4
	mesh->T = { 0.0f, 0.0f, 20.0f };
	mesh->R = { 0.0f, 0.0f, 0.0f };
	mesh->S = { 1.0f, 1.0f, 1.0f };

	// set mesh material
	mesh->material = material;

	// set mesh vertices
	for (int i = 0; i < nv; i++)
	{
		mesh->vertices[i].x = vArr[i * 3] * scale;
		mesh->vertices[i].y = vArr[i * 3 + 1] * scale;
		mesh->vertices[i].z = vArr[i * 3 + 2] * scale;
	}

	// set mesh triangles
	for (int i = 0; i < nt; i++)
	{
		mesh->triangles[i].vInds[0] = tArr[i * 3];
		mesh->triangles[i].vInds[1] = tArr[i * 3 + 1];
		mesh->triangles[i].vInds[2] = tArr[i * 3 + 2];
	}

	/// Assignment 1: 
	/// Calculate and store suitable vertex normals for the mesh here.
	/// Replace the code below that simply sets some arbitrary normal values

	/// Lab 1 Del 1.3
	/// Jag måste skapa normal function av varje vektor punkt
	/// Beräkna varje punkt i triangeln i mesh.cpp
	for (int i = 0; i < nv; i++)
	{
		Vector temp = { 0, 0, 0 };

		for (int j = 0; j < nt; j++)
		{
			for (int l = 0; l < 3; l++)
			{
				if (mesh->triangles[j].vInds[l] == i)
				{
					/// Get 2 vectors between a triangles vertices
					/// Hämta 2 vektorer a triangels vertex
					Vector v1 = Subtract(mesh->vertices[mesh->triangles[j].vInds[1]], mesh->vertices[mesh->triangles[j].vInds[0]]);
					Vector v2 = Subtract(mesh->vertices[mesh->triangles[j].vInds[2]], mesh->vertices[mesh->triangles[j].vInds[0]]);

					/// Räknar ut tranglens normal
					Vector normal = CrossProduct(v1, v2);
					normal = Normalize(normal);

					/// Lagrar all tranglar normaler 
					temp = Add(temp, normal);
					break;
				}
			}
		}
		mesh->vnorms[i] = Normalize(temp);
	}

	/// Lab 2 Del 1.1
	/// Sumerar värdet av punkter och hittar center 
	for (int i = 0; i < mesh->nv; i++) 
	{
		center.x += mesh->vertices[i].x;
		center.y += mesh->vertices[i].y;
		center.z += mesh->vertices[i].z;
	}

	/// Lab 2 Del 1.1
	/// Delar alla värden på antal vertexes
	center.x /= mesh->nv;
	center.y /= mesh->nv;
	center.z /= mesh->nv;

	mesh->center = { center.x, center.y, center.z };

	float distance = 0.0f;
	float tempDistance = 0.0f;

	/// Lab 2 Del 1.1
	/// Hittar den längre sidan av objektet 
	for (int i = 0; i < mesh->nv; i++)
	{
		Vector temp = mesh->vertices[i];

		temp = Subtract(temp, mesh->center);
		tempDistance = Length(temp);

		if (tempDistance >= distance)
		{
			distance = tempDistance;
		}
	}

	/// Lab 2 Del 1.1
	/// Här så sätter jag radius till distance
	mesh->radius = distance;

	mesh->next = *list;
	*list = mesh;	
}

/// Lab 2 Del 1.2 
/// Den här funktionen hittar längden till punkten från plane till alla andra sidor 
float DistanceToPoint(Plane plane, Vector p)
{
	return plane.a*p.x + plane.b*p.y + plane.c*p.z + plane.d;
}

/// Lab 2 Del 1.2 
/// Den här funktionen normalizerar alla sidor på planen så den inte flyger iväg annars blir frustum helt fel 
void NormalizePlane(Plane *plane)
{
	float mag;
	mag = sqrt(plane->a * plane->a + plane->b * plane->b + plane->c * plane->c);
	plane->a = plane->a / mag;
	plane->b = plane->b / mag;
	plane->c = plane->c / mag;
	plane->d = plane->d / mag;
}

/// Lab 2 Del 1.2 
/// Den här funktionen hämtar alla sidor till frustrum
void ExtractPlanesGL(Plane * p_planes, Matrix comboMatrix, bool normalize)
{
	/// Left clipping plane
	p_planes[0].a = comboMatrix.e[3] + comboMatrix.e[0];
	p_planes[0].b = comboMatrix.e[7] + comboMatrix.e[4];
	p_planes[0].c = comboMatrix.e[11] + comboMatrix.e[8];
	p_planes[0].d = comboMatrix.e[15] + comboMatrix.e[12];
	/// Right clipping plane 
	p_planes[1].a = comboMatrix.e[3] - comboMatrix.e[0];
	p_planes[1].b = comboMatrix.e[7] - comboMatrix.e[4];
	p_planes[1].c = comboMatrix.e[11] - comboMatrix.e[8];
	p_planes[1].d = comboMatrix.e[15] - comboMatrix.e[12];
	/// Top clipping plane
	p_planes[2].a = comboMatrix.e[3] - comboMatrix.e[1];
	p_planes[2].b = comboMatrix.e[7] - comboMatrix.e[5];
	p_planes[2].c = comboMatrix.e[11] - comboMatrix.e[9];
	p_planes[2].d = comboMatrix.e[15] - comboMatrix.e[13];
	/// Bottom clipping plane
	p_planes[3].a = comboMatrix.e[3] + comboMatrix.e[1];
	p_planes[3].b = comboMatrix.e[7] + comboMatrix.e[5];
	p_planes[3].c = comboMatrix.e[11] + comboMatrix.e[9];
	p_planes[3].d = comboMatrix.e[15] + comboMatrix.e[13];
	/// Near clipping plane
	p_planes[4].a = comboMatrix.e[3] + comboMatrix.e[2];
	p_planes[4].b = comboMatrix.e[7] + comboMatrix.e[6];
	p_planes[4].c = comboMatrix.e[11] + comboMatrix.e[10];
	p_planes[4].d = comboMatrix.e[15] + comboMatrix.e[14];
	/// Far clipping plane
	p_planes[5].a = comboMatrix.e[3] - comboMatrix.e[2];
	p_planes[5].b = comboMatrix.e[7] - comboMatrix.e[6];
	p_planes[5].c = comboMatrix.e[11] - comboMatrix.e[10];
	p_planes[5].d = comboMatrix.e[15] - comboMatrix.e[14];

	/// Normalize the plane equations, if requested
	if (normalize == true)
	{
		NormalizePlane(&p_planes[0]);
		NormalizePlane(&p_planes[1]);
		NormalizePlane(&p_planes[2]);
		NormalizePlane(&p_planes[3]);
		NormalizePlane(&p_planes[4]);
		NormalizePlane(&p_planes[5]);
	}
}

/// Lab 2 Del 1.2 
/// Den här funktionen kollar om objektet och spheren är utanför frustumen
bool cullingFrustum(float radius, HomVector center, Plane *p_planes)
{
	//PrintHomVector("center", center);

	for (int i = 0; i < 6; i++) 
	{
		if ((p_planes[i].a*center.x + p_planes[i].b*center.y + p_planes[i].c*center.z + p_planes[i].d * center.w) <= -radius) 
		{
			return false;
		}
	}
	return true;
}

Material createMaterial(Vector KD, Vector KA, Vector KS)
{
	Material material;
	material.diffuse = KD;
	material.ambient = KA;
	material.specular = KS;
	return material;
}
