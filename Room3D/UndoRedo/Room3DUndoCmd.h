//----------------------------------------------//
//	File: Room3DUndoCmd.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QString>
#include "Math3D.h"
#include <Room3DDocument.h>

/*!
	\class CRoom3DUndoCmd
	\brief This is base class to  all of the undo commands.(by 001, 2012/6/23).
	\sa 
*/
class CRoom3DUndoCmd
{
public:
	CRoom3DUndoCmd(void);
	virtual ~CRoom3DUndoCmd(void);
public:
	virtual bool IsUndoable(){return true;}
	virtual bool Undo() { m_bUndo = true;  return true; }
	virtual bool Redo() { m_bUndo = false; return true; }
protected:
	bool m_bUndo;
	CRoom3DDocument* m_pDocument;
};

class CRoom3DLoadBackgroundImgCmd : public CRoom3DUndoCmd
{
public:
	CRoom3DLoadBackgroundImgCmd(){};
	~CRoom3DLoadBackgroundImgCmd(){};
	virtual bool IsUndoable(){return false;}
	virtual bool Undo();
	virtual bool Redo();
public:
	QString m_sBackgroundImgPath;
	QString m_sPrevBackgroundImgPath;
};

class CRoom3DCalibSetCoordinateCmd : public CRoom3DUndoCmd
{
public:
	CRoom3DCalibSetCoordinateCmd(){};
	~CRoom3DCalibSetCoordinateCmd(){};
	virtual bool Undo();
	virtual bool Redo();
public:
	float m_fCoordinatePosition[4*2];
};

class CRoom3DFurnitureAddCmd : public CRoom3DUndoCmd
{
public:
	CRoom3DFurnitureAddCmd()
	{ 
		m_pFurniture = NULL; 
	}
	~CRoom3DFurnitureAddCmd()
	{
		if(m_bUndo)
			SAFE_DELETE(m_pFurniture);
	}
	virtual bool Undo();
	virtual bool Redo();
public:
	void* m_pFurniture;
};

class CRoom3DFurnitureDelCmd : public CRoom3DUndoCmd
{
public:
	CRoom3DFurnitureDelCmd()
	{ 
		m_pFurniture = NULL; 
	}
	
	~CRoom3DFurnitureDelCmd()
	{
		if(!m_bUndo)
			SAFE_DELETE(m_pFurniture);
	}

	virtual bool Undo();
	virtual bool Redo();
public:
	void* m_pFurniture;
};

class CRoom3DFurnitureRotateCmd : public CRoom3DUndoCmd
{
public:
	CRoom3DFurnitureRotateCmd(){};
	~CRoom3DFurnitureRotateCmd(){};
	virtual bool Undo();
	virtual bool Redo();
public:
	CFurniture* m_pFurniture;
	float m_fCurRotY;
	float m_fPrevRotY;

};

class CRoom3DFurnitureMoveCmd : public CRoom3DUndoCmd
{
public:
	CRoom3DFurnitureMoveCmd(){};
	~CRoom3DFurnitureMoveCmd(){};
	virtual bool Undo();
	virtual bool Redo();
public:
	CFurniture* m_pFurniture;
	vec3 m_vCurPos;
	vec3 m_vPrevPos;
};

class CRoom3DBkImgBrightCmd : public CRoom3DUndoCmd
{
public:
	CRoom3DBkImgBrightCmd(){};
	~CRoom3DBkImgBrightCmd(){};
	virtual bool Undo();
	virtual bool Redo();
public:
	int m_nPrevBright;
	int m_nPrevContrast;
	int m_nBright;
	int m_nContrast;
};
