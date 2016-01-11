#define _CRT_SECURE_NO_WARNINGS
#include "interact.h"
#include "GlObject.h"
#include "GlTexManager.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <GL/glut.h>

using namespace std;
#define UNSELECTED -1;

static int selectedId = UNSELECTED;
int windowWidth=600;
int windowHeight=800;
float whRatio;

float center[] = { 0, -0.8, -6 };
float eye[] = { 0, 1.2, 2 };

bool bPersp = true;
bool bAnim = false;
bool bWire = false;
float fTranslate;
float fRotate = 156.5f;
float fScale = 1.0f;	// set inital scale value to 1.0f

bool valueMode = false;
int currentMaterialMode = 0;
float currentValue = 0;

int tranformMode = -1;
float* transformArray;
enum MENUID{
	LIGHT,
	ITEM,
	GEOMETRY,
	MATERIAL,
	TEXTURE,
	SCALE,
	TRANSLATE,
	ROTATE,
	LENGTAI,
	DISFFUSE_R=GL_DIFFUSE*4,
	DISFFUSE_G=GL_DIFFUSE*4+1,
	DISFFUSE_B=GL_DIFFUSE*4+2,
	DISFFUSE_A=GL_DIFFUSE*4+3,
	AMBIENT_R=GL_AMBIENT*4,
	AMBIENT_G=GL_AMBIENT*4+1,
	AMBIENT_B=GL_AMBIENT*4+2,
	AMBIENT_A=GL_AMBIENT*4+3,
	SPECULAR_R=GL_SPECULAR*4,
	SPECULAR_B=GL_SPECULAR*4+1,
	SPECULAR_G=GL_SPECULAR*4+2,
	SPECULAR_A=GL_SPECULAR*4+3,
	SHINESS=GL_SHININESS*4,
	TEXTURE0 = 222
};
enum MENUSTATE{
	MAIN,
	OBJ,
	RESET,
	
};
vector<MENUID> menuVec;
static MENUSTATE menuState=RESET;

static int menuid;
static int color;
static int texture;
static int texture_type;
static int material;
static int disffuse;
static int ambient;
static int speculars;
static int shiness;
static int val;
static int item;
static int transform;
static int obj;

char textBuffer[100]="";


