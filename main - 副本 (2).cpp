// glutEx1.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include "objstream.h"
#include "basic.h"
#include "light.h"
#include "glut.h"
using namespace std;
float fTranslate;
float fRotate;
float fScale = 1.0f;	// set inital scale value to 1.0f
Obj obj1, obj2;
glObject obj;
Light light;
bool bPersp = false;
bool bAnim = false;
bool bWire = false;
bool bDouble = false;
bool bPic = true;
Basic basic=Basic::getBasic();// a global basic manager!
int wHeight = 0;
int wWidth = 0;
GLfloat light_pos[] = { 5, 5, 5, 1 };
float ObjPos[] = { -2.0f, -2.0f, -5.0f };					// Object Position
float LightPos[] = { 5.0f, 5.0f, 5.0f, 1.0f };
GLUquadricObj	*q;										// Quadratic For Drawing A Sphere
float SpherePos[] = { -4.0f, -5.0f, -6.0f };
float LightAmb[] = { 0.2f, 0.2f, 0.2f, 1.0f };			// Ambient Light Values
float LightDif[] = { 0.6f, 0.6f, 0.6f, 1.0f };			// Diffuse Light Values
float LightSpc[] = { -0.2f, -0.2f, -0.2f, 1.0f };			// Specular Light Values
float white[] = { 1, 1, 1, 1 };
float MatAmb[] = { 0.4f, 0.4f, 0.4f, 1.0f };				// Material - Ambient Values
float MatDif[] = { 0.2f, 0.6f, 0.9f, 1.0f };				// Material - Diffuse Values
float MatSpc[] = { 0.0f, 0.0f, 0.0f, 1.0f };				// Material - Specular Values
float MatShn[] = { 0.0f };

void init()
{
	basic.addCylinder2(0.25, 0.5, 1);
	basic.addCube(0.5);
	basic.addSphere(0.25);
	basic.addCylinder(0.25, 1);
	basic.addCone(0.25, 0.75);
	basic.addCone2(0.25, 1, 5);
	basic.addPrism2(0.5, 0.25, 0.25, 6);
	basic.addPrism(0.25, 1, 3);
	basic.addPlane(10, 4);
	basic.addPlane(10, 4);
	basic.addPlane(10, 4);
	basic.addPlane(10, 4);
	basic.addPlane(10, 4);
	//basic.addCircle(10);
	basic.objs[12].r[2] = 90; basic.objs[12].t[0] -= 5;
	basic.objs[11].r[2] = 90; basic.objs[11].t[0] += 5;
	basic.objs[10].t[1] += 5;
	basic.objs[9].r[0] = -90; basic.objs[9].t[2] -= 5;
	basic.objs[8].t[1] -= 5;
	basic.objs[7].t[0] -= 1; basic.objs[7].t[1] -= 1; basic.objs[7].t[2] -= 1;
	basic.objs[6].t[0] += 1; basic.objs[6].t[2] += 1;
	basic.objs[5].t[0] -= 2;
	basic.objs[4].t[0] += 2;
	basic.objs[3].t[1] += 2;
	basic.objs[2].t[2] += 1; basic.objs[2].s[1] = 1;
	basic.objs[0].t[0] += 1;
	basic.objs[1].t[0] -= 1;
	light.addLight();
	/*obj2.read("Creature\\Arakkoa\\arakkoa_sage");
	for (int i = 0; i < obj2.objs.size(); ++i){
	obj2.objs[i].t[1] -= 3;
	obj2.objs[i].s[0] = 0.5;
	obj2.objs[i].s[1] = 0.5;
	obj2.objs[i].s[2] = 0.5;
	}
	obj1.read("Tails");
	for (int i = 0; i < obj1.objs.size(); ++i){
	obj1.objs[i].s[0] = 0.25;
	obj1.objs[i].s[1] = 0.25;
	obj1.objs[i].s[2] = 0.25;
	}*/
	/*basic.loadTex(1, "Monet.bmp");
	basic.openTex(1);
	basic.loadTex(2, "Monet.bmp");
	basic.openTex(2);
	basic.loadTex(0, "Monet.bmp");
	basic.openTex(0);
	basic.loadTex(3, "Monet.bmp");
	basic.openTex(3);
	basic.loadTex(4, "Monet.bmp");
	basic.openTex(4);
	basic.loadTex(5, "Monet.bmp");
	basic.openTex(5);
	basic.loadTex(6, "Monet.bmp");
	basic.openTex(6);
	basic.loadTex(7, "Monet.bmp");
	basic.openTex(7);*/
	for (int i = 0; i < basic.objs.size() - 5; ++i){
		basic.loadTex(i, "Monet.bmp");
		basic.openTex(i);
	}
	ReadObject("Object2.txt", &obj);
	setConnectivity(obj);
	for (unsigned int i = 0; i<obj.planeSize; i++)			// Loop Through All Object Planes
		calPlane(obj, obj.planes[i]);
	obj.t[0] += 2; obj.t[1] -= 2;
	obj.s[0] = 0.3; obj.s[1] = 0.3; obj.s[2] = 0.3;
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2, 0.2, 1.0, 1.0);
	glClearDepth(1.0f);
	glClearStencil(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	/*glLightfv(GL_LIGHT1, GL_POSITION, LightPos);		// Set Light1 Position
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glEnable(GL_LIGHT1);*/								// Enable Light1
	glEnable(GL_LIGHTING);								// Enable Lightins
	glEnable(GL_NORMALIZE);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	q = gluNewQuadric();								// Initialize Quadratic
	gluQuadricNormals(q, GL_SMOOTH);					// Enable Smooth Normal Generation
	gluQuadricTexture(q, GL_FALSE);
	//light.addLight();
	//obj2.read("Tails");
}


