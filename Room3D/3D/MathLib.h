//----------------------------------------------//
//	Copyright (c) 2012 -- All Rights Reserved	//
//----------------------------------------------//
//	File: MathLib.h								//
//	Desc: 										//
//	Update: 2012.6				 				//
//----------------------------------------------//


#pragma once
#include <float.h>

// GEMM Flags
#define MATOP_GEMM_A_T 1
#define MATOP_GEMM_B_T 2
#define MATOP_GEMM_C_T 4

//extern   int nMathLib;

#define SQR(a) ((a) * (a))
#define ABS(a) ((a)>=0 ? (a) : -(a))
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#define PI 3.1415926535897932384626433832795
#define A2R(x) (x*PI/180)
#define R2A(x) (x*180/PI)
#define  CMP(a,b)    (((a) > (b)) - ((a) < (b)))
#define  SIGN(a)     CMP((a),0)

//             MATRIX OPERATION            //
// The function Transpose transposes m*m matrix.
  void Transpose(int m, double * mat);

// Transpose src to dst
// dst = src^T
// src : m*n
// dst : n*m
  inline void Transpose( int m, int n, double * src, double * dst );

  inline void Rotate90( int m, int n, int * src, int * dst );
  inline void Rotate180( int m, int n, int * src, int * dst );
  inline void Rotate270( int m, int n, int * src, int * dst );

// dst = mat1 * mat2
// mat1 : m*n
// mat2 : n*k
// Deprecated
  void MatMul( int m, int n, int k, double * mat1, double * mat2, double * dst );

//	[0 -w y]
//	[w 0 -x]
//	[-y x 0]
  void VecMul(double * src, double * dst);

/*The function GEMM performs generalized matrix multiplication:
dst = alpha*op(src1)*op(src2) + beta*op(src3), where op(X) is X or XT.
All the matrices should have the same data type and the coordinated sizes.*/
  void GEMM(int m, int n, int k, double * src1, double * src2, double alpha,
		  double * src3, double beta, double * dst, int tABC, bool bUseOMP = false);

// Pinv
// pinv(A) = (A'A)^-1 * A'
// Using svd.
  void Pinv( int m, int n, double * A, double * invA );

// return Norm(mat1-mat2)
  inline double SqNorm( int m, const double * mat1, const double * mat2 = 0 );
  inline double Norm( int m, const double * mat1, const double * mat2 = 0 );

  inline double Det(double * A);
  double Det(int m, double * A);







//             VECTOR OPERATION            //

// C = A x B;
  void CrossProduct(double * A, double * B, double * C);

// return A . B;
  double DotProduct(double * A, double * B);
  double DotProduct(int m, double * A, double * B);

// project vector to plane.
  void ProjectVector2Plane(double * vect, double * normal, double * newVect);

// Normalize A so that norm(A) = 1;
  void Normalize(double *A);

  void Normalize(int m, double *A);










//                     SVD                        //

#define MATHLIB_SVD_MODIFY_A   1
#define MATHLIB_SVD_U_T        2
#define MATHLIB_SVD_V_T        4

// Singular Value Decomposition
// Decompose m*n matrix A so that A = U*S*V'
// where U,V is a m*m, n*n rotation matrix and S is m*n diagonal matrix with unaccendent members.
// Returns iteration count.
// * * Read [Matrix Computations], [Handbook of Linear Algebra] for it's principle.
  int SVD(int m, int n, double * matA, double * matS, double * matU = 0, double * matV = 0, int flags = 0);

/* Performs Singular Value Back Substitution (solves A*X = B):
flags must be the same as in cvSVD */
// W : m*n
// U : m*m
// V : n*n
// B : m * k
// X : n * k [out]
// A = U*W*V'
// X = VW-1U'B
// W-1 = 1/W(i,i)	if W(i,i)>eps(sig(i,W(i,i))) 
//		 0			otherwise
  void SVBkSb( int m, int n, int k, const double * W, const double * U, const double * V, const double * B,
						double * X, int flags = 0 );

// Solves X that minimize norm(Ax-b)
// A is m*n, b is m*k, X is n*k
  void Solve( int m, int n, int k, double * A, double * b, double * X);

// Solves X that minimize norm(AX)
// A is m*n and X is n*1
  void SolveHomo( int m, int n, double * A, double * X );



/*function g = givens(x,y)
%GIVENS Givens rotation matrix.
%   G = GIVENS(x,y) returns the complex Givens rotation matrix
%
%       | c       s |                  | x |     | r |
%   G = |           |   such that  G * |   |  =  |   |
%       |-conj(s) c |                  | y |     | 0 |
%
%   where c is real, s is complex, and c^2 + |s|^2 = 1. 

%   Copyright 1986-2002 The MathWorks, Inc. 
%   $Revision: 1.10 $  $Date: 2002/04/10 06:33:23 $*/
  void Givens( double x, double y, double * c, double * s );













//                     Matrix Decomposition                        //

// LU Decompose
// A=LDU
// L : lower triangle matrix
// D : diagonal matrix
// U : upper triangle matrix
// ?? Error: If diagonal element is zero, it fails.
  void LUDecompose( int m, const double * A, double * L, double * U, double * D);

// Cholesky Decompose
// A=R'R
// R : upper triangle matrix
// A : symmetric positive definite matrix
// return false if A isn't positive definite matrix.
// ?? Error because of LU.
  bool CholDecompose( int m, const double * A, double * R, bool bTranspose = false);






