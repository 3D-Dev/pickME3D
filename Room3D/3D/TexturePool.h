#ifndef _TexturePool_H
#define _TexturePool_H

#include <windows.h>
#include <algorithm>
#include <vector>
#include <string.h>
#include <gl/gl.h>
#include <gl/glu.h>


struct TextureInfo 
{
	char * name;

	unsigned int id;
	unsigned int width;
	unsigned int height;
	int bpp;
	unsigned char* pData;
};

class CTexturePool 
{
public:
	CTexturePool();
	~CTexturePool();
	long Load(char *filename);	
	bool SetTex(long id);
	long GetId(const char *name);
	void ConvertARGBtoRGBA(unsigned char* pData, unsigned int nWidth, unsigned int nHeight, unsigned int nBpp);
	void FllipVerticale(unsigned char* pData, unsigned int nWidth, unsigned int nHeight, unsigned int nBpp);
private:
	std::vector<TextureInfo> m_vecTexInf;
};



#endif  // _TexturePool_H