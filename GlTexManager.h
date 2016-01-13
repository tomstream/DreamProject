#pragma once
#include <windows.h>
#include <map>
#include <set>
#include <string>
#define MAX_TEX 30
class GlTexManager
{
public:
	GlTexManager();
	~GlTexManager();
	static bool loadTex(const char *filename);
	static bool texload(const char *filename, int i);
	static void addParas(int i, std::pair<int, int> para);
	static void init();
	static std::set<std::string> textureNameSet;
	static std::map<int,int> defaults;
	static int defaultEnv;
	static void drawdefaultTexSetting();
	static std::map<std::string, unsigned int> textureMap;
private:
	static unsigned char *GlTexManager::LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	static int textureNum;
	static unsigned int texture[MAX_TEX];
	
};

