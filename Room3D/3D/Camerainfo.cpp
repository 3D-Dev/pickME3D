// datacamerainfo.cpp : Defines camera information data
//
#include "Camerainfo.h"
#include "MathLib.h"
#include "Projection.h"
#include <math.h>

#include "Global.h"

#include <qgl.h>

#include "Common.h"
#include "Room3DApplication.h"


CCameraInfo::CCameraInfo()
{
	SetInternalParam(0.,0.,0.,0., 0.);
	SetDistortionParam(0.,0.,0.,0.,0.);
	SetExternalParam(0.,0.,0.,0.,0.,0.);

	m_nWidth = 700;
	m_nHeight = 508;

	m_fFOV = CAMERA_FOV;

	ResetParameters();

	m_pFont = new QFont("Arial Unicode MS", 10, QFont::Bold);
	m_pFont->setLetterSpacing(QFont::PercentageSpacing, 180);
	m_blMouseMove = false;

	m_arrAxisList << "X" << "Y" << "Z";
}

CCameraInfo::~CCameraInfo()
{
	if(m_pFont != NULL)
		delete m_pFont;
}

void CCameraInfo::SetCameraDimension(int width, int height)
{
	this->m_nWidth = width;
	this->m_nHeight = height;
}
void CCameraInfo::GetCameraDimension(int *width, int *height)
{
	*width = this->m_nWidth;
	*height = this->m_nHeight;
}

void CCameraInfo::SetInternalParam(double _fx, double _fy, double _cx, double _cy, double _s)
{
	m_fFx = _fx;
	m_fFy = _fy;
	m_fCx = _cx;
	m_fCy = _cy;
	m_fS = _s;
}

void CCameraInfo::GetInternalParam(double * _fx, double * _fy, double * _cx, double * _cy, double * _s)
{
	*_fx = m_fFx;
	*_fy = m_fFy;
	*_cx = m_fCx;
	*_cy = m_fCy;
	*_s = m_fS;
}

void CCameraInfo::SetDistortionParam(double _k1, double _k2, double _k3, double _p1, double _p2)
{
	m_fK1 = _k1;
	m_fK2 = _k2;
	m_fK3 = _k3;
	m_fP1 = _p1;
	m_fP2 = _p2;
}

void CCameraInfo::GetDistortionParam(double * _k1, double * _k2, double * _k3, double * _p1, double * _p2)
{
	*_k1 = m_fK1;
	*_k2 = m_fK2;
	*_k3 = m_fK3;
	*_p1 = m_fP1;
	*_p2 = m_fP2;
}

void CCameraInfo::SetExternalParam(double _r1, double _r2, double _r3, double _t1, double _t2, double _t3)
{
	m_fR1 = _r1;
	m_fR2 = _r2;
	m_fR3 = _r3;
	m_fT1 = _t1;
	m_fT2 = _t2;
	m_fT3 = _t3;
}

void CCameraInfo::GetExternalParam(double * _r1, double * _r2, double * _r3, double * _t1, double * _t2, double * _t3)
{
	*_r1 = m_fR1;
	*_r2 = m_fR2;
	*_r3 = m_fR3;
	*_t1 = m_fT1;
	*_t2 = m_fT2;
	*_t3 = m_fT3;
}
void CCameraInfo::GetExternalMatrix(double * transform)
{
	double rot[9], trans[3];
	GetRotationParam(rot);
	GetTranslationParamOrigin(trans);
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			transform[i*4+j] = rot[i*3+j];
		}
		transform[i*4+3] = trans[i];
	}
	//
}

void CCameraInfo::SetRotationParam( double * mR, bool bRodrigues )
{
	if(bRodrigues)
	{
		m_fR1 = mR[0];
		m_fR2 = mR[1];
		m_fR3 = mR[2];
	}
	else
	{
		double r[3] = {0,};
		Rodrigues2(mR,r, false);
		m_fR1 = r[0];
		m_fR2 = r[1];
		m_fR3 = r[2];
	}
}
void CCameraInfo::GetRotationParam( double * mR, bool bRodrigues )
{
	if(bRodrigues)
	{
		mR[0] = m_fR1;
		mR[1] = m_fR2;
		mR[2] = m_fR3;
	}
	else
	{
		double r[3] = {m_fR1,m_fR2,m_fR3};
		Rodrigues2(r,mR, true);
	}
}
void CCameraInfo::SetTranslationParam( double * mT )
{
	m_fT1 = mT[0];
	m_fT2 = mT[1];
	m_fT3 = mT[2];
}
void CCameraInfo::SetTranslationParamOrigin( double * mT )
{
	// mT = -RT
	// T = -R'mT
	double Rot[9] = {0,};
	GetRotationParam(Rot,false);
	double t[3] = {0,};
	GEMM(3,3,1,Rot,mT,-1,0,0,t,MATOP_GEMM_A_T);
	SetTranslationParam(t);
}

void CCameraInfo::GetTranslationParam( double * mT )
{
	mT[0] = m_fT1;
	mT[1] = m_fT2;
	mT[2] = m_fT3;
}

void CCameraInfo::GetTranslationParamOrigin( double * mT )
{
	// mT = -RT
	double Rot[9];
	GetRotationParam(Rot,false);
	double t[3];
	t[0] = m_fT1;
	t[1] = m_fT2;
	t[2] = m_fT3;
	GEMM(3,3,1,Rot,t,-1,0,0,mT,0);
}

//		[fx		s		cx	]
// A =	[		fy		cy	]
//		[				1	]
void CCameraInfo::SetInternalMatrix(double * A)
{
	m_fFx = A[0];
	m_fS  = A[1];
	m_fCx = A[2];

	m_fFy = A[4];
	m_fCy = A[5];
}

