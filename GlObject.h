#pragma once
#include <string>
#include <map>
#include <utility>
#include <functional>

class GlObject
{
private:
	std::string name;
	unsigned int textureId;
	std::map<int, int> textureMap;
	int textureEnv;
	unsigned int id;
	std::map<int, float*> materialFMap;
public:
	float transArray[3];
	float rotateArray[4];
	float scaleArray[3];

	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess[1];
	GlObject(std::string s, std::function<void(void)>f);
	GlObject(string s, std::function<void(void)>f, float* transArray, float* scaleArray, float* rotateArray);
	GlObject() = default;
	void setTexEnv(int para);
	void setName(std::string s);
	float* getMaterialfv(int para);
	void setTexture(int count){
		textureId = count+1;
	}
	std::string getName();
	unsigned int getId();
	~GlObject();
	void drawObj();
	std::function<void(void)> draw;
};

class GlObjectManager{
private:
	static std::map<unsigned int, GlObject> objMap;
	static unsigned int currentId;
public:
	static void init();
	static unsigned int getNewId();
	static void addGlObj(GlObject& obj);
	static GlObject& getGlObj(unsigned int id);
	static void dropGlObj(unsigned int id);
	static void drawAll();
};
