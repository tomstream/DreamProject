#ifndef TGATEXTURE_H  
#define TGATEXTURE_H  

 
#include <iostream>  

using namespace std;

//纹理结构体定义  
typedef struct
{
	unsigned char *imageData;//图像数据  
	unsigned int bpp;//像素深度  
	unsigned int width;//图像宽度  
	unsigned int height;//图像高度  
	unsigned int texID;//对应的纹理ID  
}TextureImage;

//加载TGA图像，生成纹理  
bool LoadTGA(TextureImage *texture, const char *fileName);
void LoadBMP(TextureImage *texture, const char *fileName);
#endif  