void CCameraInfo::GetInternalMatrix(double * A)
{
	A[0] = m_fFx;
	A[1] = m_fS;
	A[2] = m_fCx;

	A[3] = 0;
	A[4] = m_fFy;
	A[5] = m_fCy;

	A[6] = A[7] = 0;
	A[8] = 1;
}

void CCameraInfo::GetDistortionVector( double * k )
{
	k[0] = m_fK1;
	k[1] = m_fK2;
	k[2] = m_fK3;
	k[3] = m_fP1;
	k[4] = m_fP2;
}
void CCameraInfo::SetDistortionVector( double * k )
{
	m_fK1 = k[0];
	m_fK2 = k[1];
	m_fK3 = k[2];
	m_fP1 = k[3];
	m_fP2 = k[4];
}

void CCameraInfo::GetProjectionMatrix(double * ProjMat)
{
	// P =	KR[I|-C]
	double A[9];
	double R[9];
	double C[3];
	GetInternalMatrix(A);
	GetRotationParam(R, false);
	double KR[9], KRC[9];
	GEMM(3, 3, 3, A, R, 1, 0, 0, KR, 0);
	GetTranslationParam(C);
	GEMM(3, 3, 1, KR, C, -1, 0, 0, KRC, 0);

	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
			ProjMat[i*4+j] = KR[i*3+j];
		ProjMat[i*4+3] = KRC[i];
	}
}

void CCameraInfo::BackInternal(double * src, double * dst)
{
	// fx s cx
	// 0 fy cy
	// 0  0  1
	dst[1] = (src[1] - m_fCy) / m_fFy;
	dst[0] = (src[0] - m_fCx - m_fS * dst[1]) / m_fFx;
}
void CCameraInfo::GetProjection(double * pWorld, double * pScreen)
{
	/*double Proj[12];
	GetProjectionMatrix(Proj);
	double world[4];
	double screen[3];
	for( int i = 0; i < 3; i++ )
		world[i] = pWorld[i];
	world[3] = 1.0f;
	GEMM(3, 4, 1, Proj, world, 1.0f, NULL, 0, screen, 0);
	pScreen[0] = screen[0] / screen[2];
	pScreen[1] = screen[1] / screen[2];*/
	double A[9], r[3], t[3], k[5] = {0};
	GetInternalMatrix(A);
	GetRotationParam(r, true);
	GetTranslationParamOrigin(t);
	Project_Metric(A, r, t, k, pWorld, pScreen);
}
void CCameraInfo::GetGLProjection(double * pWorld, double * pScreen)
{
	double projection[16];
	double modelview[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	double screen[3];
	gluProject(pWorld[0], pWorld[1], pWorld[2], modelview, projection, viewport, &screen[0], &screen[1], &screen[2]);

	pScreen[0] = screen[0];
	pScreen[1] = viewport[3] - screen[1];

	
}

void CCameraInfo::SetGLMatrix()
{
	if(!IsCalibComplete() && m_nIndex != 5)
		return;

	double transform[3];
	GetTranslationParam(transform);
	double Rot[9];
	GetRotationParam(Rot);
	double RotTransform[16] = {0};
	RotTransform[15] = 1.0f;
	for( int i = 0; i < 3; i++ )
	{
		Rot[1*3+i] *= -1;
		Rot[2*3+i] *= -1;
	}
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 3; j++ )
			RotTransform[i*4+j] = Rot[j*3+i];
	glMultMatrixd(RotTransform);
	glTranslatef(-transform[0], -transform[1], -transform[2]);
}

void CCameraInfo::SetFOV(double fov)
{
	m_fFOV = fov;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(GetFOV(), 1.0*m_nWidth / m_nHeight, CAMERA_NEAR, CAMERA_FAR);
}
void CCameraInfo::SetFOVX(double fov)
{
	if(fov == 0)
		SetFOV(CAMERA_FOV);
	else
	{
		double radian = A2R(fov / 2);
		double tradian = 1.0 * tan(radian);
		double rHeight = tradian * m_nHeight / m_nWidth;
		SetFOV(R2A(atan(rHeight)) * 2);
	}
}

double CCameraInfo::GetFOV()
{
	return m_fFOV;
}

// Single Camera Calibration

void CCameraInfo::getVector(double * screen, double * normalized)
{
	BackInternal(screen, normalized);
	normalized[2] = 1.0f;
	Normalize(3, normalized);
}

void CCameraInfo::getLine(double * point1, double * point2, double * lEquation)
{
	double x1 = point1[0];
	double y1 = point1[1];

	double x2 = point2[0];
	double y2 = point2[1];

	double X = x2 - x1;
	double Y = y2 - y1;
	if(X == 0) X = 0.000001;
	if(Y == 0) Y = 0.000001;

	lEquation[0] = 1/X;
	lEquation[1] = -1/Y;
	lEquation[2] = -x1/X+y1/Y;
}

void CCameraInfo::getVectors(double * proj, double * vectors, int nAxis)
{
	// get origin, x, y, z vectors
	getVector(&proj[0*2], &vectors[0*3]); //vo
	getVector(&proj[1*2], &vectors[1*3]); //vx
	getVector(&proj[2*2], &vectors[2*3]); //vy
	getVector(&proj[3*2], &vectors[3*3]); //vz
	getVector(&proj[4*2], &vectors[5*3]); //vx_1
	getVector(&proj[5*2], &vectors[6*3]); //vx_2

	// get line with o-x
	double lEqua1[3] = {0,}, lEqua2[3] = {0,};
	//	X1 = getLine(Proj(1,:),Proj(2,:));
	getLine(&proj[0*2], &proj[(nAxis+1)*2], lEqua1);
	// get line with x1-x2
	//	X2 = getLine(Proj(6,:),Proj(5,:));
	getLine(&proj[5*2], &proj[4*2], lEqua2);

	// get intersection point of o-x and x1-x2
	//	XA = [X1(1) X1(2); X2(1) X2(2)];
	//Xb = -[X1(3); X2(3)];
	//iX = XA\Xb;
	double XA[4] = {lEqua1[0], lEqua1[1], lEqua2[0], lEqua2[1]};
	double Xb[2] = {-lEqua1[2], -lEqua2[2]};
	double XAinv[4] = {0,};
	Pinv(2, 2, XA, XAinv);
	double intersect_point[2] = {0,};
	GEMM(2, 2, 1, XAinv, Xb, 1, NULL, 0, intersect_point, 0);

	//vx_ = get_vector([iX; 1], false)';
	getVector(intersect_point, &vectors[4*3]); //vz
}


