#include "BasicObject.h"
#include "interact.h"
#include <GL/glut.h>
#include <thread>
using namespace std;

void draw(void){
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glutSolidTeapot(1);
	glPopMatrix();
}

void draw2(int z){
	glPushMatrix();
	glTranslatef(0, 0, z);
	
	glutSolidTeapot(1);
	glPopMatrix();
}
void task(){
	int x = -5;
	auto f = [=](){draw2(x); };
	BasicObject obj1("1",draw);
	BasicObject obj2("2", f);
	BasicObjectManager::addGlObj(obj1);
	BasicObjectManager::addGlObj(obj2);
	char*argv[] = { "1", 0 };
	startApp(1,argv);
}
void main(){
	task();

}
