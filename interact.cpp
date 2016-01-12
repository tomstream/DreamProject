#define _CRT_SECURE_NO_WARNINGS
#include "interact.h"
#include "globject.h"
#include "globjmgr.h"
#include "file.h"
#include <iostream>
#include <cstdio>
#include <vector>
#include <GL/glut.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <thread>
#include <mutex>
#include <windows.h>
using namespace std;
using namespace cv;
#define UNSELECTED -1;

GLObjectManager basic;
cv::VideoCapture cap(0);

mutex mtx;
Mat frame;
Mat texttmp;

static int selectedId = UNSELECTED;
int wWidth=0;
int wHeight=0;
float whRatio;

Light light;
float LightPos[] = { 5.0f, 5.0f, 5.0f, 1.0f };

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
	CUBE,
	SPHERE,
	CYLINDER,
	CYLINDER2,
	CONE,
	CONE2,
	PRISM,
	PRISM2,
	REMOVE,
	QUIT_MENU,
	ROMOVE_TEX,
	CAMERA_TEX,
	DISFFUSE_R = GL_DIFFUSE * 4,
	DISFFUSE_G = GL_DIFFUSE * 4 + 1,
	DISFFUSE_B = GL_DIFFUSE * 4 + 2,
	DISFFUSE_A = GL_DIFFUSE * 4 + 3,
	AMBIENT_R = GL_AMBIENT * 4,
	AMBIENT_G = GL_AMBIENT * 4 + 1,
	AMBIENT_B = GL_AMBIENT * 4 + 2,
	AMBIENT_A = GL_AMBIENT * 4 + 3,
	SPECULAR_R = GL_SPECULAR * 4,
	SPECULAR_B = GL_SPECULAR * 4 + 1,
	SPECULAR_G = GL_SPECULAR * 4 + 2,
	SPECULAR_A = GL_SPECULAR * 4 + 3,
	EMISSION_R = GL_EMISSION * 4,
	EMISSION_G = GL_EMISSION * 4 + 1,
	EMISSION_B = GL_EMISSION * 4 + 2,
	EMISSION_A = GL_EMISSION * 4 + 3,
	TEXTURE0 = 222,
	OBJ0 = 333
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
static int emission;
static int val;
static int item;
static int transformation;
static int obj;

char textBuffer[100]="";
unsigned int cameratexid;

void cvInit(){
	cap.set(CV_CAP_PROP_FRAME_WIDTH, wWidth);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, wHeight);

	if (!cap.isOpened())
	{
		cout << "Could not initialize capturing...\n";
	}
	namedWindow("Cap", 1);
	glGenTextures(1, &cameratexid);
}

