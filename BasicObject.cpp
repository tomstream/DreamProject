#include "BasicObject.h"
#include "GLTexManager.h"
#include "glObject.h"
#include "basic.h"
#include <algorithm>
#include <cassert>
#include <GL/glut.h>


using namespace std;

const float ambientDefault[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
const float diffuseDefault[4] = {0.8,0.8,0.8,1.0};
const float specularDefault[4] = { 0, 0, 0, 1.0f };
const float shininessDefault[1] = {0};
const float emissionDefault[4] = {0,0,0.1,1.0};
const float colorIndexDefault[3] = {0,1,1};

BasicObject::BasicObject(string s, std::function<void(void)>f)
{
	setName(s);
	transArray = new float[3];
	rotateArray = new float[3];
	scaleArray = new float[3];
	id = BasicObjectManager::getNewId();
	draw = f;
	for (int i = 0; i < 3; i++){
		transArray[i] = rotateArray[i] = 0;
		scaleArray[i] = 1;
	}
	for (int i = 0; i < 4; i++){
		ambient[i] = ambientDefault[i];
		diffuse[i] = diffuseDefault[i];
		specular[i] = specularDefault[i];
	}shininess[0] = shininessDefault[0];

	materialFMap[GL_AMBIENT] = ambient;
	materialFMap[GL_DIFFUSE] = diffuse;
	materialFMap[GL_SPECULAR] = specular;
	materialFMap[GL_SHININESS] = shininess;
	rotateArray[3] = 0;
	textureId = 0;
	textureEnv = GlTexManager::defaultEnv;
	textureMap = GlTexManager::defaults;

}

BasicObject::BasicObject(glObject* glObj){
	transArray = glObj->t;
	rotateArray = glObj->r;
	scaleArray = glObj->s;
	id = BasicObjectManager::getNewId();
	
	for (int i = 0; i < 3; i++){
		transArray[i] = rotateArray[i] = 0;
		scaleArray[i] = 1;
	}
	for (int i = 0; i < 4; i++){
		ambient[i] = ambientDefault[i];
		diffuse[i] = diffuseDefault[i];
		specular[i] = specularDefault[i];
	}shininess[0] = shininessDefault[0];

	materialFMap[GL_AMBIENT] = ambient;
	materialFMap[GL_DIFFUSE] = diffuse;
	materialFMap[GL_SPECULAR] = specular;
	materialFMap[GL_SHININESS] = shininess;
	rotateArray[3] = 0;
	textureId = 0;
	textureEnv = GlTexManager::defaultEnv;
	textureMap = GlTexManager::defaults;
}

BasicObject::BasicObject(string s, std::function<void(void)>f, float* transArray, float* scaleArray, float* rotateArray)
:BasicObject(s,f){
	for (int i = 0; i < 3; i++){
		this->transArray[i] = transArray[i];
		this->scaleArray[i] = scaleArray[i];
		this->rotateArray[i] = rotateArray[i];
	}
}

void BasicObject::setName(string s){
	name = s;
}

string BasicObject::getName(){
	return name;
}

unsigned int BasicObject::getId(){
	return id;
}
void BasicObject::setTexEnv(int para){
	textureEnv = para;
}

void BasicObject::drawObj(int mode){
	if (mode==GL_SELECT)
		glLoadName(this->getId());
	
	//texture
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glPushMatrix();
	if (!this->textureId){
		for (auto& para : this->textureMap){
			glTexParameteri(GL_TEXTURE_2D, para.first, para.second);
		}
	}
	for (auto &con : this->materialFMap){
		int para1 = con.first;
		float* mat = con.second;
		glMaterialfv(GL_FRONT_AND_BACK, para1, mat);
	}
	glTranslatef(transArray[0], transArray[1], transArray[2]);
	glRotatef(rotateArray[0], 1, 0, 0);
	glRotatef(rotateArray[1], 0, 1, 0);
	glRotatef(rotateArray[2], 0, 0, 1);
	glScalef(scaleArray[0],scaleArray[1],scaleArray[2]);
	this->draw();
	//if (!this->textureId){
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, this->textureEnv);
	//}
	//reset
	if (this->materialFMap.size() != 0){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientDefault);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseDefault);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularDefault);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininessDefault);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissionDefault);
		glMaterialfv(GL_FRONT_AND_BACK, GL_COLOR_INDEX, colorIndexDefault);
	}
	glPopMatrix();
}
float* BasicObject::getMaterialfv(int para) {
	return materialFMap[para];
}

BasicObject::~BasicObject()
{
}

std::map<unsigned int, BasicObject> BasicObjectManager::objMap;
unsigned int BasicObjectManager::currentId;

void BasicObjectManager::init(){
	currentId = 0;
	objMap.clear();
}

unsigned int BasicObjectManager::getNewId(){
	currentId += 1;
	return currentId;
}

void BasicObjectManager::addGlObj(BasicObject& obj){
	objMap[obj.getId()] = obj;
}

BasicObject& BasicObjectManager::getGlObj(unsigned int id){
	return objMap[id];
}

void BasicObjectManager::dropGlObj(unsigned int id){
	objMap.erase(id);
}

void BasicObjectManager::drawAll(int mode){
	for (auto& obj : objMap){
		obj.second.drawObj(mode);
	}
	glLoadIdentity();
}