void CCameraInfo::CalcCenter(double * vectors, double * tCoef, double fScale, int nAxis, int nScaleAxis, bool blScaleCenter)
{
	//%% Equations
	//% Coordinate center and Axis Points
	//% O = C + t0*v0             (o1)
	//% X = C + tx*vx             (o2)
	//% Y = C + ty*vy             (o3)
	//% Z = C + tz*vz             (o4)

	//% Equation
	//% (Y-O)X = 0                (1)
	//% (Z-O)X = 0                (2)
	//% (Y-O)(Z-O) = 0            (3)

	//% (o3)  ==>       Y-O = vy*ty - to*vo
	//% (1)   ==>       (Y-O)*X = YX - OX = ty*vy*X - to*vo*X = 0
	//%       ==>       ty = to*(vo*X)/(vy*X)=ay*to         (r1)
	//% (2)   ==>       tz = to*(vo*X)/(vz*X)=az*to         (r2)
	//% (r1,r2->3) ==>    (vy*ty-to*vo)*(vz*tz-to*vo)
	//%            ==>    ty*tz*vy*vz - ty*to*vy*vo - tz*to*vz*vo + to*to*vo*vo
	//%            ==>    to^2*ay*az*vy*vz - to^2*ay*vy*vo - to^2*az*vz*vo + to^2*vo*vo = 0
	//%            ==>    to^2*(ay*az*vy*vz - ay*vy*vo - az*vz*vo + vo*vo) = 0
	//%            ==>    to^2=0 || (ay*az*vy*vz - ay*vy*vo - az*vz*vo + vo*vo) = 0

	//
	double * vo = &vectors[0*3+0];
	double * vx = &vectors[1*3+0];
	double * vy = &vectors[2*3+0];
	double * vz = &vectors[3*3+0];
	double * X =  &vectors[4*3+0];
	double * vx_1 =  &vectors[5*3+0];
	double * vx_2 =  &vectors[6*3+0];

	//% ay = (vo*X)/(vy*X)
	//% az = (vo*X)/(vz*X)
	double arr_A[3];
	double *ax, *ay, *az;
	ax = &arr_A[0];
	ay = &arr_A[1];
	az = &arr_A[2];
	
	for( int i = 0; i < 3; i++ )
	{
		if( i != nAxis )
			arr_A[i] = DotProduct(vo,X) / DotProduct(&vectors[(i+1)*3+0], X);
	}

	//% to = 1
	//% ty = ay*to
	//% tz = az*to
	double arr_T[3] = {0,};
	double *tx, *ty, *tz;
	tx = &arr_T[0];
	ty = &arr_T[1];
	tz = &arr_T[2];
	double to = 1;
	for( int i = 0; i < 3; i++ )
	{
		if( i != nAxis )
			arr_T[i] = arr_A[i] * to;
	}

	//o = to*vo;
	//y = ty*vy;
	//z = tz*vz;
	//Y = y-o;
	double o[3] = {0,}, y[3] = {0,}, z[3] = {0,}, Y[3] = {0,}, x[3] = {0,}, x_1[3] = {0,}, x_2[3] = {0,};
	double arr_P[3*3] = {0,};
	for( int i = 0; i < 3; i++ )
	{
		o[i] = to*vo[i];
		for( int j = 0; j < 3; j++ )
		{
			if( j != nAxis)
			{
				arr_P[j*3+i] = arr_T[j] * vectors[(j+1)*3 + i];
				Y[i] = arr_P[j*3+i] - o[i];
			}
		}
	}
	Normalize(3, Y);

	//% tx = to*(vo*Y)/(vx*Y)
	arr_A[nAxis] = DotProduct(vo, Y) / DotProduct(&vectors[(nAxis+1)*3], Y);
	arr_T[nAxis] = to * arr_A[nAxis];
	double ax_1 = DotProduct(vo,Y)/DotProduct(vx_1,Y);
	double ax_2 = DotProduct(vo,Y)/DotProduct(vx_2,Y);
	double tx_1 = to * ax_1;
	double tx_2 = to * ax_2;

	for( int i = 0; i < 3; i++ )
	{
		arr_P[nAxis*3+i] = arr_T[nAxis]*vectors[(nAxis+1)*3+i];
		//x_1[i] = tx_1*vx_1[i];
		//x_2[i] = tx_2*vx_2[i];
		x_1[i] = (arr_T[nScaleAxis])*vectors[(nScaleAxis+1)*3+i];
		x_2[i] = to*vo[i];
	}

	double fXScale = Norm(3, x_1, x_2);
	if(blScaleCenter)
		fXScale = 1.0f;
	else
	{
		int a = 1;
	}
	to = to * fScale / fXScale;
	for( int i = 0; i < 3; i++ )
		arr_T[i] *= (fScale / fXScale);

	//double discrimination = 0.0f;
	//discrimination = ay*az*dot(vy,vz) - ay*dot(vy,vo) - az*dot(vz,vo) + dot(vo,vo);
	double discrimination = (*ay)*(*az)*DotProduct(vy,vz) - (*ay)*DotProduct(vy,vo) - (*az)*DotProduct(vz,vo) + DotProduct(vo,vo);
	

	tCoef[0] = to;
	for( int i = 0; i < 3; i++ )
		tCoef[i+1] = arr_T[i];
	tCoef[4] = discrimination;
	printf("Discrimination : %f\n", discrimination);
}