void capture_thread(){
	while (1){
		mtx.lock();
		cap >> frame;
		mtx.unlock();
		imshow("Cap", frame);
		waitKey(150);
	}
}
void createMainMenu(void){
	if (menuState == MAIN)return;
	menuVec.clear();
	obj= glutCreateMenu(menu);
	auto& objNameVec = basic.objNameVec;
	for (int i = 0; i < objNameVec.size(); i++){
		glutAddMenuEntry(objNameVec[i].c_str(), OBJ0 + i);
	}
	valueMode = false;
	tranformMode = -1;

	item = glutCreateMenu(menu);
	glutAddMenuEntry("导入立方体", CUBE);
	
	glutAddMenuEntry("导入球体", SPHERE);
	glutAddMenuEntry("导入圆柱", CYLINDER);
	glutAddMenuEntry("导入圆台", CYLINDER2);
	glutAddMenuEntry("导入圆锥", CONE);
	glutAddMenuEntry("导入锥体", CONE2);
	glutAddMenuEntry("导入棱柱", PRISM);
	glutAddMenuEntry("导入棱台", PRISM2);
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
		glObject &obj = basic.getById(selectedId);
		transformArray = obj.t;
		tranformMode = TRANSLATE;
	}
	else if (value == SCALE){
		glObject &obj = basic.getById(selectedId);
		transformArray = obj.s;
		tranformMode = SCALE;
	}
	else if (value == ROTATE){
		glObject &obj = basic.getById(selectedId);
		transformArray = obj.r;
		tranformMode = ROTATE;
	}
	else if (value == CUBE){
		glObject &obj = basic.addCube();
		selectedId=obj.id;
	}
	else if (value == SPHERE){
		glObject &obj = basic.addSphere();
		selectedId = obj.id;
		createItemMenu();
	}
	else if (value == CYLINDER){
		glObject &obj = basic.addCylinder(1,1);
		selectedId = obj.id;
		createItemMenu();
	}
	else if (value == CYLINDER2){
		float baseR, topR, height;
		cout << "Input [baseR][topR][height]" << endl<<">";
		cin >> baseR >> topR >> height;
		glObject &obj = basic.addCylinder2(baseR,topR,height);
		selectedId = obj.id;
		createItemMenu();
	}
	else if (value == CONE){
		glObject &obj = basic.addCone(1,1);
		selectedId = obj.id;
		createItemMenu();
	}
	else if (value == CONE2){
		float radius, height, slices;
		cout << "Input [radius][height][slice]" << endl<<">";
		cin >> radius >> height >> slices;
		glObject &obj = basic.addCone2(radius,height,slices);
		selectedId = obj.id;
		createItemMenu();
	}
	else if (value == PRISM){
		float radius, height, slices;
		cout << "Input [radius][height][slice]" << endl << ">";
		cin >> radius >> height >> slices;
		glObject &obj = basic.addPrism(radius, height, slices);
		selectedId = obj.id;
		createItemMenu();
	}
	else if (value == PRISM2){
		float baseR,topR, height, slices;
		cout << "Input [radius][height][slice]" << endl << ">";
		cin >> baseR>>topR >> height >> slices;
		glObject &obj = basic.addPrism2(baseR,topR, height, slices);
		createItemMenu();
	}
	else if (value == REMOVE){
		basic.removeById(selectedId);
		createMainMenu();
	}
	else if (value == QUIT_MENU){
		createMainMenu();
	}
	else if (value == ROMOVE_TEX){
		basic.closeTexById(selectedId);
	}
	else if (value == CAMERA_TEX){
		glObject& obj = basic.getById(selectedId);
		obj.hasTex = true;
		obj.tex.texture.texID = cameratexid;
	}
	else if (value >= TEXTURE0&&value<TEXTURE0+MAX_TEX){
		int textureNum = value - TEXTURE0;
		glObject &obj = basic.getById(selectedId);
		auto& texNameVec = basic.texNameVec;
		basic.loadTexById(selectedId,texNameVec[textureNum].c_str());
		basic.openTexById(selectedId);
		
	}
	else if (value>=OBJ0&&value < OBJ0 + 30){
		int objNum = value - OBJ0;
		auto& objNameVec = basic.objNameVec;
		string objName = objNameVec[objNum];
		glObject &obj = basic.addObj((char*)(objName.c_str()));
		basic.openTexById(obj.id);
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
			float value = x * 1.0/ wWidth;
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
	glutAddMenuEntry("清除纹理", ROMOVE_TEX);
	glutAddMenuEntry("相机纹理", CAMERA_TEX);
	auto& texNameVec = basic.texNameVec;
	for (int i = 0; i < texNameVec.size();i++){
		glutAddMenuEntry(texNameVec[i].c_str(), TEXTURE0 + i);
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

	emission = glutCreateMenu(menu);
	glutAddMenuEntry("R", EMISSION_R);
	glutAddMenuEntry("G", EMISSION_G);
	glutAddMenuEntry("B", EMISSION_B);
	glutAddMenuEntry("A", EMISSION_A);

	material = glutCreateMenu(menu);
	glutAddSubMenu("环境光", ambient);
	glutAddSubMenu("散射光", disffuse);
	glutAddSubMenu("镜面反射光",speculars );
	glutAddSubMenu("发射光",emission);

	transformation = glutCreateMenu(menu);
	glutAddMenuEntry("平移", TRANSLATE);
	glutAddMenuEntry("旋转", ROTATE);
	glutAddMenuEntry("伸缩",SCALE);
	
	menuid = glutCreateMenu(menu);
	glutAddSubMenu("纹理", texture);
		

	glutAddSubMenu("几何变换", transformation);


	glutAddSubMenu("材料", material);
	glutAddMenuEntry("移除", REMOVE);
	glutAddMenuEntry("退出菜单", QUIT_MENU);
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

	if (tranformMode != -1){ valueMode = 0; return; }
	if (valueMode){
		glObject &obj = basic.getById(selectedId);
		int materialpara = currentMaterialMode / 4;
		int materialNum = currentMaterialMode % 4; 
		float* arr = obj.getMaterialfv(materialpara);
		arr[materialNum] = currentValue;
		textBuffer[0] = 0;
		valueMode = false;
		return;
	}
	//get viewport and select buffer
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, selectBuf);
	glRenderMode(GL_SELECT);

	//initialize names
	glInitNames();
	glPushName(-1);

	//save projection matrix
	//glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	
	//set pick matrix
	//gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 1.0, 1.0, viewport);
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 2.0, 2.0, viewport);

	//set projection matrix for select mode
	whRatio = (GLfloat)wWidth / (GLfloat)wHeight;
	if (bPersp) {
		gluPerspective(45.0f, whRatio, 0.001f, 1000.0f);
		//glFrustum(-3, 3, -3, 3, 3,100);
	}
	else {
		glOrtho(-3, 3, -3, 3, -100, 100);
	}
	//draw in select mode and name objects
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);
	glRotatef(fRotate, 0, 1, 0);
	basic.setDefaultMaterial();
	   
	basic.drawAll();

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
	/*unsigned int* p = selectBuf;
	for (int i = 0; i < 6 * 4; i++)
	{
		printf("Slot %d: - Value: %d\n", i, p[i]);
	}

	printf("Buff size: %x\n", (GLbyte)p[0]);
	*/
	glMatrixMode(GL_MODELVIEW);
}
int state = 0;


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

		//geometry transform
	case 'Q':tranformMode = -1; textBuffer[0] = 0;
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

	case 'P':SnapScreen(); break;
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
	updateView(wWidth,wHeight);
	glutPostRedisplay();
}

