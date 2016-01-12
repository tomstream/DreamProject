#include "file.h"
#include "io.h"
#define BITMAP_ID 0x4D42
#include <windows.h>
#include <time.h>
#include <cstring>
#include <stdio.h>
#include <GL/glut.h>

extern int wWidth;
extern int wHeight;

using namespace std;

void getFiles(string path, vector<string>& files)
{
	//�ļ����
	long   hFile = 0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮
			//�������,�����б�
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
/*�ж��Ƿ�ΪͼƬ�ļ�*/
bool isPicture(string s)
{
	for (auto& c : s)
	{
		c = tolower(c);
	}
	return string(s.end() - 4, s.end()) == ".jpg"
		|| string(s.end() - 4, s.end()) == ".bmp"
		|| string(s.end() - 4, s.end()) == ".png";
}
bool SnapScreen(){
	time_t tt = time(NULL);//��䷵�ص�ֻ��һ��ʱ��cuo
	tm* t = localtime(&tt);
	static char buffer[100];
	sprintf(buffer,"Snap%d%02d%02d%02d%02d%02d",
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);
	string fileName = string("snap/") + buffer + ".bmp";
	return SnapScreen(wWidth, wHeight, fileName);
}
bool SnapScreen(int width, int height, string fileName)
{
	const char *file =fileName.c_str();
	byte *image;          //����ͼ������
	FILE *fp;            //�ļ�ָ��
	BITMAPFILEHEADER FileHeader;    //����λͼ�ļ�ͷ
	BITMAPINFOHEADER InfoHeader;    //����λͼ��Ϣͷ

	FileHeader.bfType = BITMAP_ID;                                                  //ID����Ϊλͼ��id��
	FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);      //ʵ��ͼ�����ݵ�λ�����ļ�ͷ����Ϣͷ֮��
	FileHeader.bfReserved1 = 0;                                                    //��������Ϊ0
	FileHeader.bfReserved2 = 0;                                                    //��������Ϊ0
	FileHeader.bfSize = height*width * 24 + FileHeader.bfOffBits;                      //BMPͼ���ļ���С

	InfoHeader.biXPelsPerMeter = 0;                                              //ˮƽ�ֱ��ʣ�������ʱ��Ϊ0����
	InfoHeader.biYPelsPerMeter = 0;                                              //��ֱ�ֱ��ʣ�������ʱ��Ϊ0����
	InfoHeader.biClrUsed = 0;                                                    //ͼ��ʹ�õ���ɫ��������ʱ��Ϊ0����
	InfoHeader.biClrImportant = 0;                                                //��Ҫ����ɫ����������ʱ��Ϊ0����                        //��ֱ�ֱ��ʣ�������ʱ��Ϊ0����
	InfoHeader.biPlanes = 1;                //��������Ϊ1
	InfoHeader.biCompression = BI_RGB;                                              //����ΪBI_RGBʱ,��ʾͼ��û�в�ɫ��
	InfoHeader.biBitCount = 24;                                                    //ͼ���λ��
	InfoHeader.biSize = sizeof(BITMAPINFOHEADER);                                  //�ṹ��Ĵ�С
	InfoHeader.biHeight = height;
	InfoHeader.biWidth = width;
	InfoHeader.biSizeImage = height*width * 4;

	image = (byte *)malloc(sizeof(byte)*InfoHeader.biSizeImage);
	if (image == NULL)
	{
		free(image);
		printf("Exception: No enough space!\n");
		return false;
	}
	//���ظ�ʽ����4�ֽڶ���
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//���ճ����ص�����
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, image);

	fp = fopen(file, "w");
	if (fp == NULL)
	{
          printf("Exception: Fail to open file!\n");
		return false;
	}
	fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(image, InfoHeader.biSizeImage, 1, fp);
	free(image);
	fclose(fp);
	return true;
}