int CCameraInfo::getCoordinate(double * vectors, double * tCoef, double * origin, double * rot, double * length, int nAxis)
{
	//
	double to, tx, ty, tz;
	to = tCoef[0];
	tx = tCoef[1];
	ty = tCoef[2];
	tz = tCoef[3];

	double * vo = &vectors[0*3+0];
	double * vx = &vectors[1*3+0];
	double * vy = &vectors[2*3+0];
	double * vz = &vectors[3*3+0];

	//    O = to*vo;
	//    X = tx*vx;
	//    Y = ty*vy;
	//    Z = tz*vz;
	double O[3] = {0,}, X[3] = {0,}, Y[3] = {0,}, Z[3] = {0,};
	for( int i = 0; i < 3; i++ )
	{
		O[i] = to*vo[i];
		X[i] = tx*vx[i];
		Y[i] = ty*vy[i];
		Z[i] = tz*vz[i];
	}

	//    OX = X - O;
	//    OY = Y - O;
	//    OZ = Z - O;
	//    OX = OX / norm(OX);
	//    OY = OY / norm(OY);
	//    OZ = OZ / norm(OZ);
	double OX[3] = {0,}, OY[3] = {0,}, OZ[3] = {0,};
	for( int i = 0; i < 3; i++ )
	{
		OX[i] = X[i] - O[i];
		OY[i] = Y[i] - O[i];
		OZ[i] = Z[i] - O[i];
	}
	length[0] = Norm(3, OX);
	length[1] = Norm(3, OY);
	length[2] = Norm(3, OZ);
	length[3] = to;
	Normalize(3, OX);
	Normalize(3, OY);
	Normalize(3, OZ);

	if(nAxis == 0 || nAxis == 1)
	{
		CrossProduct(OX, OY, OZ);
	}
	else if(nAxis == 2)
	{
		CrossProduct(OY, OZ, OX);
	}

	//    Rot = [OX' OY' OZ'];
	//    Rot = CorrectRotationMatrix(Rot);
	double Rot[9] = {0,};
	for( int i = 0; i < 3; i++ )
	{
		Rot[i*3+0] = OX[i];
		Rot[i*3+1] = OY[i];
		Rot[i*3+2] = OZ[i];
	}
 	int nCorrect = CorrectRotationMatrix(Rot);
	if(nCorrect != 0)
	{
		/*double Rot90[9] = {1, 0, 0, 0, 0, -1, 0, 1, 0};
		double RotResult[9];
		GEMM(3, 3, 3, Rot, Rot90, 1.0, NULL, 0, RotResult, 0);
		COPY(RotResult, Rot, double, 9);*/
	}

	for( int i = 0; i < 3; i++ )
		origin[i] = O[i];
	for( int i = 0; i < 9; i++ )
		rot[i] = Rot[i];

	return nCorrect;
}

void CCameraInfo::Calc(float * coordPositions, float fScale, int nAxis, int nScaleAxis, bool blScaleCenter)
{
	//(height/2) / focal = tan(fovy/2)
	// focal = (height / 2) / tan(fovy/2)
	double focal = (m_nHeight / 2) / tan(float(A2R(m_fFOV)/2));
	SetInternalParam(focal, focal, (m_nWidth-1.)/2, (m_nHeight-1.)/2, 0.);

	SetDistortionParam(0,0,0,0,0);

	if(blScaleCenter == false)
	{
		int a = 1;
	}

	double vectors[7*3] = {0,};
	double projection[6*2] = {0,};
	for( int i = 0; i < 6; i++ )
	{
		for( int j = 0; j < 2; j++ )
			projection[i*2+j] = coordPositions[i*2+j];
	}
	//if(nAxis == 1 || nAxis == 2)
	//{
	//	for( int i = 0; i < nAxis; i++)
	//	for( int j = 0; j < 2; j++ )
	//	{
	//		double temp = projection[1*2+j];
	//		projection[1*2+j] = projection[2*2+j];
	//		projection[2*2+j] = projection[3*2+j];
	//		projection[3*2+j] = temp;
	//	}
	//}

	getVectors(projection, vectors, nAxis);

	double tCoef[5] = {0};
	CalcCenter(vectors, tCoef, fScale, nAxis, nScaleAxis, blScaleCenter);
	
	bool blValid = true;
	//float fInvalidScale = 4.0f;
	for( int i = 1; i < 4; i++ )
	{
		if(tCoef[i] < 0)
			blValid = false;
		//if(tCoef[i] > fInvalidScale )
		//	blValid = false;
	}
	//if(!blValid)
	//	return;
	//if(nAxis == 1 || nAxis == 2)
	//{
	//	for( int i = 0; i < nAxis; i++)
	//	{
	//		for( int j = 0; j < 3; j++ )
	//		{
	//			double temp = vectors[3*3+j];
	//			vectors[3*3+j] = vectors[2*3+j];
	//			vectors[2*3+j] = vectors[1*3+j];
	//			vectors[1*3+j] = temp;
	//		}
	//		double temp = tCoef[3];
	//		tCoef[3] = tCoef[2];
	//		tCoef[2] = tCoef[1];
	//		tCoef[1] = temp;
	//	}
	//}

	double Origin[3] = {0}, Rotation[9] = {0};
	m_nRotationCorrect = getCoordinate(vectors, tCoef, Origin, Rotation, m_arrLength, nAxis);

	SetRotationParam(Rotation);
	SetTranslationParamOrigin(Origin);
}