//                     Rodrigues Convert                        //
// if bRodrigues is true, src is vector, dst is matrix.
// ??? matrix-->vector code have to be refined.
// jacobian : 9 * 3 matrix
  inline void Rodrigues2( const double * src, double * dst, bool bRodrigues = false, double * jacobian = 0 );


// get jacobian of rotation matrix by rotation angle
// rotation : 3 by 1 rodrigues rotation vector
  inline void getRotationJacobianByRotationAngle(double * rotation, double * jacobian);


// Correct rotation matrix with SVD.
// [u,s,v] = svd(rot)
// rot = uiv';
  int CorrectRotationMatrix( double * rot );

  void CorrectRotationMatrix2D( double * rot );

// Convert rotation matrix to Z-Y-X Euler Angles (alpha, beta, gamma)
// beta = atan2(-r31, sqrt(r11^2+r21^2))
// alpha = atan2(r21/cosb, r11/cosb)
// gamma = atan2(r32/cosb, r33/cosb)

// Convert rotation matrix to Z-X-Y Euler Angles (alpha, gamma, beta)
// gamma = atan2(r32, sqrt(r12^2+r22^2))
// alpha = atan2(-r12/cosr, r22/cosr)
// beta = atan2(-r31/cosr, r33/cosr)
  void ConvertRotationMatrix2EulerAngles(double * rot, double * angles);

// Convert rotation matrix to X-Y-Z Euler Angles (alpha, beta, gamma)
// beta = atan2(r13, sqrt(r11^2+r12^2))
// alpha = atan2(-r23/cosb, r33/cosb)
// gamma = atan2(-r12/cosb, r11/cosb)
  void ConvertRotationMatrix2XYZEulerAngles(double * rot, double * angles);
  void ConvertRotationVector2XYZEulerAngles(double * vect, double * angles, double * Jacobian = 0);

  void ConvertEulerAngles2RotationMatrix(double * angles, double * rot);
  void ConvertXYZEulerAngles2RotationMatrix(double * angles, double * rot);

  inline void ConvertRotVect2RotAxisAngle(double * pRotVect, double * pRotAxisAngle); // 3*1 to 4*1

  void GetAxisRotation(char chAxis, double angle, double * rot);

//  void DecomposeProjMat( double * P, double * K, double * R, double * T );


// Levenberg-Marquardts
class   CLevMarq
{
public:
	CLevMarq(void);
	~CLevMarq(void);

	void init( int nParams, int max_iter = 30, double epsilon = DBL_EPSILON );
	bool update(const double *& param, double *& JtJ, double *& JtErr, double *& errNorm);

	enum { DONE=0, STARTED=1, CALC_J=2, CHECK_ERR=3 };

public:
	bool * mask;
	double * param;

private:
	int max_iter;
	double epsilon;

	int nparams;

	double * prevParam;

	double * JtJ;
	double * JtErr;
	double * JtJN;
	double * JtJV;
	double * JtJW;
	double prevErrNorm, errNorm;

	int lambdaLg10;
	int state;

	int iters;

	void step();
	void clear();

public:
	int	   getStep() {return iters;}
	double getErrNorm() {return errNorm;}
};

// Bundle Adjustment
class   CBundleAdjustment
{
public:
	CBundleAdjustment(void);
	~CBundleAdjustment(void);

	//void init( int nSize1, int nSize2, int nCount1, int nCount2, int max_iter = 30, double epsilon = DBL_EPSILON );
	void init( int nParams1, int nSize2, int nCount2, int max_iter = 30, double epsilon = DBL_EPSILON );
	bool update(const double *& param, double *& JtJU, double *& JtJW, double *& JtJV, double *& JtErr, double *& errNorm);

	enum { DONE=0, STARTED=1, CALC_J=2, CHECK_ERR=3 };

public:
	bool * mask;
	double * param;

private:
	int max_iter;
	double epsilon;

	int /*nsize1,*/ nsize2;			// Projection matrix DOF (P:11, M:16) vs World point DOF (3)
	int /*ncount1,*/ ncount2;		// Projection matrix count (8) vs World point count (100)
	int nparams1, nparams2;		// nparams1 = nsize1*ncount1, nparams2 = nsize2*ncount2
	int nparams;				// nparams = nparams1 + nparams2;

	double * prevParam;

	double * JtJU;				// U = {Uk | sig(i) (d mki / d Pk)' (d mki / d Pk)}		nparams1*nparams1
	double * JtJW;				// W = {Wki|		(d mki / d Pk)' (d mki / d Mi)}		nparams1*nparams2
	double * JtJV;				// V = {Vi | sig(k) (d mki / d Mi)' (d mki / d Mi)}		(nsize2*nsize2)^ncount2
	double * JtErr;				// Err={errPk errMi | sig(i) (d mki / d Pk)' err(ki) and sig(k) (d mki / d Mi)' err(ki)}	nparams*1

	double * _JtJUmWViWtS;		// For SVD decompose.
	double * _JtJUmWViWtU;

	double * _JtJVi;			// V-1
	double * _JtJWVi;			// WV-1
	double * _JtJUmWViWt;		// U - WV-1W'
	double * _JtErrPM;			// errP - WV-1errM and errM - W'dP
	double prevErrNorm, errNorm;

	int lambdaLg10;
	int state;

	int iters;

	void step();
	void clear();
};



// memory operation
#define COPY(src, dst, type, count) ( memcpy((dst), (src), sizeof(type)*(count)) )
