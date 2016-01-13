#include "file.h"
#include "io.h"
#define BITMAP_ID 0x4D42
#include <windows.h>
#include <time.h>
#include <cstring>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
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
using namespace cv;
cv::Mat rotate(cv::Mat& src, double angle, Point2f p)
{
	Mat dst;
	Mat r = getRotationMatrix2D(p, angle, 1.0);
	warpAffine(src, dst, r, Size(src.cols, src.rows), CV_INTER_LINEAR, BORDER_REPLICATE);
	return dst;
}

bool SnapScreen(int width, int height, string fileName)
{
	byte*image = (byte *)malloc(width*height*3);
	if (image == NULL)
	{
		free(image);
		printf("Exception: No enough space!\n");
		return false;
	}
	//像素格式设置4字节对齐
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//接收出像素的数据
	glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, image);
	cv::Mat mat(height,width,CV_8UC3,image);
	mat = rotate(mat,180, Point2f(width / 2, height/ 2));
	cv::flip(mat, mat, 1);
	cv::imwrite(fileName,mat);
	free(image);
	return true;
}