void CCameraInfo::OnKeyPress(unsigned char key)
{
	switch(key)
	{
	case 'r':
		m_fFOV += 1.0f;
		ChangeFOV();
		break;
	case 'f':
		m_fFOV -= 1.0f;
		ChangeFOV();
		break;
	case ' ':
		ResetParameters();
		break;
	}

	// Refresh the Window
	//glutPostRedisplay();
}

int CCameraInfo::PickObject(int x, int y)
{
	for( int i = 0; i <= 3; i++ )
	{
		int sqrDist = SQR(m_CoordinatePositions[i*2+0] - x) + SQR(m_CoordinatePositions[i*2+1] - y);
		if(sqrDist < SQR(5))
		{
			return i;
		}
	}
	return -1;
}
void CCameraInfo::OnMousePress(int x, int y)
{
	// if CS_SETFLOOR
		// pick object
		// capture
	// if CS_SETSCALE
		// if first (4)
		// show axis dialog
		// if second (5)
		// show scale dialog
	if(m_nCalibState == CS_SETFLOOR)
	{
		int nObjectID = PickObject(x, y);
		if(nObjectID != -1)
		{
			m_nIndex = nObjectID;
			m_blCapture = true;
		}
	}
	else if(m_nCalibState == CS_SETSCALE)
	{
		if(m_nIndex == 4)
		{
			OnSetCoordPos(x, y);
		}
		else if(m_nIndex == 5)
		{
			OnSetCoordPos(x, y);
		}
	}
}