void createMainMenu(void){
	if (menuState == MAIN)return;
	menuVec.clear();
	obj= glutCreateMenu(menu);

	item = glutCreateMenu(menu);
	glutAddMenuEntry("导入棱台", LENGTAI);
	glutAddSubMenu("导入obj", obj);
	color = glutCreateMenu(menu);

	menuVec.push_back(LIGHT);
	menuVec.push_back(ITEM);
	//add sub menu entry
	glutAddMenuEntry("光照",LIGHT);

	glutAddSubMenu("物品", item);
	
	//Let the menu respond on the right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



void menu(int value)
{
	if (value == LIGHT){
		cout << "color" << endl;
	}
	else if (value == TRANSLATE){
		GlObject& obj = GlObjectManager::getGlObj(selectedId);
		transformArray = obj.transArray;
		tranformMode = TRANSLATE;
	}
	else if (value == SCALE){
		GlObject& obj = GlObjectManager::getGlObj(selectedId);
		transformArray = obj.scaleArray;
		tranformMode = SCALE;
	}
	else if (value == ROTATE){
		GlObject& obj = GlObjectManager::getGlObj(selectedId);
		transformArray = obj.rotateArray;
		tranformMode = ROTATE;
	}
	else if (value == LENGTAI){
		/*
		GlObject globj("name", f);
		GlObjectManager::addGlObj(globj);
		selectedId=globj.getId();
		*/
	}
	else if (value > TEXTURE0&&value<TEXTURE0+MAX_TEX){
		int textureNum = value - TEXTURE0;
		GlObject& obj = GlObjectManager::getGlObj(selectedId);
		obj.setTexture(textureNum);
	}
	else if (value > 10000){
		valueMode = true;
		currentMaterialMode = value;
	}
}

void destroyMenu(){
	for (auto item : menuVec){
		glutRemoveMenuItem(item);
	}
}

void motion(int x, int y)
{
	if (valueMode){
		unsigned char pixel[4];
		if (y < 20){
			float value = x * 1.0/ windowWidth;
			sprintf(textBuffer, "value:%f", value);
			currentValue = value;
		}
	}
}

void createItemMenu(){
	if (menuState == OBJ)return;
	menuVec.clear();

	menuVec.push_back(GEOMETRY);
	menuVec.push_back(MATERIAL);
	menuVec.push_back(TEXTURE);
	//add sub menu entry
	
	texture = glutCreateMenu(menu);
	for (auto& content : GlTexManager::textureMap){
		glutAddMenuEntry(content.first.c_str(), TEXTURE0 + content.second);
	}
	disffuse = glutCreateMenu(menu);
	glutAddMenuEntry("R", DISFFUSE_R);
	glutAddMenuEntry("G", DISFFUSE_G);
	glutAddMenuEntry("B", DISFFUSE_B);
	glutAddMenuEntry("A", DISFFUSE_A);

	ambient = glutCreateMenu(menu);
	glutAddMenuEntry("R", AMBIENT_R);
	glutAddMenuEntry("G", AMBIENT_G);
	glutAddMenuEntry("B", AMBIENT_B);
	glutAddMenuEntry("A", AMBIENT_A);
	//glutAddMenuEntry("")
	speculars = glutCreateMenu(menu);
	glutAddMenuEntry("R", SPECULAR_R);
	glutAddMenuEntry("G", SPECULAR_G);
	glutAddMenuEntry("B", SPECULAR_B);
	glutAddMenuEntry("A", SPECULAR_A);

	material = glutCreateMenu(menu);
	glutAddSubMenu("环境光", ambient);
	glutAddSubMenu("散射光", disffuse);
	glutAddSubMenu("镜面反射光",speculars );
	glutAddMenuEntry("反射指数",SHINESS );

	transform = glutCreateMenu(menu);
	glutAddMenuEntry("平移", TRANSLATE);
	glutAddMenuEntry("旋转", ROTATE);
	glutAddMenuEntry("伸缩",SCALE);
	
	menuid = glutCreateMenu(menu);
	glutAddSubMenu("纹理", texture);
		

	glutAddSubMenu("几何变换", transform);


	glutAddSubMenu("材料", material);
	//Let the menu respond on the right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void mouseClick(int button, int state, int x, int y)
{
	GLuint selectBuf[512];
	GLuint hits;
	GLint viewport[4];

	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;
	if (tranformMode != -1)return;
	if (valueMode){
		GlObject& obj = GlObjectManager::getGlObj(selectedId);
		int materialpara = currentMaterialMode / 4;
		int materialNum = currentMaterialMode % 4; 
		float* arr = obj.getMaterialfv(materialpara);
		arr[materialNum] = currentValue;
		textBuffer[0] = 0;
		return;
	}
	//get viewport and select buffer
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, selectBuf);
	glRenderMode(GL_SELECT);

	//initialize names
	glInitNames();
	glPushName(0);

	//save projection matrix
	//glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	
	//set pick matrix
	//gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 1.0, 1.0, viewport);
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 2.0, 2.0, viewport);

	//set projection matrix for select mode
	if (bPersp) {
		gluPerspective(45.0f, (GLfloat)(viewport[2] - viewport[0]) / (GLfloat)(viewport[3] - viewport[1]), 0.001f, 1000.0f);
		//glFrustum(-3, 3, -3, 3, 3,100);
	}
	else {
		glOrtho(-3, 3, -3, 3, -100, 100);
	}
	//draw in select mode and name objects
	glMatrixMode(GL_MODELVIEW);
	glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
	GlObjectManager::drawAll();

	//back to project
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	//glFlush();

	//process hits
	hits = glRenderMode(GL_RENDER);
	printf("Hits : %d\n", hits);
	/*Print a list of the objects*/
	list_hits(hits, selectBuf);

	glMatrixMode(GL_MODELVIEW);
}



