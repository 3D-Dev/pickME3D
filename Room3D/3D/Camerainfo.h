#pragma once

#include <QWidget>
#include <QFile>

/*!
\class CCameraInfo
\brief Camera Information Class(2012/6/28).
This class stores camera's internal and external parameters, still process some events.
\sa  
*/
class CCameraInfo
{
/// Camera Information Section
public:
	CCameraInfo();
	~CCameraInfo();
	// for read/write function
	bool WriteData(QFile* a_pFile);
	bool ReadData(QFile* a_pFile);
private:

	void SetCameraDimension(int width, int height);
	void GetCameraDimension(int *width, int *height);
	void SetInternalParam(double fx, double fy, double cx, double cy, double s);
	void GetInternalParam(double * fx, double * fy, double * cx, double * cy, double * s);
	void SetDistortionParam(double k1, double k2, double k3, double p1, double p2);
	void GetDistortionParam(double * k1, double * k2, double * k3, double * p1, double * p2);
	void SetExternalParam(double r1, double r2, double r3, double t1, double t2, double t3);
	void GetExternalParam(double * r1, double * r2, double * r3, double * t1, double * t2, double * t3);
	void GetExternalMatrix(double * transform);

	void SetRotationParam( double * mR, bool bRodrigues = false ); // bRodrigues:true --> vector, false --> matrix
	void GetRotationParam( double * mR, bool bRodrigues = false ); // bRodrigues:true --> vector, false --> matrix
	void SetTranslationParam( double * mT );				// World Camera Position. This struct has this.
	void SetTranslationParamOrigin( double * mT );			// Camera Matrix's T. Internally converts.
	void GetTranslationParam( double * mT );
	void GetTranslationParamOrigin( double * mT );
	void SetInternalMatrix(double * A);
	void GetInternalMatrix(double * A);
	void GetDistortionVector( double * k );
	void SetDistortionVector( double * k );

	void GetProjectionMatrix(double * ProjMat);
	void BackInternal(double * src, double * dst);

	void GetProjection(double * pWorld, double * pScreen);
	void GetGLProjection(double * pWorld, double * pScreen);


private:
	double m_fFx, m_fFy, m_fCx, m_fCy, m_fS;
	double m_fK1, m_fK2, m_fK3, m_fP1, m_fP2;
	double m_fR1, m_fR2, m_fR3;
	double m_fT1, m_fT2, m_fT3;
	int m_nWidth, m_nHeight;

/// Single Camera Calibration Section
private:
	void Calc(float * coordPositions, float fScale, int nAxis, int nScaleAxis, bool blScaleCenter = false);
	void getVector(double * screen, double * normalized);
	void getLine(double * point1, double * point2, double * lEquation);
	void getVectors(double * proj, double * vectors, int nAxis);
	void CalcCenter(double * vectors, double * tCoef, double fScale, int nAxis, int nScaleAxis, bool blScaleCenter);
	int getCoordinate(double * vectors, double * tCoef, double * origin, double * rot, double * length, int nAxis);
	void ChangeFOV();

	double m_fFOV;
	float m_CoordinatePositions[6*2];
	double m_arrLength[4];
	int m_nRotationCorrect;

/// Message Event Section
public:
	// GL Aux
	void SetGLMatrix();

	double GetFOV();
	void SetFOV(double fov);
	void SetFOVX(double fov);

	void OnKeyPress(unsigned char key);
	void OnMousePress(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseUp(int x, int y);
	void OnSetCoordPos(float x, float y);
	void OnSetCoordPos_Move(float x, float y);
	void InitCalibration();
	void OnSize(int w, int h);
	void OnDraw();

	void StartSetFloor();
	bool StartSetScale(QWidget * pWidget);
	void SetScaleParam(int nScaleAxis, double fScale);
	bool IsCalibComplete();
	int	 GetStep(){return m_nIndex;}
	int  GetState() {return m_nCalibState;}
	void ResetParameters();
	void OnResize();
	void SetComplete();
	bool GetStageComplete();
	enum Calib_State{CS_NONE, CS_SETFLOOR, CS_SETSCALE};
private:
	int m_nCalibState;
	int m_nIndex;
	bool m_blComplete;
	int m_nHighLightID;
	bool m_blCapture;

	enum Axis_Type {AT_X, AT_Y, AT_Z};
	int m_nAxis;
	int m_nScaleAxis;
	float m_fScale;
	QWidget * m_pParentWidget;
	QFont * m_pFont;//("Courier New", 10, QFont::Bold);
	bool m_blMouseMove;
	QStringList m_arrAxisList;

	int  PickObject(int x, int y);
	bool IsEnable();
};