void CCameraInfo::OnMouseMove(int x, int y)
{
	// if CS_SETFLOOR
		// if capture
		// move
	// if CS_SETSCALE
		// move
	bool blMove = false;
	if(m_nCalibState == CS_SETFLOOR)
	{
		m_nHighLightID = PickObject(x, y);
		if(m_blCapture)
		{
			OnSetCoordPos_Move(x, y);
			blMove = true;
		}
		if(m_nHighLightID != -1)
			blMove = true;
	}
	else if(m_nCalibState == CS_SETSCALE)
	{
		OnSetCoordPos_Move(x, y);
		blMove = true;
	}
	if(m_blMouseMove != blMove)
	{
		m_blMouseMove = blMove;

		QGLWidget * pWidget = g_pApp->GetSceneView();
		if(m_blMouseMove)
		{
			pWidget->setCursor(Qt::PointingHandCursor);
		}
		else
		{
			pWidget->setCursor(Qt::ArrowCursor);
		}
	}
}
void CCameraInfo::OnMouseUp(int x, int y)
{
	// if CS_SETFLOOR
		// if capture
		// release
	//OnSetCoordPos(x, y);
	if(m_nCalibState == CS_SETFLOOR)
	{
		if(m_blCapture)
		{
			m_blCapture = false;
		}
	}
}
#define GET_GL_X(x) (x-m_nWidth/2)
#define GET_GL_Y(y) (m_nHeight/2-y)
void CCameraInfo::OnSetCoordPos(float x, float y)
{
	if(!IsEnable())
		return;
	if(IsCalibComplete())
		return;
	m_CoordinatePositions[m_nIndex*2] = x;
	m_CoordinatePositions[m_nIndex*2+1] = y;
	m_nIndex++;
	if(m_nIndex != 6)
	{
		m_CoordinatePositions[m_nIndex*2] = x;
		m_CoordinatePositions[m_nIndex*2+1] = y;
	}
	if(m_nIndex == 6)
	{
		Calc(m_CoordinatePositions, m_fScale, m_nAxis, m_nScaleAxis);
		m_nCalibState = CS_NONE;
		;//g_pApp->GetDocument()->UpdateDoc(APP_MSG_CALIB_SETHELPERAXIS, (long)&g_CoordinatePositions[4*2]);
	}
}
void CCameraInfo::OnSetCoordPos_Move(float x, float y)
{
	if(!IsEnable())
		return;
	if(IsCalibComplete())
		return;
	if(m_nIndex == 0)
	{
		float cx = x - m_CoordinatePositions[m_nIndex*2];
		float cy = y - m_CoordinatePositions[m_nIndex*2 + 1];
		for( int i = 1; i <= 3; i++ )
		{
			m_CoordinatePositions[i*2] += cx;
			m_CoordinatePositions[i*2+1] += cy;
		}
	}
	m_CoordinatePositions[m_nIndex*2] = x;
	m_CoordinatePositions[m_nIndex*2+1] = y;
	if(m_nIndex == 5)
	{
		Calc(m_CoordinatePositions, CALIBRATION_SCALE, m_nAxis, m_nScaleAxis, true);
	}
}
void CCameraInfo::InitCalibration()
{
	m_nIndex = 0;
}
void CCameraInfo::OnSize(int w, int h)
{
	m_nWidth = w;
	m_nHeight = h;
	OnResize();
}
// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
{
	float length;

	// Calculate the length of the vector       
	length = (float)sqrt((vector[0]*vector[0]) + 
		(vector[1]*vector[1]) +
		(vector[2]*vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}


// Points p1, p2, & p3 specified in counter clock-wise order
#define DRAW_TEXT 1
void calcNormal(float v[3][3], float out[3])
{
	float v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}
void DrawBox(float * center, float width, float height, float length, bool * blCorrect)
{
	if(blCorrect[0])
		glTranslatef(-width, 0.0f, 0.0f);
	if(blCorrect[1])
		glTranslatef(0.0f, -height, 0.0f);
	if(blCorrect[2])
		glTranslatef(0.0f, 0.0f, -length);

	width /= 2;
	height /= 2;
	length /= 2;

	center[0] += width;
	center[1] += height;
	center[2] += length;

	// Array containing the six vertices of the cube
	GLfloat corners[] = { center[0] - width, center[1] + height, center[2] + length, // 0 // Front of cube
		center[0] + width, center[1] + height, center[2] + length, // 1
		center[0] + width, center[1] - height, center[2] + length,// 2
		center[0] - width, center[1] - height, center[2] + length,// 3
		center[0] - width, center[1] + height, center[2] - length,// 4  // Back of cube
		center[0] + width, center[1] + height, center[2] - length,// 5
		center[0] + width, center[1] - height, center[2] - length,// 6
		center[0] - width, center[1] - height, center[2] - length };// 7

	// Array of indexes to create the cube
	GLubyte indexes[] = { 0, 1, 2, 3,	// Front Face
		4, 5, 1, 0,	// Top Face
		3, 2, 6, 7,	// Bottom Face
		5, 4, 7, 6,	// Back Face
		1, 5, 6, 2,	// Right Face
		4, 0, 3, 7 };	// Left Face

	// Make the cube a wire frame
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPointSize(1.0f);

	float fBright = 0.1f;
	glColor4f(fBright, fBright, 0.8, 0.5f);
	glPolygonMode(GL_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, GL_POINT);

	// Enable and specify the vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, corners);
	// Using Drawarrays
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indexes);

	glColor4f(fBright, fBright, 0.8, 0.2f);
	glPolygonMode(GL_BACK, GL_POINT);
	glPolygonMode(GL_FRONT, GL_LINE);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indexes);

	glColor4f(fBright, fBright, 0.8, 0.2f);
	glPolygonMode(GL_BACK, GL_POINT);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indexes);
}
void CCameraInfo::OnDraw()
{
	GLfloat coordinateColor[] = { 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f};

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	QGLWidget * pWidget = g_pApp->GetSceneView();

	glPushMatrix();
	{
		glLoadIdentity();
		float distance = ( m_nHeight / 2 ) / tan(A2R(m_fFOV)/2);
		glTranslatef(0.0f, 0.0f, -distance);

		glEnable(GL_LINE_STIPPLE);
		glLineStipple(1, 0x0F0F);	
		glLineWidth(3.0f);

		glEnable(GL_POINT_SMOOTH);
		// CS_NONE --> Draw Nothing
		// CS_SETFLOOR --> Draw Axis
		// CS_SETSCALE --> Draw Axis, Helper Axis
		int nDrawTo = -1;
		if(m_nCalibState == CS_SETFLOOR)
			nDrawTo = 3;
		else if(m_nCalibState == CS_SETSCALE)
			nDrawTo = m_nIndex;
		if(IsCalibComplete())
			nDrawTo = 5;

		for( int i = 0; i <= nDrawTo; i++ )
		{
			int nColorID = i;
			if(i == 4 || i == 5)
				nColorID = m_nAxis+1;
			glColor4fv(&coordinateColor[nColorID*4]);
			// center and axis
			if( i > 0 && i < 4)
			{
				//glColor4fv(&coordinateColor[i*4]);
				glBegin(GL_LINES);
				glVertex3f(GET_GL_X(m_CoordinatePositions[0*2+0]), GET_GL_Y(m_CoordinatePositions[0*2+1]), 0.0f);
				glVertex3f(GET_GL_X(m_CoordinatePositions[i*2+0]), GET_GL_Y(m_CoordinatePositions[i*2+1]), 0.0f);
				glEnd();
			}
			// helper axis
			if(i == 5)
			{
				//glColor4fv(&coordinateColor[(m_nAxis+1)*4]);
				glBegin(GL_LINES);
				glVertex3f(GET_GL_X(m_CoordinatePositions[4*2+0]), GET_GL_Y(m_CoordinatePositions[4*2+1]), 0.0f);
				glVertex3f(GET_GL_X(m_CoordinatePositions[5*2+0]), GET_GL_Y(m_CoordinatePositions[5*2+1]), 0.0f);
				glEnd();
			}
		}

		for( int i = 0; i <= nDrawTo; i++ )
		{
			// sphere
			int nColorID = i;
			if(i == 4 || i == 5)
				nColorID = m_nAxis+1;
			glColor4fv(&coordinateColor[nColorID*4]);

			glPointSize(8.0f);
			if(i == m_nHighLightID)
				glPointSize(12.0f);
			glBegin(GL_POINTS);
			glVertex3f(GET_GL_X(m_CoordinatePositions[i*2+0]), GET_GL_Y(m_CoordinatePositions[i*2+1]), 0.0f);
			glEnd();

			if( i >= 1 && i <= 3)
			{
#if DRAW_TEXT
				pWidget->renderText(m_CoordinatePositions[i*2+0], m_CoordinatePositions[i*2+1] - 10, m_arrAxisList[i-1], *m_pFont, 2000 );
#endif
			}
		}
		glDisable(GL_LINE_STIPPLE);
	}
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	glLoadIdentity();
	bool Inverses[3];
	//SetGLMatrix();
	//float distance = ( m_nHeight / 2 ) / tan(A2R(m_fFOV)/2);
	//glTranslatef(0.0f, 0.0f, -distance);
	//glLoadIdentity();
	SetGLMatrix();
	if(IsCalibComplete() || m_nIndex == 5)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glLineWidth(1.0f);
		float fBright = 0.3f;
		glColor4f(fBright, fBright, fBright, 0.5f);
		float axis[3*3] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

		double coords[4 * 3];
		for( int i = 0; i < 3; i++ )
		{
			coords[0*3+i] = 0;
			for( int j = 0; j < 3; j++ )
				coords[(j+1)*3+i] = axis[j*3+i] * m_arrLength[j];
			coords[2*3+i] = axis[1*3+i] * m_arrLength[1];
			coords[3*3+i] = axis[2*3+i] * m_arrLength[2];
		}
		double proj[4*2];
		for( int i = 0; i < 4; i++ )
		{
			GetGLProjection(&coords[i*3], &proj[i*2]);
		}
		for( int i = 1; i < 4; i++ )
		{
			for( int j = 0; j < 2; j++ )
				proj[i*2+j] -= proj[0*2+j];
		}
		// Precision
		double precision = 0.0f;
		for( int i = 0; i < 3; i++ )
		{
			double pick[2];
			double proj2d[2];
			for( int j = 0; j < 2; j++ )
			{
				pick[j] = m_CoordinatePositions[(i+1)*2+j] - m_CoordinatePositions[0*2+j];
				proj2d[j] = proj[(i+1)*2+j];
			}
			Normalize(2, pick);
			Normalize(2, proj2d);
			precision = precision + DotProduct(2, pick, proj2d);
		}
		precision = precision - 2;
		double angle = acos(MAX(MIN(precision, 1), -1));
		angle = MIN(angle, 2*PI - angle);
		precision = fabs((PI / 2 - angle)) * 100 / (PI / 2);
		QString strPrecision;
		strPrecision.setNum(precision, 'f', 2);
		QString strText = TR("Precision: ");
		strText += strPrecision;
		strText += "%";
#if DRAW_TEXT
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		pWidget->renderText(30, 30, strText, *m_pFont, 2000 );
#endif
#if CAMERA_TEST /*003*/
		for( int i = 0; i < 3; i++ )
		{
			//glColor4fv(&coordinateColor[(i+1)*4]);
			glColor4f(coordinateColor[(i+1)*4], coordinateColor[(i+1)*4+1], coordinateColor[(i+1)*4+2], 0.5f);
			glBegin(GL_LINES);
			glVertex3f(coords[0], coords[1], coords[2]);
			glVertex3f(coords[(i+1)*3+0], coords[(i+1)*3+1], coords[(i+1)*3+2]);
			glEnd();
		}
#endif

		for( int i = 1; i < 4; i++ )
		{
			double pick[2];
			for( int j = 0; j < 2; j++ )
				pick[j] = m_CoordinatePositions[i*2+j] - m_CoordinatePositions[0*2+j];
			if(DotProduct(2, pick, &proj[i*2]) > 0)
			{
				Inverses[i-1] = false;
			}
			else
			{
				Inverses[i-1] = true;
				// Inverses[i-1] = false;
			}
		}

		float center[3] = {0.0f, 0.0f, 0.0f};
		glPushMatrix();
		float fBoxScale = m_arrLength[3];
		glScalef(fBoxScale, fBoxScale, fBoxScale);
		DrawBox(center, 0.1, 0.1, 0.1, Inverses);
		glPopMatrix();
		glDisable(GL_BLEND);
	}
	glPopMatrix();
	glEnable(GL_LIGHTING);
}
void CCameraInfo::StartSetFloor()
{
	m_nCalibState = CS_SETFLOOR;
	m_nIndex = 0;
	m_blComplete = false;
}
bool CCameraInfo::StartSetScale(QWidget * pWidget)
{
	//if(m_nIndex >= 4)
	{
		// Get Helper Axis
		// Show Axis Dialog
		m_pParentWidget = pWidget;
		QStringList items;
		QString strXAxis = STR_CAMERA_X_AXIS;
		QString strYAxis = STR_CAMERA_Y_AXIS;
		QString strZAxis = STR_CAMERA_Z_AXIS;
		m_nRotationCorrect = 0;
		//items << strXAxis << strYAxis << strZAxis;
		items << strYAxis;//-temp

		bool ok;
		QString item = QInputDialog::getItem(m_pParentWidget, STR_CAMERA_SELECT_HELPER_AIXS,
			STR_CAMERA_AXIS_, items, 0, false, &ok);
		if (ok && !item.isEmpty())
		{
			m_nCalibState = CS_SETSCALE;
			m_blComplete = false;
			m_nIndex = 4;
			if(item.compare(strXAxis) == 0)
			{
				m_nAxis = AT_X;
			}
			else if(item.compare(strYAxis) == 0)
			{
				m_nAxis = AT_Y;
			}
			else if(item.compare(strZAxis) == 0)
			{
				m_nAxis = AT_Z;
			}
			return true;
		}
		else
		{
			return false;
		}	
	}
}
void CCameraInfo::SetScaleParam(int nScaleAxis, double fScale)
{
	m_nScaleAxis = nScaleAxis;
	m_fScale = fScale / 100.f;
}