void key(unsigned char k, int x, int y)
{
	switch (k)
	{
	case 27:
	case 'q': {exit(0); break; }
	case 'p': {bPersp = !bPersp; break; }

	case ' ': {bAnim = !bAnim; break; }
	case 'o': {bWire = !bWire; break; }
	//case '0': {drawMode++; drawMode %= 3; break; }

	case 'a': {
		eye[0] -= 0.2f;
		center[0] -= 0.2f;
		break;
	}
	case 'd': {
		eye[0] += 0.2f;
		center[0] += 0.2f;
		break;
	}
	case 'w': {
		eye[1] -= 0.2f;
		center[1] -= 0.2f;
		break;
	}
	case 's': {
		eye[1] += 0.2f;
		center[1] += 0.2f;
		break;
	}
	case 'z': {
		eye[2] -= 0.2f;
		center[2] -= 0.2f;
		break;
	}
	case 'c': {
		eye[2] += 0.2f;
		center[2] += 0.2f;
		break;
	}
	case 'Q':tranformMode = -1;
		break;
	case 'W':updateTransformArray(tranformMode, 1, 1, transformArray);
		break;
	case 'S':updateTransformArray(tranformMode, 0, 1, transformArray);
		break;
	case 'A':updateTransformArray(tranformMode, 0, 0, transformArray);
		break;
	case 'D':updateTransformArray(tranformMode, 1, 0, transformArray);
		break;
	case 'Z':updateTransformArray(tranformMode, 0, 2, transformArray);
		break;
	case 'C':updateTransformArray(tranformMode, 1, 2, transformArray);
		break;
	}

	updateView(windowWidth, windowHeight);
}

void updateTransformArray(int mode, bool pos,int count, float* p){
	if (mode == -1)return;
	else{
		if (mode == TRANSLATE){
			if (pos)transformArray[count] += 0.1;
			else transformArray[count] -= 0.1;
		}
		else if (mode == SCALE){
			if (pos)transformArray[count] *= 0.95;
			else transformArray[count] /= 0.95;
		}
		else if (mode == ROTATE){
			if (pos)transformArray[count] += 5;
			else transformArray[count] -= 5;
		}
	}
}

void list_hits(int hits, unsigned int *names)
{

	/*
	For each hit in the buffer are allocated 4 bytes:
	1. Number of hits selected (always one,
	beacuse when we draw each object
	we use glLoadName, so we replace the
	prevous name in the stack)
	2. Min Z
	3. Max Z
	4. Name of the hit (glLoadName)
	*/

	printf("%d hits:\n", hits);

	for (int i = 0; i < hits; i++)
		printf("Number: %d\n"
		"Min Z: %d\n"
		"Max Z: %d\n"
		"Name on stack: %d\n",
		(GLubyte)names[i * 4],
		(GLubyte)names[i * 4 + 1],
		(GLubyte)names[i * 4 + 2],
		(GLubyte)names[i * 4 + 3]
		);

	printf("\n");

	int i;
	if (hits == 0){
		puts("catch nothing!");
		selectedId = UNSELECTED;
		createMainMenu();
		return;
	}
	/*
	For each hit in the buffer are allocated 4 bytes:
	1. Number of hits selected (always one,
	beacuse when we draw each object
	we use glLoadName, so we replace the
	prevous name in the stack)
	2. Min Z
	3. Max Z
	4. Name of the hit (glLoadName)
	*/
	unsigned int minZ = -1;
	int minId = 0;
	for (i = 0; i < hits; i++){
		unsigned int Z, id;
		id = (GLubyte)names[i * 4 + 3];
		Z = names[i * 4 + 1];
		cout <<i<<":"<< id<<" "<<Z<<" "<<names[i*4+1]<<endl;
		if (Z < minZ){
			minZ = Z;
			minId = id;
		}
	}
	selectedId = minId;
	createItemMenu();
	cout << selectedId << endl<<endl;
}

void idle()
{
	glutPostRedisplay();
}

