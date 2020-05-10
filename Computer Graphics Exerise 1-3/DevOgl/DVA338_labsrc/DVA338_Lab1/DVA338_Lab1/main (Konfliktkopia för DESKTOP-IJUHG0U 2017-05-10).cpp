//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>

#include "algebra.h"
#include "shaders.h"
#include "mesh.h"

int screen_width = 1024;
int screen_height = 768;

Mesh *meshList = NULL; // Global pointer to linked list of triangle meshes

Mesh *boundingSphere = NULL; /// Lab 2 Del 1.1 

Mesh *currentMesh = NULL; /// För lab 1 del 1.4 så man kan rotera objektet och flytta på olika objekt inte kameran 

Camera cam = {{0,0,20}, {0,0,0}, 60, 1, 10000}; // Setup the global camera parameters

GLuint shprg; // Shader program id

bool projection = true; /// Lab 1 Del 1.2 

bool culling = true;

Plane newPlane[6];

bool bound = true;

// Global transform matrices
// V is the view transform
// P is the projection transform
// PV = P * V is the combined view-projection transform
Matrix V, P, PV;

void prepareShaderProgram(const char ** vs_src, const char ** fs_src) 
{
	GLint success = GL_FALSE;

	shprg = glCreateProgram();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in vertex shader!\n");
	else printf("Vertex shader compiled successfully!\n");

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);	
	if (!success) printf("Error in fragment shader!\n");
	else printf("Fragment shader compiled successfully!\n");

	glAttachShader(shprg, vs);
	glAttachShader(shprg, fs);
	glLinkProgram(shprg);
	GLint isLinked = GL_FALSE;
	glGetProgramiv(shprg, GL_LINK_STATUS, &isLinked);
	if (!isLinked) printf("Link error in shader program!\n");
	else printf("Shader program linked successfully!\n");
}

void prepareMesh(Mesh *mesh) 
{
	int sizeVerts = mesh->nv * 3 * sizeof(float);
	int sizeCols  = mesh->nv * 3 * sizeof(float);
	int sizeTris = mesh->nt * 3 * sizeof(int);
	
	// For storage of state and other buffer objects needed for vertex specification
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	// Allocate VBO and load mesh data (vertices and normals)
	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeVerts + sizeCols, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerts, (void *)mesh->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeVerts, sizeCols, (void *)mesh->vnorms);

	// Allocate index buffer and load mesh indices
	glGenBuffers(1, &mesh->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeTris, (void *)mesh->triangles, GL_STATIC_DRAW);

	// Define the format of the vertex data
	GLint vPos = glGetAttribLocation(shprg, "vPos");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// Define the format of the vertex data 
	GLint vNorm = glGetAttribLocation(shprg, "vNorm");
	glEnableVertexAttribArray(vNorm);
	glVertexAttribPointer(vNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)(mesh->nv * 3 *sizeof(float)));

	glBindVertexArray(0);
}

void renderMesh(Mesh *mesh) {
	
	// Assignment 1: Apply the transforms from local mesh coordinates to world coordinates here
	// Combine it with the viewing transform that is passed to the shader below

	/// Lab 1 Del 1.4
	/// Uträkning för currentMesh, formeln fick jag från labb spesen
	Matrix W = TranslationMatrix(-mesh->center.x, -mesh->center.y, -mesh->center.z);
	W = MatMatMul(ScalingMatrix(mesh->S.x, mesh->S.y, mesh->S.z), W);
	W = MatMatMul(RotationMatrix(mesh->R.x, mesh->R.y, mesh->R.z), W);
	W = MatMatMul(TranslationMatrix(-mesh->T.x, -mesh->T.y, -mesh->T.z), W);
	///W = MatMatMul(V, W);
	W = MatMatMul(PV, W);

	/// Printar matriser så när man ändrar värden så ser man dom nya värderna 
	///PrintMatrix("Current mesh", W);

	// Pass the viewing transform to the shader
	GLint loc_PV = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, W.e);

	// Select current resources 
	glBindVertexArray(mesh->vao);
	
	// To accomplish wireframe rendering (can be removed to get filled triangles)
	/// Har lagt till GL_FILL så objektet blir fylld isället för att den ska vara skapad av linjer 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, mesh->nt * 3, GL_UNSIGNED_INT, NULL); 
	glBindVertexArray(0);
}