void CCameraInfo::ChangeFOV()
{
	//printf("FOV : %f\n", fov);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_fFOV, (float)m_nWidth/(float)m_nHeight, 0.1f, 100000.0f);
	if(m_nIndex == 6)
		Calc(m_CoordinatePositions, CALIBRATION_SCALE, m_nAxis, m_nScaleAxis);
}

void CCameraInfo::ResetParameters()
{
	m_nIndex = -1;
	m_nHighLightID = -1;
	m_nCalibState = CS_NONE;
	m_blCapture = false;
	m_blComplete = false;
	m_nAxis = AT_X;
	m_fScale = CALIBRATION_SCALE;
	m_nRotationCorrect = 0;
	/*for( int i = 0;i < 6; i++)
	{
		m_CoordinatePositions[i*2+0] = -2000.0f;
		m_CoordinatePositions[i*2+1] = -2000.0f;
	}*/
	m_CoordinatePositions[0*2+0] = m_nWidth / 2;
	m_CoordinatePositions[0*2+1] = m_nHeight / 2;
	m_CoordinatePositions[1*2+0] = m_CoordinatePositions[0*2+0] + (100);
	m_CoordinatePositions[1*2+1] = m_CoordinatePositions[0*2+1] + (30);
	m_CoordinatePositions[2*2+0] = m_CoordinatePositions[0*2+0] + (0);
	m_CoordinatePositions[2*2+1] = m_CoordinatePositions[0*2+1] + (-120);
	m_CoordinatePositions[3*2+0] = m_CoordinatePositions[0*2+0] + (-50);
	m_CoordinatePositions[3*2+1] = m_CoordinatePositions[0*2+1] + (50);
	m_CoordinatePositions[4*2+0] = -2000;//m_nWidth / 2;
	m_CoordinatePositions[4*2+1] = -2000;//m_nHeight / 2;
	m_CoordinatePositions[5*2+0] = -2000;//m_nWidth / 2;
	m_CoordinatePositions[5*2+1] = -2000;//m_nHeight / 2;
}
void CCameraInfo::OnResize()
{
	//
}
void CCameraInfo::SetComplete()
{
	m_blComplete = true;
}
bool CCameraInfo::GetStageComplete()
{
	return m_blComplete;
}

