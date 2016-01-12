#pragma once
#include <string>
#include <map>
#include <utility>
#include <functional>
struct glObject;

class BasicObject
{
private:
	std::string name;
	unsigned int textureId;
	std::map<int, int> textureMap;
	int textureEnv;
	unsigned int id;
	std::map<int, float*> materialFMap;
	glObject* pObj;
public:
	float* transArray;
	float* rotateArray;
	float* scaleArray;

	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess[1];
	BasicObject(std::string s, std::function<void(void)>f);
	BasicObject(std::string s, std::function<void(void)>f, float* transArray, float* scaleArray, float* rotateArray);
	BasicObject(glObject* glObj);
	BasicObject() = default;
	void setTexEnv(int para);
	void setName(std::string s);
	float* getMaterialfv(int para);
	void setTexture(int count){
		textureId = count+1;
	}
	std::string getName();
	unsigned int getId();
	~BasicObject();
	void drawObj(int mode);
	std::function<void(void)> draw;
};

class BasicObjectManager{
private:
	static std::map<unsigned int, BasicObject> objMap;
	static unsigned int currentId;
public:
	static void init();
	static unsigned int getNewId();
	static void addGlObj(BasicObject& obj);
	static BasicObject& getGlObj(unsigned int id);
	static void dropGlObj(unsigned int id);
	static void drawAll(int mode);
};
