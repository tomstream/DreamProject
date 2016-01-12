#ifndef __LIGHT_H__
#define __LIGHT_H__
#include <vector>
#include <set>
#include "vec.h"
struct RGB{
	float r;
	float g;
	float b;
};
struct Pos{
	float p[4];
};
class Light{public:
	std::vector<int> light_num;
	std::vector<RGB> intensity;
	std::vector<Pos> pLight;
	std::set<int> used;

	void addLight(Vec3f pos = Vec3f(5, 5, 5));
	void addSpotLight(Vec3f pos = Vec3f(5, 5, 5), float cutoff = 45, Vec3f dir = Vec3f(0, -1, 0), float exp = 10.0);
	void rmLight(int index);
	void closeLight(int index);
	void openLight(int index);
	void addIntensity(int index, int channel);
	void subIntensity(int index, int channel);
	void changeX(int index, float len);
	void changeY(int index, float len);
	void changeZ(int index, float len);
	void enableLights();
	void drawLights();
};

#endif