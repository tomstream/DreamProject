#include "interact.h"

#include <thread>
#include <GL/glut.h>
using namespace std;

void task(){
	char*argv[] = { "1", 0 };
	startApp(1, argv);
}
void main(){
	task();

}