bool CCameraInfo::IsCalibComplete()
{
	return (m_nIndex == 6);
}

bool CCameraInfo::IsEnable()
{
	if(m_nIndex >= 0 && m_nIndex < 4 && m_nCalibState == CS_SETFLOOR)
		return true;
	if(m_nIndex > 3 && m_nCalibState == CS_SETSCALE)
		return true;
	return false;
}

// for read/write function
bool CCameraInfo::WriteData(QFile* a_pFile)
{
	a_pFile->write((char*)&m_fFx, sizeof(double));
	a_pFile->write((char*)&m_fFy, sizeof(double));
	a_pFile->write((char*)&m_fCx, sizeof(double));
	a_pFile->write((char*)&m_fCy, sizeof(double));
	a_pFile->write((char*)&m_fS, sizeof(double));
	a_pFile->write((char*)&m_fK1, sizeof(double));
	a_pFile->write((char*)&m_fK2, sizeof(double));
	a_pFile->write((char*)&m_fK3, sizeof(double));
	a_pFile->write((char*)&m_fP1, sizeof(double));
	a_pFile->write((char*)&m_fP2, sizeof(double));
	a_pFile->write((char*)&m_fR1, sizeof(double));
	a_pFile->write((char*)&m_fR2, sizeof(double));
	a_pFile->write((char*)&m_fR3, sizeof(double));
	a_pFile->write((char*)&m_fT1, sizeof(double));
	a_pFile->write((char*)&m_fT2, sizeof(double));
	a_pFile->write((char*)&m_fT3, sizeof(double));

	a_pFile->write((char*)&m_nWidth, sizeof(int));
	a_pFile->write((char*)&m_nHeight, sizeof(int));

	a_pFile->write((char*)&m_fFOV, sizeof(double));
	a_pFile->write((char*)m_CoordinatePositions, sizeof(float) * 12);
	a_pFile->write((char*)m_arrLength, sizeof(double) * 4);
	a_pFile->write((char*)&m_nRotationCorrect, sizeof(int));
	
	a_pFile->write((char*)&m_nCalibState, sizeof(int));
	a_pFile->write((char*)&m_nIndex, sizeof(int));
	a_pFile->write((char*)&m_blComplete, sizeof(bool));
	a_pFile->write((char*)&m_nAxis, sizeof(int));
	a_pFile->write((char*)&m_nScaleAxis, sizeof(int));
	
	return true;
}

bool CCameraInfo::ReadData(QFile* a_pFile)
{
	a_pFile->read((char*)&m_fFx, sizeof(double));
	a_pFile->read((char*)&m_fFy, sizeof(double));
	a_pFile->read((char*)&m_fCx, sizeof(double));
	a_pFile->read((char*)&m_fCy, sizeof(double));
	a_pFile->read((char*)&m_fS, sizeof(double));
	a_pFile->read((char*)&m_fK1, sizeof(double));
	a_pFile->read((char*)&m_fK2, sizeof(double));
	a_pFile->read((char*)&m_fK3, sizeof(double));
	a_pFile->read((char*)&m_fP1, sizeof(double));
	a_pFile->read((char*)&m_fP2, sizeof(double));
	a_pFile->read((char*)&m_fR1, sizeof(double));
	a_pFile->read((char*)&m_fR2, sizeof(double));
	a_pFile->read((char*)&m_fR3, sizeof(double));
	a_pFile->read((char*)&m_fT1, sizeof(double));
	a_pFile->read((char*)&m_fT2, sizeof(double));
	a_pFile->read((char*)&m_fT3, sizeof(double));

	a_pFile->read((char*)&m_nWidth, sizeof(int));
	a_pFile->read((char*)&m_nHeight, sizeof(int));

	a_pFile->read((char*)&m_fFOV, sizeof(double));
	a_pFile->read((char*)m_CoordinatePositions, sizeof(float) * 12);
	a_pFile->read((char*)m_arrLength, sizeof(double) * 4);
	a_pFile->read((char*)&m_nRotationCorrect, sizeof(int));

	a_pFile->read((char*)&m_nCalibState, sizeof(int));
	a_pFile->read((char*)&m_nIndex, sizeof(int));
	a_pFile->read((char*)&m_blComplete, sizeof(bool));

	a_pFile->read((char*)&m_nAxis, sizeof(int));
	a_pFile->read((char*)&m_nScaleAxis, sizeof(int));

	if(m_nCalibState == CS_SETSCALE && !m_blComplete)
	{
		m_nIndex = 4;
	}

	return true;
}
