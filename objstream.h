#ifndef __OBJSTREAM_H__
#define __OBJSTREAM_H__
#include <string>
#include <vector>
#include <map>
#include "loader.h"
#include "vec.h"
#include "globject.h"

/*struct Face{
	int v[3];
	int n[3];
	int t[3];
};*/


class Obj{
public:
	std::vector<Vec3f> vArr;
	std::vector<Vec2f> vtArr;
	std::vector<Vec3f> vnArr;
	std::vector<std::vector<sPlane> > fArr;
	std::vector<std::string> names;
	std::map<std::string, Tex> textures;
	vector<glObject> objs;

	void read(std::string file, int id);
	void drawObj();
	void display();
};

#endif