void updateView(int width, int height)
{
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)width / (GLfloat)height;
	if (bPersp) {
		gluPerspective(45.0f, whRatio, 0.1f, 100.0f);
		//glFrustum(-3, 3, -3, 3, 3,100);
	}
	else {
		glOrtho(-10, 10, -10, 10, -100, 100);
	}

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 200.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
}

void idle()
{

}

float eye[] = { 0, 0, 8 };
float center[] = { 0, 0, 0 };
int state = 0;
void key(unsigned char k, int x, int y)
{

	switch (k)
	{
	case 27:
	case 'q': {exit(0); break; }
	case 't':{state = (state + 1) % light.pLight.size(); break; }
	case 'p': {bPersp = !bPersp; break; }
	case ' ':{fRotate += 2; break; }
	case 'x':{fRotate -= 2; break; }
	case 'o':{bWire = !bWire; break; }
	case 'a': {eye[0] -= 0.2f; center[0] -= 0.2f; break; }
	case 'd': {eye[0] += 0.2f; center[0] += 0.2f; break; }
	case 'w': {eye[1] -= 0.2f; center[1] -= 0.2f; break; }
	case 's': {eye[1] += 0.2f; center[1] += 0.2f; break; }
	case 'z': {eye[2] -= 0.2f; center[2] -= 0.2f; break; }
	case 'c': {eye[2] += 0.2f; center[2] += 0.2f; break; }


	case 'j':{light.changeX(state, -1); break; }
	case 'l':{light.changeX(state, 1); break; }
	case 'i':{light.changeY(state, 1); break; }
	case 'k':{light.changeY(state, -1); break; }
	case 'm':{light.changeZ(state, 1); break; }
	case '.':{light.changeZ(state, -1); break; }
		/*case 'j':{LightPos[0] -= 0.5f; break; }
		case 'l':{LightPos[0] += 0.5f; break; }
		case 'i':{LightPos[1] += 0.5f; break; }
		case 'k':{LightPos[1] -= 0.5f; break; }
		case 'm':{LightPos[2] += 0.5f; break; }
		case '.':{LightPos[2] -= 0.5f; break; }*/

	case 'u':{light.addLight(); break; }
	case 'r':{light.rmLight(state); if (light.pLight.size())state %= light.pLight.size(); break; }
	case 'f':{light.closeLight(state); break; }
	case 'v':{light.openLight(state); break; }

	case 'y':{light.addIntensity(state, 0); break; }
	case 'h':{light.subIntensity(state, 0); break; }
	}
	glutPostRedisplay();
	//updateView(wHeight, wWidth);
}

