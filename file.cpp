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
	//文件句柄
	long   hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
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
/*判断是否为图片文件*/
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
	time_t tt = time(NULL);//这句返回的只是一个时间cuo
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
	byte *image;          //接受图像数据
	FILE *fp;            //文件指针
	BITMAPFILEHEADER FileHeader;    //接受位图文件头
	BITMAPINFOHEADER InfoHeader;    //接受位图信息头

	FileHeader.bfType = BITMAP_ID;                                                  //ID设置为位图的id号
	FileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);      //实际图像数据的位置在文件头和信息头之后
	FileHeader.bfReserved1 = 0;                                                    //必须设置为0
	FileHeader.bfReserved2 = 0;                                                    //必须设置为0
	FileHeader.bfSize = height*width * 24 + FileHeader.bfOffBits;                      //BMP图像文件大小

	InfoHeader.biXPelsPerMeter = 0;                                              //水平分辨率，这里暂时设为0就是
	InfoHeader.biYPelsPerMeter = 0;                                              //垂直分辨率，这里暂时设为0就是
	InfoHeader.biClrUsed = 0;                                                    //图像使用的颜色，这里暂时设为0就是
	InfoHeader.biClrImportant = 0;                                                //重要的颜色数，这里暂时设为0就是                        //垂直分辨率，这里暂时设为0就是
	InfoHeader.biPlanes = 1;                //必须设置为1
	InfoHeader.biCompression = BI_RGB;                                              //设置为BI_RGB时,表示图像并没有彩色表
	InfoHeader.biBitCount = 24;                                                    //图像的位数
	InfoHeader.biSize = sizeof(BITMAPINFOHEADER);                                  //结构体的大小
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
	//像素格式设置4字节对齐
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//接收出像素的数据
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