void renderBoundingSphere(Mesh *mesh) 
{
	/// Lab 2 Del 1.1
	/// Sätt storleken på sfären till radius
	/// Här måste vi ändra för att få sphären att gå med objektet 
	Vector sizeOfSphere = ScalarVecMul(mesh->radius, { mesh->S.x, mesh->S.y, mesh->S.z });
	Matrix W = ScalingMatrix(sizeOfSphere.x, sizeOfSphere.y, sizeOfSphere.z);
	W = MatMatMul(RotationMatrix(mesh->R.x, mesh->R.y, mesh->R.z), W);
	W = MatMatMul(TranslationMatrix(-mesh->T.x, -mesh->T.y, -mesh->T.z), W);
	W = MatMatMul(PV, W);

	/// Printar matriser så när man ändrar värden så ser man dom nya värderna 
	///PrintMatrix("Render Bounding Sphere", W);

	// Pass the viewing transform to the shader
	GLint loc_PV = glGetUniformLocation(shprg, "PV");
	glUniformMatrix4fv(loc_PV, 1, GL_FALSE, W.e);

	// Select current resources 
	glBindVertexArray(boundingSphere->vao);

	// To accomplish wireframe rendering (can be removed to get filled triangles)
	/// Har lagt till GL_FILL så objektet blir fylld isället för att den ska vara skapad av linjer 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Draw all triangles
	glDrawElements(GL_TRIANGLES, boundingSphere->nt * 3, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void display(void) 
{
	Mesh *mesh;
	Mesh *sphere;
	float distance[6];
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	// Assignment 1: Calculate the transform to view coordinates yourself 	
	// The matrix V should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 

	/// Lab 1 Del 1.1
	/// Itsället för att ha fasta värden så har vi skapat funktioner som sätter in olika värden beroede på vad man trycker på kanpparn som finns i switch satsen 
	V = MatMatMul(TranslationMatrix(cam.position.x, cam.position.y, -cam.position.z), RotationMatrix(cam.rotation.x, cam.rotation.y, cam.rotation.z));
	V = MatMatMul(V, ScalingMatrix(1.0, 1.0, 1.0));

	// Assignment 1: Calculate the projection transform yourself 	
	// The matrix P should be calculated from camera parameters
	// Therefore, you need to replace this hard-coded transform. 
	 
	/// Lab 1 Del 1.2
	if (projection)
	{
		/// Normal visning på objektet 3D
		P = PerpectiveMatrix(cam.fov/20, cam.nearPlane, cam.farPlane, screen_width/screen_height);
	}
	else
	{
		/// Den visar i 2D som när man ritar på papper den har bara x och y värden den faller på skärmen bara 
		P = ParallellMatrix(cam.nearPlane, -cam.farPlane, 10, -10, -10 * (screen_width/screen_height), 10 * (screen_width / screen_height));
	}

	// This finds the combined view-projection matrix
	if (!culling)
	{
		PV = MatMatMul(P, V);
	}

	// Select the shader program to be used during rendering 
	glUseProgram(shprg);

	// Render all meshes in the scene
	mesh = meshList;

	sphere = boundingSphere;

	printf("%f %f %f %f\n", newPlane->a, newPlane->b, newPlane->c, newPlane->d);
	

	while (mesh != NULL)
	{

		if (culling)
		{
			Matrix W = TranslationMatrix(-mesh->center.x, -mesh->center.y, -mesh->center.z);
			W = MatMatMul(ScalingMatrix(mesh->S.x, mesh->S.y, mesh->S.z), W);
			W = MatMatMul(RotationMatrix(mesh->R.x, mesh->R.y, mesh->R.z), W);
			W = MatMatMul(TranslationMatrix(-mesh->T.x, -mesh->T.y, -mesh->T.z), W);
			Matrix M = MatMatMul(P, V);
			Matrix N = MatMatMul(PV, M);

			ExtractPlanesGL(newPlane, N, true);

			for (int i = 0; i < 6; i++)
			{
				distance[i] = DistanceToPoint(newPlane[i], mesh->center);
				//printf("%f\n", distance[i]);
			}
			if (cullingFrustum(mesh->radius, distance))
			{
				renderMesh(mesh);
				if (bound)
				{
					renderBoundingSphere(mesh);
				}
			}
			else
			{
				printf("Its outside bro\n");
			}

		}
		else
		{
			renderMesh(mesh);
			if (bound)
			{
				renderBoundingSphere(mesh);
			}
		}
		sphere = sphere->next;
		mesh = mesh->next;
	}

	//while (mesh != NULL) 
	//{
	//	renderMesh(mesh);

	//	/// Lab 2 Del 1.1
	//	renderBoundingSphere(mesh);

	//	mesh = mesh->next;
	//}

	glFlush();
}

void changeSize(int w, int h) 
{
	screen_width = w;
	screen_height = h;
	glViewport(0, 0, screen_width, screen_height);
}

void keypress(unsigned char key, int x, int y) 
{
	switch(key) 
	{
	/// Lab 1 Del 1.1
	/// Utzomming och inzomming för kameran
	case 'z':
		cam.position.z -= 2.0f;
		break;
	case 'Z':
		cam.position.z += 2.0f;
		break;

	/// Lab 1 Del 1.4
	/// Utzomming och inzomming för objektet 
	case 'x':
		currentMesh->T.z += 2.0f;
		break;
	case 'X':
		currentMesh->T.z -= 2.0f;
		break;

	/// Lab 1 Del 1.1
	/// Flytta kameran upp, ner, höger och vänster
	case 'w':
		cam.position.y += 0.2f;
		break;
	case 's':
		cam.position.y -= 0.2f;
		break;
	case 'a':
		cam.position.x -= 0.2f;
		break;
	case 'd':
		cam.position.x += 0.2f;
		break;

	/// Lab 1 Del 1.4
	/// Flytta på objektet upp, ner, höger och vänster
	case 'W':
		currentMesh->T.y += 0.2f;
		break;
	case 'S':
		currentMesh->T.y -= 0.2f;
		break;
	case 'A':
		currentMesh->T.x -= 0.2f;
		break;
	case 'D':
		currentMesh->T.x += 0.2f;
		break;

	/// Lab 1 Del 1.1
	/// Så man kan rotera på kameran runt objektet 
	case 'i':
		cam.rotation.x += 0.2f;
		break;
	case 'k':
		cam.rotation.x -= 0.2f;
		break;
	case 'j':
		cam.rotation.y -= 0.2f;
		break;
	case 'l':
		cam.rotation.y += 0.2f;
		break;

	/// Lab 1 Del 1.4
	/// Så man kan rotera på objektet istället för kameran 
	case 'I':
		currentMesh->R.x += 0.2f;
		break;
	case 'K':
		currentMesh->R.x -= 0.2f;
		break;
	case 'J':
		currentMesh->R.y -= 0.2f;
		break;
	case 'L':
		currentMesh->R.y += 0.2f;
		break;

	/// Lab 1 Del 1.4
	/// Så man kan byta från objekt till objekt 
	case 'C':
		if (currentMesh->next != NULL)
		{
			currentMesh = currentMesh->next;
		}
		else
		{
			currentMesh = meshList;
		}
		break;

	/// Lab 1 Del 1.2
	/// Här så ska man byta på hur man vill se objektet 
	case 'p':
		if (!projection)
		{
			projection = true;
		}
		else
		{
			projection = false;
		}
		break;

	/// Lab 2 Del 1.2
	/// Här så ska man ta bort och lägga till spheren 
	case 'B':
		if (culling) culling = false;
		else culling = true;
		break;

	/// Lab 1 Del 1.1
	/// Avsluta programmet 
	case 'Q':
	case 'q':		
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void init(void) 
{
	// Compile and link the given shader program (vertex shader and fragment shader)
	prepareShaderProgram(vs_n2c_src, fs_ci_src); 

	// Setup OpenGL buffers for rendering of the meshes
	Mesh * mesh = meshList;

	/// Lab 1 Del 1.4
	/// Här så sätter jag currentMesh till meshList 
	currentMesh = meshList;

	while (mesh != NULL) 
	{
		prepareMesh(mesh);
		mesh = mesh->next;
	}

	/// Lab 2 Del 1.1
	prepareMesh(boundingSphere);
}

void cleanUp(void) 
{	
	printf("Running cleanUp function... ");
	// Free openGL resources
	// ...

	// Free meshes
	// ...
	printf("Done!\n\n");
}

// Include data for some triangle meshes (hard coded in struct variables)
#include "./models/mesh_bunny.h"
#include "./models/mesh_cow.h"
#include "./models/mesh_cube.h"
#include "./models/mesh_frog.h"
#include "./models/mesh_knot.h"
#include "./models/mesh_sphere.h"
#include "./models/mesh_teapot.h"
#include "./models/mesh_triceratops.h"


int main(int argc, char **argv) 
{
	// Setup freeGLUT
	/// Lagt till GLUT_DEPTH så lab 1 del 1.3 ska kunna fungera 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("DVA338 Programming Assignments");
	glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	/// Lab 1 Del 1.3
	/// Så objektet blir bra med renderingen allt bakom som vi inte ser blir inte uträknad
	/// Enable z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Specify your preferred OpenGL version and profile
	glutInitContextVersion(4, 5);
	//glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);	
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// Uses GLEW as OpenGL Loading Library to get access to modern core features as well as extensions
	GLenum err = glewInit(); 
	if (GLEW_OK != err) { fprintf(stdout, "Error: %s\n", glewGetErrorString(err)); return 1; }

	// Output OpenGL version info
	fprintf(stdout, "GLEW version: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, "OpenGL version: %s\n", (const char *)glGetString(GL_VERSION));
	fprintf(stdout, "OpenGL vendor: %s\n\n", glGetString(GL_VENDOR));

	for (int i = 0; i < 6; i++)
	{
		newPlane[i] = { 0.0f, 0.0f, 0.0f, 0.0f};
	}


	// Insert the 3D models you want in your scene here in a linked list of meshes
	// Note that "meshList" is a pointer to the first mesh and new meshes are added to the front of the list

	/// Här kan man implementera olika objekt som man ska kunna använda

	//insertModel(&meshList, cow.nov, cow.verts, cow.nof, cow.faces, 20.0);
	//insertModel(&meshList, triceratops.nov, triceratops.verts, triceratops.nof, triceratops.faces, 3.0);
	insertModel(&meshList, bunny.nov, bunny.verts, bunny.nof, bunny.faces, 60.0);	
	insertModel(&meshList, cube.nov, cube.verts, cube.nof, cube.faces, 5.0);
	//insertModel(&meshList, frog.nov, frog.verts, frog.nof, frog.faces, 2.5);
	insertModel(&meshList, knot.nov, knot.verts, knot.nof, knot.faces, 1.0);
	//insertModel(&meshList, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 12.0);
	insertModel(&meshList, teapot.nov, teapot.verts, teapot.nof, teapot.faces, 3.0);

	/// Lab 2 Del 1.1
	insertModel(&boundingSphere, sphere.nov, sphere.verts, sphere.nof, sphere.faces, 1.0);
	
	init();
	glutMainLoop();

	cleanUp();	
	return 0;
}
