#ifndef __GLOBJECT_H__
#define __GLOBJECT_H__
#include "vec.h"
#include "loader.h"
#include "light.h"
#include <vector>
#include <map>
#include <string>
typedef float GLvector4f[4];				
typedef float GLmatrix16f[16];
struct Tex{
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emission[4];
	TextureImage texture;
	Tex(){
		for (int i = 0; i < 3; ++i){
			ambient[i] = 0.2;
			diffuse[i] = 0.8;
			specular[i] = 0;
			emission[i] = 0;
		}
		ambient[3] = diffuse[3] = 1.0;
		specular[3] = emission[3] = 1.0;
	}

};
struct sPoint{
	float x, y, z;
	sPoint(){};
	sPoint(float f1, float f2, float f3)
		:x(f1), y(f2), z(f3) {}
};
//平面方程
struct sPlaneEq{
	float a, b, c, d;
};
//平面
struct sPlane{
	int psize = 0;
	int p[4];
	int tsize = 0;
	int tex[4];
	sPoint normal[3];
	int nsize = 0;
	int neighbor[4];
	sPlaneEq PlaneEq;
	bool visible;
	sPlane(){
		for (int i = 0; i < 4; ++i){
			p[i] = tex[i] = neighbor[i] = 0;
		}
	}
};
struct glObject{
	glObject();

	//GLuint nPlanes, nPoints;
	int pointSize = 0;
	sPoint* points;
	int planeSize = 0;
	sPlane* planes;
	int texSize = 0;
	Vec2f* texPoints;
	Tex tex;
	bool hasTex = false;
	int id;
	float r[3];
	float s[3];
	float t[3];
	std::map<int, float*> materialFMap;

	float* getMaterialfv(int para){
		return materialFMap[para];
	}
	void setAmbient(float *ambient){
		for (int i = 0; i < 4; ++i){
			tex.ambient[i] = ambient[i];
		}
	}
	void setDiffuse(float *diffuse){
		for (int i = 0; i < 4; ++i){
			tex.diffuse[i] = diffuse[i];
		}
	}
	void setSpecular(float *specular){
		for (int i = 0; i < 4; ++i){
			tex.specular[i] = specular[i];
		}
	}
	void setEmission(float *emission){
		for (int i = 0; i < 4; ++i){
			tex.emission[i] = emission[i];
		}
	}
};
void setConnectivity(glObject &o);
void calPlane(glObject o, sPlane &plane);
void drawGLObject(glObject o);
void castShadow(glObject &o, float *lp);
void VMatMult(GLmatrix16f M, GLvector4f v);
int ReadObject(char *st, glObject *o);
void addShadowObject(glObject &o, Light light);
void drawShadow();
#endif