void DrawGLRoom()										// Draw The Room (Box)
{
	glBegin(GL_QUADS);									// Begin Drawing Quads
	// Floor
	glNormal3f(0.0f, 1.0f, 0.0f);					// Normal Pointing Up
	glVertex3f(-10.0f, -10.0f, -20.0f);				// Back Left
	glVertex3f(-10.0f, -10.0f, 20.0f);				// Front Left
	glVertex3f(10.0f, -10.0f, 20.0f);				// Front Right
	glVertex3f(10.0f, -10.0f, -20.0f);				// Back Right
	// Ceiling
	glNormal3f(0.0f, -1.0f, 0.0f);					// Normal Point Down
	glVertex3f(-10.0f, 10.0f, 20.0f);				// Front Left
	glVertex3f(-10.0f, 10.0f, -20.0f);				// Back Left
	glVertex3f(10.0f, 10.0f, -20.0f);				// Back Right
	glVertex3f(10.0f, 10.0f, 20.0f);				// Front Right
	// Front Wall
	glNormal3f(0.0f, 0.0f, 1.0f);					// Normal Pointing Away From Viewer
	glVertex3f(-10.0f, 10.0f, -20.0f);				// Top Left
	glVertex3f(-10.0f, -10.0f, -20.0f);				// Bottom Left
	glVertex3f(10.0f, -10.0f, -20.0f);				// Bottom Right
	glVertex3f(10.0f, 10.0f, -20.0f);				// Top Right
	// Back Wall
	glNormal3f(0.0f, 0.0f, -1.0f);					// Normal Pointing Towards Viewer
	glVertex3f(10.0f, 10.0f, 20.0f);				// Top Right
	glVertex3f(10.0f, -10.0f, 20.0f);				// Bottom Right
	glVertex3f(-10.0f, -10.0f, 20.0f);				// Bottom Left
	glVertex3f(-10.0f, 10.0f, 20.0f);				// Top Left
	// Left Wall
	glNormal3f(1.0f, 0.0f, 0.0f);					// Normal Pointing Right
	glVertex3f(-10.0f, 10.0f, 20.0f);				// Top Front
	glVertex3f(-10.0f, -10.0f, 20.0f);				// Bottom Front
	glVertex3f(-10.0f, -10.0f, -20.0f);				// Bottom Back
	glVertex3f(-10.0f, 10.0f, -20.0f);				// Top Back
	// Right Wall
	glNormal3f(-1.0f, 0.0f, 0.0f);					// Normal Pointing Left
	glVertex3f(10.0f, 10.0f, -20.0f);				// Top Back
	glVertex3f(10.0f, -10.0f, -20.0f);				// Bottom Back
	glVertex3f(10.0f, -10.0f, 20.0f);				// Bottom Front
	glVertex3f(10.0f, 10.0f, 20.0f);				// Top Front
	glEnd();											// Done Drawing Quads
}

