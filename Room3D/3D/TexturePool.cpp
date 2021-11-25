#include "TexturePool.h"
#include "Global.h"
#include <QString>
#include <QImage>

CTexturePool::CTexturePool()
{
}

CTexturePool::~CTexturePool()
{
	for(long i=0; i < m_vecTexInf.size() ;i++)
	{
		SAFE_DELETE_ARRAY(m_vecTexInf[i].name);

		if(m_vecTexInf[i].id != 0)
		{
			glDeleteTextures(1,&m_vecTexInf[i].id);
		}
#if USE_THREAD
		if(m_vecTexInf[i].pData)
		{
			free(m_vecTexInf[i].pData);
			m_vecTexInf[i].pData = NULL;
		}
#endif
	}
	m_vecTexInf.clear();
}

long CTexturePool::Load(char *filename) 
{
	TextureInfo Tex;
	memset(&Tex, 0x00, sizeof(Tex));

	long id = GetId(filename); 
	if (id >= 0 && id < m_vecTexInf.size()) return id;
	
	Tex.id = m_vecTexInf.size() - 1;
	QImage	qtTexture;
	QString flieName(filename);

	if (!qtTexture.load(filename))
        return -1;

	bool bAlpha = false;

	if(qtTexture.format() == QImage::Format_ARGB32)
 	{
 		bAlpha = true;
 	}
 	else
	{
		qtTexture = qtTexture.convertToFormat(QImage::Format_RGB888);
	}
	
	Tex.name = new char[MAX_PATH_LEN];
	strcpy(Tex.name, filename);

	Tex.bpp    = (qtTexture.depth()/8);
	Tex.width  = qtTexture.width();
	Tex.height = qtTexture.height();
	
#if USE_THREAD
	int nSize = qtTexture.byteCount();
	Tex.pData = (unsigned char*) malloc(nSize);
	memcpy(Tex.pData, qtTexture.bits(), nSize);
#else
	Tex.pData = qtTexture.bits();
#endif
	
	if(bAlpha)
		ConvertARGBtoRGBA(Tex.pData, Tex.width, Tex.height, Tex.bpp);

	FllipVerticale(Tex.pData, Tex.width, Tex.height, Tex.bpp);

#if USE_THREAD
	Tex.id = UINT_MAX;
#else
	glGenTextures(1,&Tex.id);
	glBindTexture(GL_TEXTURE_2D, Tex.id);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

 	if(bAlpha)
 		gluBuild2DMipmaps(GL_TEXTURE_2D, Tex.bpp, Tex.width, Tex.height, GL_RGBA, GL_UNSIGNED_BYTE, Tex.pData);
 	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, Tex.bpp, Tex.width, Tex.height, GL_RGB, GL_UNSIGNED_BYTE, Tex.pData);
#endif

	m_vecTexInf.push_back(Tex);
	return m_vecTexInf.size() - 1;
}

bool CTexturePool::SetTex(long id) 
{	
	if(id < 0)	
		return false;

	glEnable(GL_TEXTURE_2D);
#if USE_THREAD
	if(m_vecTexInf[id].id == UINT_MAX)
	{
		glGenTextures(1,&m_vecTexInf[id].id);
		glBindTexture(GL_TEXTURE_2D, m_vecTexInf[id].id);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if(m_vecTexInf[id].bpp == 4)
			gluBuild2DMipmaps(GL_TEXTURE_2D, m_vecTexInf[id].bpp, m_vecTexInf[id].width, m_vecTexInf[id].height, GL_RGBA, GL_UNSIGNED_BYTE, m_vecTexInf[id].pData);
		else
			gluBuild2DMipmaps(GL_TEXTURE_2D, m_vecTexInf[id].bpp, m_vecTexInf[id].width, m_vecTexInf[id].height, GL_RGB, GL_UNSIGNED_BYTE, m_vecTexInf[id].pData);		
	}
#endif
	glBindTexture(GL_TEXTURE_2D, m_vecTexInf[id].id);
	return true;
}

long CTexturePool::GetId(const char *name) 
{	
	for(long i=0; i < m_vecTexInf.size(); i++)
		if (!strcmp(m_vecTexInf[i].name, name))
			return i;
	return -1;
}

void CTexturePool::ConvertARGBtoRGBA(unsigned char* pData, unsigned int nWidth, unsigned int nHeight, unsigned int nBpp)
{
	// QImage::Format_ARGB32 = BGRA

	unsigned char* pRed;
	unsigned char red;

	if(nBpp != 4)
		return;

	long l = nWidth*nBpp;

	for(long y=0; y<nHeight; y++)
	{
		for(long x=0; x<nWidth; x++)
		{
			red = pData[y*l+x*nBpp + 2];
			pRed = pData + y*l+x*nBpp + 2;
			*pRed = pData[y*l+x*nBpp];
			pData[y*l+x*nBpp] = red;
		}
	}
}

void CTexturePool::FllipVerticale(unsigned char* pData, unsigned int nWidth, unsigned int nHeight, unsigned int nBpp)
{
	unsigned char pix;
	long y = (nHeight >> 1) - 1;
	for(long l = nWidth*nBpp; y >= 0 ;y--) 
	{
		for(long x = 0; x < l ;x++) 
		{
			pix = pData[y*l + x];
			pData[y*l + x] = pData[(nHeight - 1 - y)*l + x];
			pData[(nHeight - 1 - y)*l + x] = pix;
		}
	}
}
