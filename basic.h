#ifndef __BASIC_H__
#define __BASIC_H__
#include <vector>
#include <map>
#include "globject.h"
#include "vec.h"
class Basic{
public:
	static Basic& getBasic(){ static Basic basic; return basic; }
	vector<glObject> objs;
	//vector<GLint, int> names;
	//draw pictures without shadow using draw*
	//draw pictures with shadow using add*
	//and firstly drawAll, then using addShadowObject then drawShadow
	void drawAll();
	void loadTex(int i, char *file);
	void openTex(int i);
	void closeTex(int i);
	void setDefaultMaterial();
	//立方体
	void addCube(float len = 1);
	void drawCube(Vec3f center = Vec3f(0, 0, 0), Vec3f size = Vec3f(1, 1, 1));
	//球体
	void addSphere(float radius = 1, int slices = 20);
	void drawSphere(Vec3f center = Vec3f(0, 0, 0), Vec3f size = Vec3f(1, 1, 1));
	//圆柱和圆台
	void addCylinder(float radius, float height, int slices = 50);
	void addCylinder2(float baseR, float topR, float height, int slices = 50);
	
	void drawCylinder(Vec2f size = Vec2f(1, 1), Vec3f center = Vec3f(0, 0, 0), int slices = 16);
	void drawCylinder2(float baseR, float topR, float height, Vec3f center = Vec3f(0, 0, 0), int slices = 16);
	//圆锥和椎体
	void addCone(float radius, float height, int slices = 50);
	void addCone2(float radius, float height, int slices);

	void drawCone(float radius, float height, int slices = 16, Vec3f center = Vec3f(0, 0, 0));
	void drawCone2(float radius, float height, int slices, Vec3f center = Vec3f(0, 0, 0));
	//棱柱和棱台
	void addPrism(float radius, float height, int slices);
	void addPrism2(float baseR, float topR, float height, int slices);

	void drawPrism(float radius, float height, int slices, Vec3f baseCenter = Vec3f(0, 0, 0));
	void drawPrism2(float baseR, float topR, float height, int slices, Vec3f baseCenter = Vec3f(0, 0, 0));
	void drawPrismSide(float baseR, float topR, float height, int slices);
	//多边形平面
	void addPlane(float len, int slices);
	void addCircle(float radius, int slices = 16);
	void drawPlane(float length, int dir = 1, Vec3f center = Vec3f(0, 0, 0));
	void drawCircle(float radius, int slices, int dir = 1, Vec3f center = Vec3f(0, 0, 0));
};

#endif