void redraw()
{
	GLmatrix16f Minv;
	GLvector4f wlp, lp;
	lp[0] = LightPos[0];
	lp[1] = LightPos[1];
	lp[2] = LightPos[2];
	lp[3] = LightPos[3];
	// Clear Color Buffer, Depth Buffer, Stencil Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.2, 0.2, 1.0, 1.0);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);
	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glShadeModel(GL_SMOOTH);
	glRotatef(fRotate, 0, 1, 0);
	/*for (int i = 0; i < obj2.objs.size(); ++i){
	glObject &o = obj2.objs[i];
	glPushMatrix();
	glTranslatef(o.t[0], o.t[1], o.t[2]);
	glScalef(o.s[0], o.s[1], o.s[2]);
	glRotatef(o.r[2], 0, 0, 1);
	glRotatef(o.r[1], 0, 1, 0);
	glRotatef(o.r[0], 1, 0, 0);
	drawGLObject(obj2.objs[i]);
	glPopMatrix();
	}
	for (int i = 0; i < obj1.objs.size(); ++i){
	glObject &o = obj1.objs[i];
	glPushMatrix();
	glTranslatef(o.t[0], o.t[1], o.t[2]);
	glScalef(o.s[0], o.s[1], o.s[2]);
	glRotatef(o.r[2], 0, 0, 1);
	glRotatef(o.r[1], 0, 1, 0);
	glRotatef(o.r[0], 1, 0, 0);
	drawGLObject(obj1.objs[i]);
	glPopMatrix();
	}*/
	//glGetFloatv(GL_MODELVIEW, Minv);
	//VMatMult(Minv, lp);
	glLightfv(GL_LIGHT1, GL_POSITION, lp);
	light.enableLights();
	glObject &o = obj;
	glPushMatrix();
	glTranslatef(o.t[0], o.t[1], o.t[2]);
	glScalef(o.s[0], o.s[1], o.s[2]);
	glRotatef(o.r[2], 0, 0, 1);
	glRotatef(o.r[1], 0, 1, 0);
	glRotatef(o.r[0], 1, 0, 0);
	drawGLObject(obj);
	glPopMatrix();
	basic.setDefaultMaterial();
	/*glPushMatrix();
	glTranslatef(0, 0, -5);
	glRotatef(-90, 1, 0, 0);
	glRotatef(45, 0, 1, 0);
	basic.drawPlane(10, -1);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, -5, 0);
	glRotatef(180, 1, 0, 0);
	glRotatef(45, 0, 1, 0);
	basic.drawPlane(10, -1);
	glPopMatrix();*/

	basic.drawAll();


	//for (int i = 0; i < 2; ++i){
	//	drawGLObject(obj1.objs[i]);
	//}
	//basic.objs[0].r[0] = 180;

	for (int i = 0; i < basic.objs.size(); ++i)
		addShadowObject(basic.objs[i], light);
	for (int i = 0; i < obj1.objs.size(); ++i){
		addShadowObject(obj1.objs[i], light);
	}
	addShadowObject(obj, light);
	drawShadow();

	glPushMatrix();
	glTranslatef(0, -3, 0);
	glutSolidCube(1.0);
	glPopMatrix();
	//drawGLObject(basic.objs[0]);
	// Reset Modelview Matrix
	/*glTranslatef(0.0f, 0.0f, -20.0f);					// Zoom Into Screen 20 Units
	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);		// Position Light1
	glTranslatef(SpherePos[0], SpherePos[1], SpherePos[2]);	// Position The Sphere
	gluSphere(q, 1.5f, 32, 16);							// Draw A Sphere

	glRotatef(fRotate, 0, 1, 0);
	obj.t[1] = ObjPos[1];
	obj.t[2] = ObjPos[2] - 20;


	glColor4f(0.7f, 0.4f, 0.0f, 1.0f);					// Set Color To An Orange
	// Reset Modelview Matrix
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -20.0f);					// Zoom Into The Screen 20 Units
	DrawGLRoom();
	// Draw The Room
	//glTranslatef(ObjPos[0], ObjPos[1], ObjPos[2]);		// Position The Object
	//glRotatef(xrot, 1.0f, 0.0f, 0.0f);					// Spin It On The X Axis By xrot
	//glRotatef(yrot, 0.0f, 1.0f, 0.0f);					// Spin It On The Y Axis By yrot
	//drawGLObject(obj);									// Procedure For Drawing The Loaded Object
	//castShadow(obj, lp);								// Procedure For Casting The Shadow Based On The Silhouette
	*///addShadowObject(obj, LightPos);

	/*


	for (int i = 0; i < 2; ++i){
	obj1.objs[i].s[0] = 0.2;
	obj1.objs[i].s[1] = 0.2;
	obj1.objs[i].s[2] = 0.2;
	addShadow(obj1.objs[i], LightPos);
	}
	glPopMatrix();
	glLoadIdentity();
	glColor4f(0.7f, 0.4f, 0.0f, 1.0f);					// Set Color To Purplish Blue
	glDisable(GL_LIGHTING);								// Disable Lighting
	glDepthMask(GL_FALSE);								// Disable Depth Mask
	glTranslatef(LightPos[0], LightPos[1], LightPos[2] - 20);					// Translate To Light's Position
	// Notice We're Still In Local Coordinate System
	gluSphere(q, 0.2f, 16, 8);							// Draw A Little Yellow Sphere (Represents Light)
	glEnable(GL_LIGHTING);								// Enable Lighting
	glDepthMask(GL_TRUE);								// Enable Depth Mask
	*/
	//xrot += xspeed;										// Increase xrot By xspeed
	//yrot += yspeed;										// Increase yrot By yspeed
	light.drawLights();
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(480, 480);
	int windowHandle = glutCreateWindow("Simple GLUT App");

	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);
	init();
	glutMainLoop();
	return 0;
}