void updateTransformArray(int mode, bool pos,int count, float* p){
	float tarray[3] = {};
	if (mode == -1)return;
	else{
		if (mode == TRANSLATE){
			if (pos)tarray[count] = 0.1;
			else tarray[count] = -0.1;
			basic.translate(selectedId, tarray);
		}
		else if (mode == SCALE){
			tarray[0] = tarray[1] = tarray[2] = 1;
			if (pos)tarray[count] = 0.95;
			else tarray[count] =1/0.95;
			basic.scale(selectedId, tarray);
		}
		else if (mode == ROTATE){
			if (pos)tarray[count] = 5;
			else tarray[count] =- 5;
			basic.rotate(selectedId, tarray);
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
	/*
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
	*/
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
	mtx.lock();
	cvtColor(frame, texttmp, CV_BGR2RGB);
	mtx.unlock();
	flip(texttmp, texttmp, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, cameratexid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, wWidth, wHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texttmp.data);
	glDisable(GL_TEXTURE_2D);

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
	

	glLightfv(GL_LIGHT1, GL_POSITION, lp);
	light.enableLights();
	
	glRotatef(fRotate, 0, 1, 0);
	basic.setDefaultMaterial();

	basic.drawAll();

	//basic.objs[0].r[0] = 180;

	for (int i = 0; i < basic.objs.size(); ++i)
		addShadowObject(basic.objs[i], light);
	drawShadow();

	glPushMatrix();
	glTranslatef(0, -3, 0);
	glutSolidCube(1.0);
	glPopMatrix();

	light.drawLights();
	if (bAnim) fRotate += 0.5f;
	drawText(textBuffer, 100, 100);
	if (valueMode)
		drawRectangle(0, wHeight - 20, wWidth, 20);
	glFlush();
	glutSwapBuffers();
	
}


void updateView(int width, int height)
{
							// Reset The Current Viewport
	glViewport(0, 0, wWidth , wHeight);
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

	wHeight = height;
	wWidth = width;

	updateView(width, height);
}

void startApp(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);
	int windowHandle = glutCreateWindow("Simple GLUT App");
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouseClick);
	glutKeyboardFunc(key);
	glutPassiveMotionFunc(motion);
	createMainMenu();
	cvInit();
	std::thread cv_thread(capture_thread);
	cv_thread.detach();
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
	glOrtho(0, wWidth, 0, wHeight, -1, 1);    // 位置正投影
	glMatrixMode(GL_MODELVIEW);   // 选择Modelview矩阵
	glPushMatrix();               // 保存原矩阵
	glLoadIdentity();             // 装入单位矩阵
	glRasterPos2f(x, wHeight-y);
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
	glOrtho(0, wWidth, 0, wHeight, -1, 1);    // 位置正投影
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

