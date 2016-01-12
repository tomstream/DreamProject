#ifndef TGATEXTURE_H  
#define TGATEXTURE_H  

 
#include <iostream>  

using namespace std;

//����ṹ�嶨��  
typedef struct
{
	unsigned char *imageData;//ͼ������  
	unsigned int bpp;//�������  
	unsigned int width;//ͼ����  
	unsigned int height;//ͼ��߶�  
	unsigned int texID;//��Ӧ������ID  
}TextureImage;

//����TGAͼ����������  
bool LoadTGA(TextureImage *texture, const char *fileName);
void LoadBMP(TextureImage *texture, const char *fileName);
#endif  