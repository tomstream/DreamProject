#define _CRT_SECURE_NO_WARNINGS

#include "GlTexManager.h"
#include <cstdio>
#include <GL/glut.h>
#include <algorithm>
#include "file.h"
#define BITMAP_ID 0x4D42
using namespace std;

unsigned int GlTexManager::texture[MAX_TEX];
map<int, int> GlTexManager::defaults;
int GlTexManager::textureNum = 0;
int GlTexManager::defaultEnv = GL_MODULATE;
std::map<string, unsigned int> GlTexManager::textureMap;

GlTexManager::GlTexManager()
{
}

GlTexManager::~GlTexManager()
{
}

unsigned char *GlTexManager::LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;	// �ļ�ָ��
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap�ļ�ͷ
	unsigned char	*bitmapImage;		// bitmapͼ������
	int	imageIdx = 0;		// ͼ��λ������
	unsigned char	tempRGB;	// ��������

	// �ԡ�������+����ģʽ���ļ�filename 
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL) return NULL;
	// ����bitmap�ļ�ͼ
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// ��֤�Ƿ�Ϊbitmap�ļ�
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// ����bitmap��Ϣͷ
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// ���ļ�ָ������bitmap����
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// Ϊװ��ͼ�����ݴ����㹻���ڴ�
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// ��֤�ڴ��Ƿ񴴽��ɹ�
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// ����bitmapͼ������
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// ȷ�϶���ɹ�
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//����bitmap�б���ĸ�ʽ��BGR�����潻��R��B��ֵ���õ�RGB��ʽ
	for (imageIdx = 0;
		imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	auto myswap = [](unsigned char& a,unsigned char& b){
		int tmp = a;
		a = b;
		b = tmp;
	};
	/*
	for (imageIdx = 0; imageIdx <= bitmapInfoHeader->biSizeImage; imageIdx += 3){
		for (int i = 0; i<3; i++){
			myswap(bitmapImage[imageIdx + i], bitmapImage[3 * (bitmapInfoHeader->biSizeImage - 1 - imageIdx) + i]);
		}
	}*/
	//reverse the image
	// �ر�bitmapͼ���ļ�
	fclose(filePtr);
	return bitmapImage;
}

bool GlTexManager::texload(const char *filename, int i){
 	int ret=0;
	BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap��Ϣͷ
	unsigned char*   bitmapData;                                       // ��������

	bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
	if (!bitmapData)return false;
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	glTexCoord2f(0.0f, 1.0f);
	// ָ����ǰ����ķŴ�/��С���˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D,
		0, 	    //mipmap���(ͨ��Ϊ����ʾ���ϲ�) 
		GL_RGB,	//����ϣ���������к졢�̡�������
		bitmapInfoHeader.biWidth, //��������������n�����б߿�+2 
		bitmapInfoHeader.biHeight, //����߶ȣ�������n�����б߿�+2 
		0, //�߿�(0=�ޱ߿�, 1=�б߿�) 
		GL_RGB,	//bitmap���ݵĸ�ʽ
		GL_UNSIGNED_BYTE, //ÿ����ɫ���ݵ�����
		bitmapData);	//bitmap����ָ��  
	return true;
}

bool GlTexManager::loadTex(const char* filename){
	if (texload(filename,textureNum)){
		textureNum += 1;
		return true;
	}
	else{
		return false;
	}
}

void GlTexManager::init(){
	glGenTextures(MAX_TEX, texture);
	glEnable(GL_TEXTURE_2D);

	defaults[GL_TEXTURE_MAG_FILTER]=GL_NEAREST;
	defaults[GL_TEXTURE_MIN_FILTER] = GL_NEAREST;
	defaults[GL_TEXTURE_WRAP_S] = GL_REPEAT;
	defaults[GL_TEXTURE_WRAP_T] = GL_REPEAT;
	vector<string> fileName;
	getFiles("texture", fileName);
	for (auto &str : fileName){
		string last3 = string(str.end() - 4, str.end());
		if (last3 != ".bmp"){
			continue;
		}
		else{
			if (!loadTex(str.c_str())){
				continue;
			}
			textureMap[str]=textureNum-1;
		}
	}
}

