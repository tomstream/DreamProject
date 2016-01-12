#include "interact.h"

#include "globjmgr.h"
#include <GL/glut.h>

using namespace std;

extern GLObjectManager basic;

void init(){

}

void task(){
	char*argv[] = { "1", 0 };
	startApp(1, argv);
}
void main(){
	task();

}
