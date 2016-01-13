#ifndef __GLOBJMGR_H__
#define __GLOBJMGR_H__
#include <vector>
#include <map>
#include <set>
#include "globject.h"
#include "objstream.h"
#include "vec.h"

#define MAX_TEX 30
#define MAX_OBJ 30
class GLObjectManager{
public:
	GLObjectManager();
	std::vector<glObject> objs;
	std::map<int, int> ids; //id to index of objs
	std::set<int> objFile;

	int currentId = 0;
	//vector<GLint, int> names;
	std::vector<std::string> texNameVec;
	std::vector<std::string> objNameVec;
	int getId(){
		return currentId++;
	}
	void newId();
	void subId(){
		currentId--;
	}

	void removeById(int id);
	void removeByIndex(int index);
	glObject& getById(int id);
	glObject& getByIndex(int index);
	//draw pictures without shadow using draw*
	//draw pictures with shadow using add*
	//and firstly drawAll, then using addShadowObject then drawShadow
	void drawAll();

	bool testCrash(Vec3f s, Vec3f d);

	void setTexByIndex(int index, int texId);
	void loadTexByIndex(int index, const char *file);
	void openTexByIndex(int index);
	void closeTexByIndex(int index);

	void setTexById(int id, int tex);
	void loadTexById(int id, const char *file);
	void openTexById(int id);
	void closeTexById(int id);
	void setDefaultMaterial();

	glObject& addObj(char *file);
	void scale(int id, float *s);
	void translate(int id, float *t);
	void rotate(int id, float *r);
	//Á¢·½Ìå
	glObject& addCube(float len = 1);
	void drawCube(Vec3f center = Vec3f(0, 0, 0), Vec3f size = Vec3f(1, 1, 1));
	//ÇòÌå
	glObject& addSphere(float radius = 1, int slices = 20);
	void drawSphere(Vec3f center = Vec3f(0, 0, 0), Vec3f size = Vec3f(1, 1, 1));
	//Ô²ÖùºÍÔ²Ì¨
	glObject& addCylinder(float radius, float height, int slices = 50);
	glObject& addCylinder2(float baseR, float topR, float height, int slices = 50);

	void drawCylinder(Vec2f size = Vec2f(1, 1), Vec3f center = Vec3f(0, 0, 0), int slices = 16);
	void drawCylinder2(float baseR, float topR, float height, Vec3f center = Vec3f(0, 0, 0), int slices = 16);
	//Ô²×¶ºÍ×µÌå
	glObject& addCone(float radius, float height, int slices = 50);
	glObject& addCone2(float radius, float height, int slices);

	void drawCone(float radius, float height, int slices = 16, Vec3f center = Vec3f(0, 0, 0));
	void drawCone2(float radius, float height, int slices, Vec3f center = Vec3f(0, 0, 0));
	//ÀâÖùºÍÀâÌ¨
	glObject& addPrism(float radius, float height, int slices);
	glObject& addPrism2(float baseR, float topR, float height, int slices);

	void drawPrism(float radius, float height, int slices, Vec3f baseCenter = Vec3f(0, 0, 0));
	void drawPrism2(float baseR, float topR, float height, int slices, Vec3f baseCenter = Vec3f(0, 0, 0));
	void drawPrismSide(float baseR, float topR, float height, int slices);
	//¶à±ßÐÎÆ½Ãæ
	glObject& addPlane(float len, int slices);
	glObject& addCircle(float radius, int slices = 16);
	void drawPlane(float length, int dir = 1, Vec3f center = Vec3f(0, 0, 0));
	void drawCircle(float radius, int slices, int dir = 1, Vec3f center = Vec3f(0, 0, 0));
};

#endif