GLfloat white[] = { 0.5, 0.5, 0.5, 1.0f };
GLfloat nowhite[] = { 0.4f, 0.2f, 0.3f, 1.0f };
GLfloat lowLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat* pLight = white;
GLfloat light_pos[] = { 5.0f, 5.0f, 5.0f, 1 };


GLfloat spot_direction[] = { 0.0, 0.0, -1.0, 1.0 };
GLfloat light1Pos[] = { 0.0f, 1.0f, 10.0f, 1.0f };
GLfloat diffuseLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat angle = 0.1f;

void draw_block(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glutSolidCube(1.0);
	glPopMatrix();
}

void redraw()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);				// ≥°æ∞£®0£¨0£¨0£©µƒ ”µ„÷––ƒ (0,5,50)£¨Y÷·œÚ…œ

	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_pos[] = { 5, 5, 5, 1 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);
	glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
	GlObjectManager::drawAll();	
	

	if (bAnim) fRotate += 0.5f;
	drawText(textBuffer, 100, 100);
	//getFPS();
	if (valueMode)
		drawRectangle(0, windowHeight-20, windowWidth, 20);
	glutSwapBuffers();
	
}


void updateView(int width, int height)
{
							// Reset The Current Viewport
	glViewport(0, 0, windowWidth , windowHeight);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	whRatio = (GLfloat)width / (GLfloat)height;
	if (bPersp) {
		gluPerspective(45.0f, whRatio, 0.001f, 1000.0f);
		//glFrustum(-3, 3, -3, 3, 3,100);
	}
	else {
		glOrtho(-3, 3, -3, 3, -100, 100);
	}

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	windowHeight = height;
	windowWidth = width;

	updateView(height, width);
}

void startApp(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(windowHeight, windowWidth);
	glutInitWindowPosition(0, 0);
	int windowHandle = glutCreateWindow("Simple GLUT App");
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouseClick);
	glutKeyboardFunc(key);
	glutPassiveMotionFunc(motion);
	createMainMenu();
	GlTexManager::init();
	glutMainLoop();
}

void drawText(const char* str, int x, int y)
{
	static int frame = 0, time, timebase = 0;


	char mode[64];

	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);  // 选择投影矩阵
	glPushMatrix();               // 保存原矩阵
	glLoadIdentity();             // 装入单位矩阵
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);    // 位置正投影
	glMatrixMode(GL_MODELVIEW);   // 选择Modelview矩阵
	glPushMatrix();               // 保存原矩阵
	glLoadIdentity();             // 装入单位矩阵
	glRasterPos2f(x, windowHeight-y);
	for (; *str != '\0'; str++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *str);
	}
	glMatrixMode(GL_PROJECTION);  // 选择投影矩阵
	glPopMatrix();                // 重置为原保存矩阵
	glMatrixMode(GL_MODELVIEW);   // 选择Modelview矩阵
	glPopMatrix();                // 重置为原保存矩阵
	glEnable(GL_DEPTH_TEST);
}

void drawRectangle(int x, int y, int width, int length){
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);  // 选择投影矩阵
	glPushMatrix();               // 保存原矩阵
	glLoadIdentity();             // 装入单位矩阵
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);    // 位置正投影
	glMatrixMode(GL_MODELVIEW);   // 选择Modelview矩阵
	glPushMatrix();               // 保存原矩阵
	glLoadIdentity();             // 装入单位矩阵
	
	glTranslatef(x, y, 0);
	glScalef(width, length, 1);
	glBegin(GL_QUADS);
	glColor3ub(255, 0, 0);
	glVertex2f(0, 0);
	glColor3ub(0, 255, 0);
	glVertex2f(0, 1);
	glColor3ub(0, 0, 255);
	glVertex2f(1, 1);
	glColor3ub(0, 0, 0);
	glVertex2f(1, 0);
	glEnd();

	glMatrixMode(GL_PROJECTION);  // 选择投影矩阵
	glPopMatrix();                // 重置为原保存矩阵
	glMatrixMode(GL_MODELVIEW);   // 选择Modelview矩阵
	glPopMatrix();                // 重置为原保存矩阵
	glEnable(GL_DEPTH_TEST);
}