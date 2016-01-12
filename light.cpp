#include "light.h"
#include <iostream>
#include "glut.h"
using namespace std;
void Light::addLight(Vec3f pos){
	if (light_num.size() == 3){
		cout << "WARNING: Too much lights. Adding lights denied." << endl;
		return;
	}
	int num = GL_LIGHT0;
	while (used.count(num)) num++;
	light_num.push_back(num);
	used.insert(num);

	RGB t;
	t.r = t.g = t.b = 1.0f;
	GLfloat color[4] = { t.r, t.g, t.b, 1.0 };
	GLfloat no[4] = { 0, 0, 0, 1.0 };
	glLightfv(num, GL_DIFFUSE, color);
	glLightfv(num, GL_AMBIENT, color);
	glLightfv(num, GL_SPECULAR, no);
	glLightf(num, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(num, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(num, GL_QUADRATIC_ATTENUATION, 0.001);
	intensity.push_back(t);
	GLfloat position[4] = { pos[0], pos[1], pos[2], 1 };
	Pos tp;
	tp.p[0] = pos[0]; tp.p[1] = pos[1]; tp.p[2] = pos[2]; tp.p[3] = 1;
	pLight.push_back(tp);
	glLightfv(num, GL_POSITION, position);
	glEnable(num);
}

void Light::addSpotLight(Vec3f pos, float cutoff, Vec3f dir, float exp){
	if (light_num.size() == 3){
		cout << "WARNING: Too much lights. Adding lights denied." << endl;
		return;
	}
	int num = GL_LIGHT4;
	while (used.count(num)) num++;
	light_num.push_back(num);
	used.insert(num);

	RGB t;
	t.r = t.g = t.b = 1.0f;
	GLfloat color[4] = { t.r, t.g, t.b, 1.0 };
	GLfloat no[4] = { 0, 0, 0, 1.0 };
	float direction[3] = { dir[0], dir[1], dir[2] };
	glLightfv(num, GL_DIFFUSE, color);
	glLightfv(num, GL_AMBIENT, color);
	glLightfv(num, GL_SPECULAR, no);
	glLightf(num, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(num, GL_SPOT_CUTOFF, cutoff);
	glLightfv(num, GL_SPOT_DIRECTION, direction);
	glLightf(num, GL_SPOT_EXPONENT, exp);
	intensity.push_back(t);
	GLfloat position[4] = { pos[0], pos[1], pos[2], 1 };
	Pos tp;
	tp.p[0] = pos[0]; tp.p[1] = pos[1]; tp.p[2] = pos[2]; tp.p[3] = 1;
	pLight.push_back(tp);
	glLightfv(num, GL_POSITION, position);
	glEnable(num);
}

void Light::setSpotCutoff(int index, float cutoff){
	int num = light_num[index];
	if (num < GL_LIGHT4) return;
	glLightf(num, GL_SPOT_CUTOFF, cutoff);
}

void Light::setSpotDir(int index, float *dir){
	int num = light_num[index];
	if (num < GL_LIGHT4) return;
	glLightfv(num, GL_SPOT_DIRECTION, dir);
}

void Light::setSpotExponent(int index, float exp){
	int num = light_num[index];
	if (num < GL_LIGHT4) return;
	glLightf(num, GL_SPOT_EXPONENT, exp);
}

void Light::rmLight(int index){
	if (light_num.size() == 0) return;
	int num = light_num[index];
	glDisable(num);
	light_num.erase(light_num.begin() + index);
	intensity.erase(intensity.begin() + index);
	used.erase(used.find(num));
	pLight.erase(pLight.begin() + index);
}

void Light::closeLight(int index){
	if (light_num.size() == 0) return;
	int num = light_num[index];
	glDisable(num);
}

void Light::openLight(int index){
	if (light_num.size() == 0) return;
	int num = light_num[index];
	glEnable(num);
}

void Light::addIntensity(int index, int channel){
	int num = light_num[index];
	RGB &t = intensity[index];
	switch (channel){
	case 0:
		t.r += 0.1;
		if (t.r > 1.0) t.r = 1.0;
		break;
	case 1:
		t.g += 0.1;
		if (t.g > 1.0) t.g = 1.0;
		break;
	case 2:
		t.b += 0.1;
		if (t.b > 1.0) t.b = 1.0;
		break;
	}
	GLfloat color[4] = { t.r, t.g, t.b, 1.0 };
	GLfloat no[4] = { 0, 0, 0, 1.0 };
	glLightfv(num, GL_DIFFUSE, color);
	glLightfv(num, GL_AMBIENT, color);
	glLightfv(num, GL_SPECULAR, no);
	glEnable(num);
}

void Light::subIntensity(int index, int channel){
	int num = light_num[index];
	RGB &t = intensity[index];
	switch (channel){
	case 0:
		t.r -= 0.1;
		if (t.r < 0.0) t.r = 0.0;
		break;
	case 1:
		t.g -= 0.1;
		if (t.g < 0.0) t.g = 0.0;
		break;
	case 2:
		t.b -= 0.1;
		if (t.b < 0.0) t.b = 0.0;
		break;
	}
	GLfloat color[4] = { t.r, t.g, t.b, 1.0 };
	GLfloat no[4] = { 0, 0, 0, 1.0 };
	glLightfv(num, GL_DIFFUSE, color);
	glLightfv(num, GL_AMBIENT, color);
	glLightfv(num, GL_SPECULAR, no);
	glEnable(num);
}

void Light::changeX(int index, float len){
	int num = light_num[index];
	Pos &t = pLight[index];
	t.p[0] += len;
	cout << t.p[0] << ' ' << t.p[1] << ' ' << t.p[2] << ' ' << t.p[3] << endl;
	cout << num << endl;
	//glEnable(num);
}
void Light::changeY(int index, float len){
	int num = light_num[index];
	Pos &t = pLight[index];
	t.p[1] += len;
	//glLightfv(num, GL_POSITION, t.p);
	//glEnable(num);
}
void Light::changeZ(int index, float len){
	int num = light_num[index];
	Pos &t = pLight[index];
	t.p[2] += len;
	//glLightfv(num, GL_POSITION, t.p);
	//glEnable(num);
}

void Light::enableLights(){
	for (int i = 0; i < light_num.size(); ++i){

		int num = light_num[i];
		Pos &t = pLight[i];
		//GLfloat position[4] = { t.p[0], t.p[1], t.p[2], 1 };

		glLightfv(num, GL_POSITION, t.p);
		glEnable(num);
	}
}

void Light::drawLights(){
	for (int i = 0; i < light_num.size(); ++i){
		glColor3f(intensity[i].r * 0.5, intensity[i].g * 0.5, intensity[i].b * 0.5);
		glDisable(GL_LIGHTING);
		glDepthMask(GL_FALSE);
		glPushMatrix();
		glTranslatef(pLight[i].p[0], pLight[i].p[1], pLight[i].p[2]);
		glutSolidSphere(0.2, 16, 16);
		glPopMatrix();
		glEnable(GL_LIGHTING);
		glDepthMask(GL_TRUE);
	}
}

void Light::setAmbient(int index, float *ambient){
	int num = light_num[index];
	glLightfv(num, GL_AMBIENT, ambient);
}

void Light::setDiffuse(int index, float *diffuse){
	int num = light_num[index];
	glLightfv(num, GL_DIFFUSE, diffuse);
}

void Light::setSpecular(int index, float *specular){
	int num = light_num[index];
	glLightfv(num, GL_SPECULAR, specular);
}

void Light::setEmission(int index, float *emission){
	int num = light_num[index];
	glLightfv(num, GL_EMISSION, emission);
}


