#include "MathLib.h"
#include <math.h>
#include <assert.h>
#include <memory.h>

#define min(a,b) (a<b?a:b)
#define max(a,b) (a>b?a:b)

#pragma region Matrix Operations
  void Transpose(int m, double * A)
{
	double temp;
	int i, j;
//#pragma omp parallel for private(i,j)
	for( i = 0; i < m; i++ )
		for( j = 0; j < i; j++ )
		{
			temp = A[i*m+j];
			A[i*m+j] = A[j*m+i];
			A[j*m+i] = temp;
		}
}

  inline void Transpose( int m, int n, double * src, double * dst )
{
#ifdef MATHLIB_WITH_OPENCV
	CvMat mat_src = cvMat(m,n, CV_64FC1, src);
	CvMat mat_dst = cvMat(n,m, CV_64FC1, dst);
	cvTranspose(&mat_src, &mat_dst);
#else
	double * temp = new double[m*n];
	int i, j;
//#pragma omp parallel for private(i,j)
	for( i = 0; i < m; i++ )
		for( j = 0; j < n; j++ )
			temp[j*m+i] = src[i*n+j];
//#pragma omp parallel for private(i,j)
	for( i = 0; i < m; i++ )
		for( j = 0; j < n; j++ )
			dst[i*n+j] = temp[i*n+j];
	delete [] temp;
#endif
}

  inline void Rotate90( int m, int n, int * src, int * dst )
{
	for( int i = 0; i < m; i++ )
		for( int j = 0; j < n; j++ )
			dst[j*m + (m-i-1)] = src[i*n+j];
}
  inline void Rotate180( int m, int n, int * src, int * dst )
{
	for( int i = 0; i < m; i++ )
		for( int j = 0; j < n; j++ )
			dst[(m-i-1)*n + (n-j-1)] = src[i*n+j];
}
  inline void Rotate270( int m, int n, int * src, int * dst )
{
	for( int i = 0; i < m; i++ )
		for( int j = 0; j < n; j++ )
			dst[(n-j-1)*m + i] = src[i*n+j];
}

  void MatMul( int m, int n, int k, double * mat1, double * mat2, double * dst )
{
	for( int i = 0; i < m; i++ )
		for( int l = 0; l < k; l++ )
		{
			dst[i*k+l] = 0;
			for( int j = 0; j < n; j++ )
				dst[i*k+l] += mat1[i*n + j] * mat2[j*k+l];
		}
}

void CloneMatrix( int m, double * src, double * dst)
{
	for( int i = 0; i < m; i++ )
		dst[i] = src[i];
}

void VecMul(double * src, double * dst)
{
	//	[0 -w y]
	//	[w 0 -x]
	//	[-y x 0]

	dst[0] = dst[4] = dst[8] = 0;
	dst[1] = -src[2]; dst[2] = src[1]; dst[5] = -src[0];
	dst[3] = src[2]; dst[6] = -src[1]; dst[7] = src[0];
}

#define GEMM_C_LOOP for( i = 0; i < rows; i++ ) \
	for( l = 0; l < cols; l++ )
/*#define GEMM_AB_LOOP for( i = 0; i < rows; i++ ) \
	for( j = 0; j < mids; j++ ) \
		for( l = 0; l < cols; l++ )*/
#define GEMM_AB_LOOP(i1,i2,i3, n1, n2, n3) for( i1 = 0; i1 < n1; i1++ ) \
	for( i2 = 0; i2 < n2; i2++ ) \
		for( i3 = 0; i3 < n3; i3++ )
		

#define GEMM_D_ZERO GEMM_C_LOOP \
	D[i*cols+l] = 0.0;
#define GEMM_D_C GEMM_C_LOOP \
	D[i*cols+l] = beta * C[i*cols+l];
#define GEMM_D_Ct GEMM_C_LOOP \
	D[i*cols+l] = beta * C[l*rows+i];
#define GEMM_A_B GEMM_AB_LOOP(i,j,l, rows,mids,cols) \
	D[i*cols+l] += alpha * A[i*mids+j] * B[j*cols+l];
#define GEMM_A_Bt GEMM_AB_LOOP(i,l,j, rows,cols,mids) \
	D[i*cols+l] += alpha * A[i*mids+j] * B[l*mids+j];
#define GEMM_At_B GEMM_AB_LOOP(i,j,l, rows,mids,cols) \
	D[i*cols+l] += alpha * A[j*rows+i] * B[j*cols+l];
/*#define GEMM_At_Bt GEMM_AB_LOOP(l,j,i, cols,mids,rows) \
	D[i*cols+l] += alpha * A[j*rows+i] * B[l*mids+j];*/
#define GEMM_At_Bt GEMM_AB_LOOP(i,j,l, cols,mids,rows) \
	/*D[i*rows+l]*/D[l*cols+i] += alpha * B[i*mids+j] * A[j*rows+l];
/*#define GEMM_At_Bt GEMM_AB_LOOP(i,j,l, rows,mids,cols) \
	D[i*cols+l] += alpha * A[j*rows+i] * B[l*mids+j];*/
//       atbt ab   abt    atb
//i j l 22 25 o           o
//i l j 18 19      o
//j i l 24 25
//j l i 16 16
//l i j 19 18
//l j i 15 14o

  void GEMM(int m, int n, int k, double * src1, double * src2, double alpha,
		  double * src3, double beta, double * dst, int tABC, bool bUseOMP)
{
	// 2010/8/9 
	// Performance up by using original data.
	//bool bUseOMP = true;
	double * A = src1;
	double * B = src2;
	double * C = src3;
	double * D = dst;

	int rows = m;
	int cols = k;
	int mids = n;

	int i, j, l;
	int nthr = 1;

	bool bA = !(tABC & MATOP_GEMM_A_T);
	bool bB = !(tABC & MATOP_GEMM_B_T);
	bool bCExist = C != 0;
	bool bC = !(tABC & MATOP_GEMM_C_T);

	{ // conditions for C
		if(!bCExist) //C=0
			GEMM_D_ZERO
		else if(bC) // C
			GEMM_D_C
		else		// Ct
			GEMM_D_Ct
	}

	{ // conditions for A & B
		if(bA && bB)
			GEMM_A_B
		else if(bA && !bB)
			GEMM_A_Bt
		else if(!bA && bB)
			GEMM_At_B
		else
			GEMM_At_Bt
	}
}

  void Pinv( int m, int n, double * A, double * invA )
{
	double * A_U = new double[m*m];
	double * A_S = new double[m*n];
	double * A_V = new double[n*n];

	double * X = invA;
	double * EyeM = new double[m*m];
	for( int i = 0; i < m; i++ )
		for( int j = 0; j < m; j++ )
			if(i==j)
				EyeM[i*m+j] = 1;
			else
				EyeM[i*m+j] = 0;
	SVD(m,n,A,A_S,A_U,A_V);
	SVBkSb(m,n,m, A_S,A_U,A_V, EyeM, X);

	delete [] A_U;
	delete [] A_S;
	delete [] A_V;
	delete [] EyeM;
}

  inline double SqNorm( int m, const double * mat1, const double * mat2)
{
	double sqnorm = 0;
	for( int i = 0; i < m; i++ )
	{
		if(mat2!=0)
			sqnorm += SQR(mat1[i]-mat2[i]);
		else
			sqnorm += SQR(mat1[i]);
	}
	return sqnorm;
}
  inline double Norm( int m, const double * mat1, const double * mat2 )
{
	double norm = 0;
	for( int i = 0; i < m; i++ )
	{
		if(mat2!=0)
			norm += SQR(mat1[i]-mat2[i]);
		else
			norm += SQR(mat1[i]);
	}
	return sqrt(norm);
}

  inline double Det(double * A)
{
	return A[0] * A[4] * A[8] + A[1] * A[5] * A[6] + A[2] * A[3] * A[7] -
		A[2] * A[4] * A[6] - A[1] * A[3] * A[8] - A[0] * A[5] * A[7];
}

  double Det(int m, double * A)
{
	return 0;
}
#pragma endregion

#pragma region Vector Operation
  void CrossProduct(double * v1, double * v2, double * out)
{
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];
}

  double DotProduct(double * A, double * B)
{
	return DotProduct(3, A, B);
}
  double DotProduct(int m, double * A, double * B)
{
	double sum = 0;
	for( int i = 0; i < m; i++ )
		sum += A[i] * B[i];
	return sum;
}

  void ProjectVector2Plane(double * vect, double * normal, double * newVect)
{
	double z_axis[3];
	// normal --> x
	// vect --> y
	// z
	CrossProduct(normal, vect, z_axis);
	CrossProduct(z_axis, normal, newVect);
}

  void Normalize(double *A)
{
	double length;

	// Calculate the length of the vector		
	length = (double)sqrt((A[0]*A[0]) + 
		(A[1]*A[1]) +
		(A[2]*A[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	A[0] /= length;
	A[1] /= length;
	A[2] /= length;
}

  void Normalize(int m, double *A)
{
	double norm = Norm(m, A);
	if(norm == 0)
		norm = 1;
	for( int i = 0; i < m; i++ )
		A[i] /= norm;
}
#pragma endregion


#pragma region SVD Section
/*
% Householder: Householder transform:
% Input: n,x,y where x,y is n*1, and norm(x) = norm(y)
% Output: H so that H is n*n orthogonal matrix, and Hx = y
*/
void Householder( double * x, double * u, double * beta, int m )
{
	double ro = 0;
	// 1. u <- x - y
	// 2. beta = 2 / (u'u)
	for( int i = 1; i < m; i++ )
	{
		ro += x[i] * x[i];
		u[i] = x[i];
	}
	if(ro < DBL_EPSILON)
	{
		if(x[0] < 0)
		{
			u[0] = 1;
			*beta = 2;
		}
		else
			*beta = 0;
	}
	else
	{
		double miu = sqrt(ro + x[0] * x[0]);
		if(x[0] <= 0)
			u[0] = x[0] - miu;
		else
			u[0] = -ro / (x[0] + miu);

		*beta = 2. * u[0] * u[0] / (ro + u[0] * u[0]);
		for( int i = 1; i < m; i++ )
			u[i] /= u[0];
		u[0] = 1;
	}
}

void MulHouseholderDiagLeft( int m, int n, double * u, double beta, double * A, int k, double * _tS )
{
	// Bidiagonalize B with left householder multiplication.
	//int m = A->rows, n = A->cols;

	/*for( int i = k; i < n; i++ )
	{
	double temp = 0;
	for( int j = k; j < m; j++ )
	temp += A->data[j*n+i] * u->data[j-k];
	temp = beta * temp;
	for( int j = k; j < m; j++ )
	A->data[j*n+i] -= temp * u->data[j-k];
	}*/
	double * _A;
	double * _tempS = _tS;
	double * _U = u;
	memset(_tS+k, 0, sizeof(double)*(n-k));
	for( int i = k; i < m; i++ )
	{
		double s = _U[i-k];
		_A = A + i*n;
		for( int j = k; j < n; j++ )
			_tempS[j] += _A[j] * s;
	}
	for( int j = k; j < n; j++ )
		_tempS[j] *= beta;
	for( int i = k; i < m; i++ )
	{
		_A = A + i*n;
		double s = _U[i-k];
		for( int j = k; j < n; j++ )
			_A[j] -= s * _tempS[j];
	}
	_A = A+(k+1)*n+k;
	_U -= k;
	for( int i = k+1; i < m; i++, _A+=n)
		*_A = _U[i];
}

void MulHouseholderSuperRight( int m, int n, double * u, double beta, double * A, int k )
{
	// Bidiagonalize B with right householder multiplication.
	//int m = A->rows, n = A->cols;

	/*for( int i = k; i < m; i++ )
	{
	double temp = 0;
	for( int j = k+1; j < n; j++ )
	temp += A->data[i*n+j] * u->data[j-k-1];
	temp = beta * temp;
	for( int j = k+1; j < n; j++ )
	A->data[i*n+j] -= temp * u->data[j-k-1];
	}*/

	double * _A = A + k*n, * _U;
	for( int i = k; i < m; i++ )
	{
		double temp = 0;
		_U = u - k - 1;
		for( int j = k+1; j < n; j++ )
			temp += _A[j] * _U[j];
		temp = beta * temp;
		for( int j = k+1; j < n; j++ )
			_A[j] -= temp * _U[j];
		_A += n;
	}
	_A = A + k*n;
	_U = u+1;
	for( int i = k+2; i < n; i++, _U++ )
		_A[i] = *_U;
}

void MulHouseholderDiagRightT(int m, double * bk, double beta, double * A,int k, double * _tS)
{
	//int m = A->rows;
	double * _A;
	double * _tempS = _tS;
	double * _U = bk - k;
	memcpy(_tempS+k, A+k*m+k, (m-k)*sizeof(double) );

	_A = A + k*m+m;
	for( int i = k+1; i < m; i++, _A+=m )
	{
		double s = _U[i];
		for( int j = k; j < m; j++ )
			_tempS[j] += _A[j] * s;
	}
	for( int j = k; j < m; j++ )
		_tempS[j] *= beta;
	_A = A + k*m;
	for( int i = k; i < m; i++, _A+=m )
	{
		double s = _U[i];
		for( int j = k; j < m; j++ )
			_A[j] -= _tempS[j] * s;
	}
}

void MulHouseholderSuperLeftT(int m, double * bk1, double beta, double * A, int k)
{
	//int m = A->rows;

	double * _A;
	double * _U = bk1-k-1;

	for( int i = k+1; i < m; i++ )
	{
		_A = A + i * m;
		double temp = _A[k+1];
		for( int j = k+2; j < m; j++ )
			temp += _A[j] * _U[j];
		temp *= beta;
		for( int j = k+1; j < m; j++ )
			_A[j] -= temp * _U[j];
	}
}

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
  void Givens( double x, double y, double * c, double * s )
{
	/*	absx = abs(x);
	if absx == 0.0
	c = 0.0; s = 1.0;
	else
	nrm = norm([x y]);
	c = absx/nrm;
	s = x/absx*(conj(y)/nrm);
	end
	g = [c s;-conj(s) c];*/
	if( x == 0. && y == 0. )
	{
		*c = 0.; *s = 1.;
	}
	else
	{	
		double nrm = sqrt(x*x+y*y);
		*c = x / nrm;
		*s = y / nrm;
	}
}

void LeftGivensRot( double * x, double * y, double c, double s, int n )
{
	double * _x = x;
	double * _y = y;
	double _c = c;
	double _s = s;
	int _n = n;
	double r1;
	for( int k = 0; k < _n; k++, _x++, _y++ )
	{
		r1 = *_x;
		*_x = _c * r1 + _s * *_y;
		*_y = -_s * r1 + _c * *_y;
	}
}

void Sort_SVD( int m, int n, double * B, double * U, double * V )
{
	/*
	temp = 0;
	tempV = zeros(n,1);
	tempU = zeros(m,1);
	for i=1:n-1
	for j=i+1:n
	if(B(i,i)<B(j,j))
	temp = B(i,i);
	B(i,i) = B(j,j);
	B(j,j) = temp;

	tempV = V(:,i);
	V(:,i) = V(:,j);
	V(:,j) = tempV;

	tempU = U(:,i);
	U(:,i) = U(:,j);
	U(:,j) = tempU;
	end
	end
	end
	*/
	double temp;
	for( int i = 0; i < n - 1; i++ )
		for( int j = i+1; j < n; j++ )
			if(B[i*n+i]<B[j*n+j])
			{
				temp = B[i*n+i];
				B[i*n+i] = B[j*n+j];
				B[j*n+j] = temp;

				if(V!=0)
					for( int k = 0; k < n; k++ )
					{
						temp = V[k*n+i];
						V[k*n+i] = V[k*n+j];
						V[k*n+j] = temp;
					}

					if(U!=0)
						for( int k = 0; k < m; k++ )
						{
							temp = U[k*m+i];
							U[k*m+i] = U[k*m+j];
							U[k*m+j] = temp;
						}
			}
}

void Sort_SVD( int m, int n, double * D, double * F, double * B, double * U, double * V )
{
	for( int i = 0; i < m; i++ )
		for( int j = 0; j < n; j++ )
		{
			if(i==j)
				B[i*n+j] = D[i];
			else
				B[i*n+j] = 0;
		}
		Sort_SVD(m,n,B,U,V);
}

int SVD_G(int m, int n, double * matA, double * matS, double * matU, double * matV);

  int SVD(int m, int n, double * matA, double * matS, double * matU, double * matV, int flags)
{
#ifdef MATHLIB_WITH_OPENCV
	CvMat mA, mS, mU, mV;
	mA = cvMat(m, n, CV_64FC1, matA);
	mS = cvMat(m, n, CV_64FC1, matS);
	mU = cvMat(m, m, CV_64FC1, matU);
	mV = cvMat(n, n, CV_64FC1, matV);
	CvMat *pmU, *pmV;
	if(matU==0)
		pmU = 0;
	else
		pmU = &mU;
	if(matV==0)
		pmV = 0;
	else
		pmV = &mV;
	cvSVD(&mA, &mS, pmU, pmV, flags);
	return -1;
#else
	// 2010/8/5 Jonghh
	// Original code was made with the assumption that m is not less than n.
	// if m<n then we have to rotate A and inverse the result, too.
	int ret;
	if(m>=n)
	{
		ret = SVD_G(m,n,matA,matS,matU,matV);
	}
	else
	{
		// A : m*n matrix with m < n
		// A = USV'
		// A'=(USV')'=VS'U'=U*S*V*'
		// U <-- V*
		// S <-- S*'
		// V <-- U
		// We have to change U and V, and transpose S
		double * temp = new double[m*n];
		Transpose(m,n,matA,temp);
		/*for( int i = 0; i < m*n; i++ )
			matA[i] = temp[i];*/
		
		ret = SVD_G(n,m,temp,matS,matV,matU);
		Transpose(n,m,matS,temp);
		for( int i = 0; i < m*n; i++ )
			matS[i] = temp[i];

		delete [] temp;
	}
	return ret;
#endif
}

int SVD_G(int m, int n, double * matA, double * matS, double * matU, double * matV)
{
	

	// Algorithm 1a: Householder reduction to bidiagonal form:
	// Input: m,n,A where A is m*n.
	// Output: B,U,V so that B is upper bidiagonal, U and V are products of Householder matrices, and A=UBV??

	double * B = matS;

	//double * temp;

	//1.	B<-A.(This step can be omitted if A is to be overwritten with B.)

	for( int i = 0; i < m * n; i++ )
		B[i] = matA[i];

	//2.	U=I(m*m)
	//3.	V=I(n*n)
	double * U = matU;
	double * V = matV;

	if(U!=0)
	{
		//U->SetIdentity();
		for( int i = 0; i < m; i++ )
			for( int j = 0; j < m; j++ )
				if(i==j)
					U[i*m+j] = 1;
				else
					U[i*m+j] = 0;
	}
	if(V!=0)
	{
		//assert(V->rows==V->cols && V->rows ==n);
		//V->SetIdentity();
		for( int i = 0; i < n; i++ )
			for( int j = 0; j < n; j++ )
				if(i==j)
					V[i*n+j] = 1;
				else
					V[i*n+j] = 0;
	}

	/*Matrix * UQk = new Matrix(m, 1, new double[m]);
	Matrix * UPk = new Matrix(n, 1, new double[n]);*/
	double * UQk = new double[m];
	double * UPk = new double[n];
	double * bk = new double[m];
	double * bk1 = new double[n];

	double * scale_u = new double[n];
	double * scale_v = new double[n-1];

	double * _tS = new double[m];

	double beta;
	//4. For k = 1, ..., n
	for( int k = 0; k < n; k++ )
	{
		//   a. Determine Householder matrix Qk with the property that:
		//       - Left multiplication by Qk leaves components 1, ..., k-1
		//       unaltered, and alter b[k+i,k] = 0. Qk[...]' = [...]'

		/*			bk = B(:,k);
		bkn = bk;
		s = 0;
		for k1 = k : m
		s = s + B(k1,k) * B(k1,k);
		bkn(k1) = 0;
		end
		bkn(k) = sqrt(s);
		if bk == bkn
		Qk = eye(m);
		else
		Qk = householder(m, bk, bkn);
		end*/
		for( int i = k; i < m; i++ )
			bk[i-k] = B[i*n+k];

		Householder(bk,UQk,&beta,m-k);

		scale_u[k] = beta;
		/*		    b. B <- QkB
		B = Qk * B;*/
		MulHouseholderDiagLeft(m,n, UQk,beta,B,k, _tS);

		/*		    c. U <- UQk
		U = U * Qk;*/
		//if(U!=0)
		//	MulHouseholderDiagRight(UQk,beta,U,k);

		//		    d. If k <= n-2, determine Householder matrix Pk+1 with the property
		//			   that:
		//		       - Right multiplication by Pk+1 leaves components 1, ..., k
		//			       unaltered, and alter b[k,k+1+i] = 0. [...]Pk+1 = [...]
		/*		if (k <= n-2)
		bk1 = B(k,:);
		bkn1 = bk1;
		s = 0;
		for k1 = k+1 : n
		s = s + B(k,k1) * B(k,k1);
		bkn1(k1) = 0;
		end
		bkn1(k+1) = sqrt(s);
		if bk1 == bkn1
		Pk = eye(n);
		else
		Pk = householder(n, bk1', bkn1')';
		end*/
		if( k < n - 2 )
		{
			for( int i = k+1; i < n; i++ )
				bk1[i-k-1] = B[k*n+i];

			Householder(bk1,UPk,&beta,n-k-1);

			scale_v[k] = beta;

			//		e. B <- BPk+1
			/*B = B * Pk;*/
			MulHouseholderSuperRight(m,n, UPk,beta,B,k);

			//		f. V <- Pk+1V
			/*V = Pk * V;*/

			/*	end
			end*/
		}
	}

	if(U!=0)
	{
		//MulHouseholderDiagRight(UQk,beta,U,k);
		bk[0] = 1.;
		for( int i = n-1; i >= 0; i--)
		{
			for( int j = i+1; j < m; j++ )
				bk[j-i] = B[j*n+i];
			MulHouseholderDiagRightT(m, bk, scale_u[i],U,i, _tS);
		}
	}

	if(V!=0)
	{
		//MulHouseholderSuperLeft(UPk,beta,V,k);
		bk1[0] = 1.;
		for( int i = n-3; i >= 0; i--)
		{
			for( int j = i+2; j < n; j++ )
				bk1[j-i-1] = B[i*n+j];
			MulHouseholderSuperLeftT(n, bk1, scale_v[i],V,i);
		}
	}

	delete [] bk;
	delete [] bk1;

	delete [] scale_u;
	delete [] scale_v;

	delete [] _tS;

	//MATOP_DELETE_MAT(UQk);
	//MATOP_DELETE_MAT(UPk);
	delete [] UQk;
	delete [] UPk;

	/* Bidiagonalization result --> to dn and fn */
	double * arrD = new double[n];
	double * arrF = new double[n-1];
	for( int i = 0; i < n; i++ )
		arrD[i] = B[i*n+i];
	for( int i = 0; i < n - 1; i++ )
		arrF[i] = B[i*n+i+1];
	/*Matrix * D = new Matrix(n, 1, arrD);
	Matrix * F = new Matrix(n-1, 1, arrF);*/
	double * D = arrD;
	double * F = arrF;

	// Algorithm 1b: Golub-Reinsch SVD:
	// Input: m,n,A where A is m*n.
	// Output: S,U,V so that S is diagonal, U and V have orthonormal
	// columns, U is m*n, V is n*n, and A=USV'.

	// 1.Apply Algorithm 1a to obtain B,U,V so that B is upper bidiagonal, U
	//	 and V are products of Householder matrices, and A=UBV'

	/*[B,U,V] = algorithm1a(m,n,A);
	V = V';*/

	/*temp = V->Clone();
	Transpose(temp,V);
	MATOP_DELETE_MAT(temp);*/
	if(U!=0)
	{
		/*temp = U->Clone();
		Transpose(temp,U);
		MATOP_DELETE_MAT(temp);*/
		Transpose(m,U);
	}

	double anorm = 0;
	for( int i = 0; i < n-1; i++ )
	{
		double tnorm = fabs(D[i]);
		tnorm += fabs(F[i]);
		if( anorm < tnorm )
			anorm = tnorm;
	}
	if(anorm < fabs(D[n-1]))
		anorm = fabs(D[n-1]);

	anorm *= DBL_EPSILON;
	double my_eps2, my_eps3;
	my_eps2 = my_eps3 = anorm;
	/*double my_eps2, my_eps3;
	my_eps2 = 1e-50;
	my_eps3 = 1e-10;*/

	// 2.Repeat:
	/*while(1)*/
	//
	int count = 0;
	//for( count = 0; count < 64; count++ )
	while(1)
	{
		count++;
		//   b. Determine the smallest p and the largest q so that B can be
		//   blocked as
		//       [B11    0       0  ]    p
		//   B=  [0      B22     0  ]    n-p-q
		//       [0      0       B33]    q
		//   where B33 is diagonal and B22 has no zero superdiagonal entry. %??
		//   if B33's tail has 0 then error
		/*	q = 0;
		for i = n:-1:2
		if( abs(B(i-1,i)) > my_eps3*(B(i-1,i-1)+B(i,i)))%if (abs(B(i-1,i))>my_eps3)%if (B(i-1,i)~=0)
		break;
		end
		q = q + 1;
		end
		if (i == 2 && abs(B(1,2))<=my_eps3*(B(1,1)+B(2,2)))%if (i == 2 && abs(B(1,2))<=my_eps3)%if (i == 2 && B(1,2)==0)
		q = q + 1;
		end
		re = 0;
		ni = i;
		for i = ni:-1:2
		if( abs(B(i-1,i)) <= my_eps3*(B(i-1,i-1)+B(i,i)))%if (abs(B(i-1,i))<=my_eps3)%if (B(i-1,i)==0)
		if (re==0)
		re = -1;
		end
		break;
		end
		re = re + 1;
		end
		if(i>=2 && (n>i || abs(B(i-1,i)) > my_eps3*(B(i-1,i-1)+B(i,i))))%if(i>=2 && (n>i || abs(B(i-1,i))>my_eps3))%if(i>=2 && (n>i || B(i-1,i)~=0))
		re = re + 1;
		end
		p = n - re - q;*/
		int q = 0;
		int i;
		for( i = n-1; i >= 1; i--)
		{
			if(fabs(F[i-1])>my_eps3)//if(abs(F->data[i-1]) > my_eps3* (D->data[i-1]+D->data[i]))//if(abs(B->data[(i-1)*n+i])>my_eps3*(B->data[(i-1)*n+i-1]+B->data[i*n+i]))
			{
				break;
			}
			q++;
		}
		if(i==1 && fabs(F[i-1])<=my_eps3)//if(i==1 && abs(F->data[i-1]) <= my_eps3* (D->data[i-1]+D->data[i]))//if(i==1 && abs(B->data[1]) <= my_eps3*(B->data[0]+B->data[n+1]))
			q++;
		int re = 0;
		int ni = i;
		for( i = ni; i >= 1; i-- )
		{
			if(fabs(F[i-1])<=my_eps3)//if( abs(F->data[i-1]) <= my_eps3* (D->data[i-1]+D->data[i]))//if(abs(B->data[(i-1)*n+i])<= my_eps3*(B->data[(i-1)*n+i-1]+B->data[i*n+i]))
			{
				if(re == 0)
					re = -1;
				break;
			}
			re++;
		}
		if( i == 0 )
			i = 1;
		if( (i>0) && ( (n-1>i) || fabs(F[i-1])>my_eps3 ) )//if( (i>0) && ( (n-1>i) || abs(F->data[i-1]) > my_eps3* (D->data[i-1]+D->data[i]) ) )//if( (i>0) && ( (n-1>i) || abs(B->data[(i-1)*n+i])>my_eps3*(B->data[(i-1)*n+i-1]+B->data[i*n+i]) ) )
			re++;
		int p = n - re - q;

		//   c. If q=n, set S = the diagonal portion of B STOP.
		/*		%S = diag(diag(B));
		[B,U,V] = Sort_SVD(n,B,U,V);
		S = B;
		return*/
		if(q==n-1)
		{
			break;
		}
		//  d. If for i = p+1,...,n-q-1, b[i,i]=0, then
		/*for i = p+1:n-q-1
		if(abs(B(i,i))<my_eps2)*/
		bool bExist = 0;
		for( int i = p; i < n - q - 1; i++ )
		{
			if(D[i] < my_eps2)//if(abs(B->data[i*n+i])<my_eps2)//if(B->data[i*n+i]==0)
			{
				bExist = 1;
				//       Apply Givens rotations so that b[i,i+1] = 0 and B22 is still
				//       upper bidiagonal.
				/*for k = i:n-q-1
				G = givens(B(k+1,k+1),B(i,k+1));
				R = eye(m);
				c = G(1,1);
				s = G(1,2);

				R(i,i) = c;
				R(k+1,k+1) = c;
				R(i,k+1) = -s;
				R(k+1,i) = s;

				B = R * B;
				U = U * R;
				end
				%		B(i,i) = 0;*/
				double tempMove = F[i], tempDiag;
				F[i] = 0;
				for( int k = i; k < n - q - 1; k++ )
				{
					if(my_eps3+fabs(tempMove) == my_eps3)//if(abs(tempMove)<my_eps3)
						break;
					double c, s;
					Givens( D[k+1], tempMove, &c, &s );

					tempDiag = D[k+1];
					D[k+1] = s * tempMove + c * tempDiag;
					tempMove = c * tempMove - s * tempDiag;

					if(U!=0)
						LeftGivensRot(U+i*m, U+k*m+m, c, -s, m );
				}
			}
		}
		if(bExist==0)
		{
			//       Apply Algorithm 1c to n,B,U,V,p,q
			/*[B,U,V] = algorithm1c(n,B,U,V,p,q);*/

			/*
			function [B,Q,P] = algorithm1c(n,B,Q,P,p,q)
			% Algorithm 1c: Golub-Kahan SVD step:
			% Input: n,B,Q,P,p,q where B is n*n and upper bidiagonal, Q and P have
			% orthogonal columns, and A=QBP'.
			% Output: B,Q,P so that B is upper bidiagonal, A=QBP', Q and P have
			% orthogonal columns, and the output B has smaller off-diagonal
			% elements than the input B. In storage, B,Q, and P are overwritten.*/

			//% 1. Let B22 be the diagonal block of B with row and column indices
			//% p+1,...,n-q.
			//% 2, Set C=lower,right 2*2 submatrix of B22'B22.
			/*
			dm = B(n-q-1,n-q-1);
			dn = B(n-q,n-q);
			fn = B(n-q-1,n-q);
			fm = 0;
			if n-q-2~=0
			fm = B(n-q-2,n-q-1);
			end
			C(1,1) = dm*dm+fm*fm;
			C(1,2) = dm*fn;
			C(2,1) = dm*fn;
			C(2,2) = fn*fn + dn*dn;*/
			double c11, c12, c21, c22, dm, dn, fn, fm;
			dm = D[n-q-2];
			dn = D[n-q-1];
			fn = F[n-q-2];
			fm = 0;
			if(n-q-3>=0)
				fm = F[n-q-3];
			c11 = dm*dm+fm*fm;
			c12 = dm*fn;
			c21 = dm*fn;
			c22 = fn*fn+dn*dn;
			/*
			% 3. Obtain eigenvalues rambda1, rambda2 of C. Set miu = whichever of
			% rambdas that is closer to c2,2.
			%rambdas = eig(C);*/
			/*	d = (C(1,1)-C(2,2))/2;
			if(d<0)
			miu = C(2,2) + d + sqrt(d*d+C(1,2)*C(1,2));
			else
			miu = C(2,2) + d - sqrt(d*d+C(1,2)*C(1,2));
			end*/
			double d = (c11-c22) / 2;
			double miu;
			if(d<0)
				miu=c22+d+sqrt(d*d+c12*c12);
			else
				miu=c22+d-sqrt(d*d+c12*c12);

			//% 4. k=p+1, alpha = bkk^2-miu, beta = b[k,k]b[k,k+1].
			/*k = p+1; alpha = B(k,k)*B(k,k)-miu; beta = B(k,k)*B(k,k+1);*/
			int k = p;
			double alpha = D[k]*D[k] - miu;
			double beta =  D[k]*F[k];

			double tempMove = 0;
			double temp1, temp2;
			//% 5. for k=p+1,...,n-q-1
			/*for k = p+1:n-q-1*/
			for( k = p; k < n-q-1; k++)
			{
				/*
				%   a. Determine c = cos(theta) and s = sin(theta) with the property
				%   that:
				%       [alpha beta][c  s] = [sqrt(alpha^2+beta^2) 0].
				%                   [-s c]
				G = givens(alpha,beta);
				c = G(1,1); s = G(2,1);*/
				double c, s;
				Givens(alpha, beta, &c, &s);
				s = -s;
				/*
				%   b. B <- BR(k,k+1, c,s) where R(k,k+1, c,s) is the Given rotation
				%   matrix that acts on columns k and k+1 during right multiplication.
				R = eye(n);
				R(k,k) = c; R(k,k+1) = s;
				R(k+1,k) = -s; R(k+1,k+1) = c;
				B = B * R;*/
				if(tempMove != 0)
				{
					F[k-1] = F[k-1] * c - tempMove * s;
				}
				temp1 = D[k];
				temp2 = F[k];
				D[k] = temp1 * c - temp2 * s;
				F[k] = temp1 * s + temp2 * c;
				tempMove = - D[k+1] * s;
				D[k+1] = D[k+1] * c;

				/*
				%   c. P <- PR(k,k+1, c,s).
				P = P * R;*/
				if(V!=0)
					//RightGivensRot(V, c, s, k, k+1, n, n);
					LeftGivensRot(V+k*n, V+k*n+n, c, -s, n );

				/*
				%   d. alpha = bkk, beta = b[k+1,k].
				alpha = B(k,k); beta = B(k+1,k);*/
				alpha = D[k];
				beta = tempMove;
				/*
				%   e. Determine c,s with the property that:
				%       [c  -s][alpha] = [sqrt(alpha^2+beta^2)]
				%       [s   c][beta ]   [  0                 ].
				G = givens(alpha, beta);
				c = G(1,1); s = -G(1,2);*/
				Givens(alpha, beta, &c, &s);
				s = -s;
				/*
				%   f. B <- R(k,k+1, c,-s)B, where R(k,k+1, c,-s) is the Givens
				%   rotation matrix that acts on rows k and k+1 during left
				%   multiplication.
				R = eye(m);
				R(k,k) = c; R(k,k+1) = -s;
				R(k+1,k) = s; R(k+1,k+1) = c;
				B = R * B;*/

				D[k] = c * D[k] - tempMove * s;
				temp1 = F[k];
				temp2 = D[k+1];
				F[k] = temp1 * c - temp2 * s;
				D[k+1] = temp1 * s + temp2 * c;
				if(k<n-q-2)
				{
					tempMove = -s * F[k+1];
					F[k+1] = c * F[k+1];
				}					

				/*
				%   g. Q <- QR(k,k+1, c,s).
				R(k,k+1) = s;
				R(k+1,k) = -s;
				Q = Q * R;*/
				if(U!=0)
					//RightGivensRot(U, c, s, k, k+1, m, m);
					LeftGivensRot(U+k*m, U+k*m+m, c, -s, m );

				/*
				%   h. if k<=n-q-1 alpha = b[k,k+1], beta = b[k,k+2].
				if (k<=n-q-2)   %??
				alpha = B(k,k+1);
				beta = B(k,k+2);
				end
				end*/
				if( k < n-q - 2 )
				{
					alpha = F[k];
					beta = tempMove;
				}
				/*end
				*/
			}

			/*end
			end
			end*/

			if(D[n-q-1]<0)
			{
				D[n-q-1] = - D[n-q-1];
				F[n-q-2] = - F[n-q-2];
				if(V!=0)
					for( int i = 0; i < n; i++ )
						V[i*n+(n-q-1)] = - V[i*n+(n-q-1)];
			}
		}
	}
	/*end*/

	//Sort_SVD(m,n,B,U,V);
	if(V!=0)
	{
		/*temp = V->Clone();
		Transpose(temp,V);
		MATOP_DELETE_MAT(temp);*/
		Transpose(n,V);
	}
	if(U!=0)
	{
		/*temp = U->Clone();
		Transpose(temp,U);
		MATOP_DELETE_MAT(temp);*/
		Transpose(m,U);
	}
	Sort_SVD(m,n,D,F,B,U,V);

	// if matU,matV is invalid, delete U and V

	/*MATOP_DELETE_MAT(D);
	MATOP_DELETE_MAT(F);*/

	delete [] arrD;
	delete [] arrF;
	return count;
}

  void SVBkSb( int m, int n, int k, const double * W, const double * U, const double * V, const double * B,
			double * X, int flags )
{
#ifdef MATHLIB_WITH_OPENCV
	CvMat mW, mU, mV, mB, mX;
	mW = cvMat(m, n, CV_64FC1, (void*)W);
	mU = cvMat(m, m, CV_64FC1, (void*)U);
	mV = cvMat(n, n, CV_64FC1, (void*)V);
	mB = cvMat(m, k, CV_64FC1, (void*)B);
	mX = cvMat(n, k, CV_64FC1, (void*)X);
	cvSVBkSb(&mW, &mU, &mV, &mB, &mX, flags);
#else
	double threshold = 0;

	for( int i = 0; i < min(m,n); i++ )
		threshold += W[i*n+i];
	threshold *= 2*DBL_EPSILON;

	//double * vw = new double[n*n];
	int last = n;
	double * UtB = new double[n*k];
	double wi;
	memset(UtB, 0, sizeof(double) * n * k);

	for( int i = 0; i < min(m,n); i++ )
	{
		wi = W[i*n+i];
		if(wi>threshold)
		{
			wi = 1. / wi;
			for( int j = 0; j < k; j++ )
			{
				//UtB[i*k+j] = 0;
				for( int l = 0; l < m; l++ )
					UtB[i*k+j] += U[l*m+i]*B[l*k+j];
				UtB[i*k+j] *= wi;
			}
		}
		else
		{
			last = i;
			break;
		}
	}

	for( int i = 0; i < n; i++ )
	{
		for( int j = 0; j < k; j++ )
		{
			X[i*k+j] = 0;
			for( int l = 0; l < last; l++ )
				X[i*k+j] += V[i*n+l]*UtB[l*k+j];
		}
	}
	delete [] UtB;
#endif
}

  void Solve( int m, int n, int k, double * A, double * b, double * X)
{
#ifdef MATHLIB_WITH_OPENCV
	CvMat matA = cvMat(m,n, CV_64FC1, A);
	CvMat matB = cvMat(m,k, CV_64FC1, b);
	CvMat matX = cvMat(n,k, CV_64FC1, X);
	cvSolve( &matA, &matB, &matX, CV_SVD );
#else
	double * matS = new double[m*n];
	double * matU = new double[m*m];
	double * matV = new double[n*n];
	SVD(m, n, A, matS, matU, matV);
	SVBkSb(m, n, k, matS, matU, matV, b, X);
	delete [] matS;
	delete [] matU;
	delete [] matV;
#endif
}

  void SolveHomo( int m, int n, double * A, double * X )
{
	double * S = new double[m*n];
	double * V = new double[n*n];
	SVD(m, n, A, S, 0, V);
	for( int i = 0; i < n; i++ )
		X[i] = V[i*n + n-1];
	delete [] S;
	delete [] V;
}
#pragma endregion

#pragma region Matrix Decomposition
  void LUDecompose( int m, const double * A, double * L, double * U, double * D)
{
	for( int i = 0; i < m * m; i++ )
	{
		L[i] = U[i] = 0;
		D[i] = A[i];
	}
	for( int i = 0; i < m; i++ )
	{
		double aii = D[i*m+i];
		for( int j = i; j < m; j++ )
			L[j*m+i] = D[j*m+i] / aii;
		for( int j = i; j < m; j++ )
			U[i*m+j] = D[i*m+j];
		for( int j = i; j < m; j++ )
			D[i*m+j] = D[j*m+i] = 0;
		for( int j = i+1; j<m; j++)
			for( int k = i+1; k < m; k++ )
				D[j*m+k] = D[j*m+k] - L[j*m+i]*U[i*m+k];
	}
	for( int i = 0; i < m; i++ )
	{
		D[i*m+i] = U[i*m+i];
		for( int j = i; j < m; j++)
			U[i*m+j] /= D[i*m+i];
	}
}

  bool CholDecompose( int m, const double * A, double * R, bool bTranspose)
{
	double * L = new double[m*m];
	double * D = new double[m*m];
	double * U = new double[m*m];
	LUDecompose(m,A,L,U,D);
	for( int i = 0; i < m; i++ )
		if(D[i*m+i]<0)
		{
			delete [] L;
			delete [] D;
			delete [] U;
			return false;
		}
	for( int i = 0; i < m; i++ )
		for( int j = 0; j < m; j++)
		{
			if(bTranspose)
				R[i*m+j] = L[i*m+j] * sqrt(D[j*m+j]);
			else
				R[i*m+j] = U[i*m+j] * sqrt(D[i*m+i]);
		}
	delete [] L;
	delete [] D;
	delete [] U;
	return true;
}
#pragma endregion

#pragma region Rodrigues Convert
  inline void Rodrigues2( const double * src, double * dst, bool bRodrigues, double * J )
{
	if(bRodrigues) // vector to matrix
	{
		// theta <- norm(r)
		// r <- r / theta
		// R = cos(theta)I + (1-cos(theta))rr' + sin(theta) * vecmul(r)
		//				[0		-x3		x2	]
		// vecmul(x) =	[x3		0		-x1	]		note as [r]x
		//				[-x2	x1		0	]
		/* Jonghh
		double theta = Norm(3, src, 0);
		if(theta == 0)
		{
			dst[1] = dst[2] = dst[3] = dst[5] = dst[6] = dst[7] = 0;
			dst[0] = dst[4] = dst[8] = 1;
		}
		else
		{
			double cost = cos(theta);
			double sint = sin(theta);
			double r[3];
			for(int i=0;i<3;i++)
				r[i] = src[i] / theta;			

			dst[0] = 1 * cost + (1-cost) * r[0]*r[0] + sint * 0;
			dst[1] = 0 * cost + (1-cost) * r[0]*r[1] + sint * (-r[2]);
			dst[2] = 0 * cost + (1-cost) * r[0]*r[2] + sint * r[1];

			dst[3] = 0 * cost + (1-cost) * r[1]*r[0] + sint * r[2];
			dst[4] = 1 * cost + (1-cost) * r[1]*r[1] + sint * 0;
			dst[5] = 0 * cost + (1-cost) * r[1]*r[2] + sint * (-r[0]);

			dst[6] = 0 * cost + (1-cost) * r[2]*r[0] + sint * (-r[1]);
			dst[7] = 0 * cost + (1-cost) * r[2]*r[1] + sint * r[0];
			dst[8] = 1 * cost + (1-cost) * r[2]*r[2] + sint * 0;
		}*/
		/*
		 Jacobian by Jonghh.
			double theta = Norm(3, r);
			double alpha = cos(theta), beta = sin(theta);
			double x_, y_, z_;
			x_ = r[0] / theta;
			y_ = r[1] / theta;
			z_ = r[2] / theta;
			double dthe_r[3] = {x_,y_,z_};
			double dx_r[3], dy_r[3], dz_r[3];
			{
				dx_r[0] = (theta-r[0]*dthe_r[0])/theta/theta;
				dx_r[1] = (		-r[0]*dthe_r[1])/theta/theta;
				dx_r[2] = (		-r[0]*dthe_r[2])/theta/theta;

				dy_r[0] = (		-r[1]*dthe_r[0])/theta/theta;
				dy_r[1] = (theta-r[1]*dthe_r[1])/theta/theta;
				dy_r[2] = (		-r[1]*dthe_r[2])/theta/theta;

				dz_r[0] = (		-r[2]*dthe_r[0])/theta/theta;
				dz_r[1] = (		-r[2]*dthe_r[1])/theta/theta;
				dz_r[2] = (theta-r[2]*dthe_r[2])/theta/theta;
			}
			double dal_r[3], dbe_r[3];
			for( i = 0; i < 3; i++ )
			{
				dal_r[i] = -beta * dthe_r[i];
				dbe_r[i] = alpha * dthe_r[i];
			}
			double dR_r[9*3];
			for( i = 0; i < 3; i++ )
			{
				dR_r[0*3+i] = dal_r[i] - dal_r[i]*x_*x_ + 2*(1-alpha)*x_*dx_r[i];
				dR_r[1*3+i] = -dal_r[i]*x_*y_ + (1-alpha)*(dx_r[i]*y_+x_*dy_r[i])-dbe_r[i]*z_-beta*dz_r[i];
				dR_r[2*3+i] = -dal_r[i]*x_*z_ + (1-alpha)*(dx_r[i]*z_+x_*dz_r[i])+dbe_r[i]*y_+beta*dy_r[i];

				dR_r[3*3+i] = -dal_r[i]*x_*y_ + (1-alpha)*(dx_r[i]*y_+x_*dy_r[i])+dbe_r[i]*z_+beta*dz_r[i];
				dR_r[4*3+i] = dal_r[i] - dal_r[i]*y_*y_ + 2*(1-alpha)*y_*dy_r[i];
				dR_r[5*3+i] = -dal_r[i]*y_*z_ + (1-alpha)*(dy_r[i]*z_+y_*dz_r[i])-dbe_r[i]*x_-beta*dx_r[i];

				dR_r[6*3+i] = -dal_r[i]*x_*z_ + (1-alpha)*(dx_r[i]*z_+x_*dz_r[i])-dbe_r[i]*y_-beta*dy_r[i];
				dR_r[7*3+i] = -dal_r[i]*y_*z_ + (1-alpha)*(dy_r[i]*z_+y_*dz_r[i])+dbe_r[i]*x_+beta*dx_r[i];
				dR_r[8*3+i] = dal_r[i] - dal_r[i]*z_*z_ + 2*(1-alpha)*z_*dz_r[i];
			}
		*/
		{ // Jacobian with OpenCV - 2010/8/9
			int i,k;
			double rx = src[0], ry = src[1], rz = src[2];
			double theta = sqrt(rx*rx + ry*ry + rz*rz);

			if( theta < DBL_EPSILON )
			{
				dst[1] = dst[2] = dst[3] = dst[5] = dst[6] = dst[7] = 0;
				dst[0] = dst[4] = dst[8] = 1;

				if( J )
				{
					memset( J, 0, sizeof(double)*27 );
					//J[5] = J[15] = J[19] = -1;
					//J[7] = J[11] = J[21] = 1;
					J[5*3+0] = J[6*3+1] = J[1*3+2] = -1;
					J[7*3+0] = J[2*3+1] = J[3*3+2] = 1;
				}
			}
			else
			{
				const double I[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

				double c = cos(theta);
				double s = sin(theta);
				double c1 = 1. - c;
				double itheta = theta ? 1./theta : 0.;

				rx *= itheta; ry *= itheta; rz *= itheta;

				double rrt[] = { rx*rx, rx*ry, rx*rz, rx*ry, ry*ry, ry*rz, rx*rz, ry*rz, rz*rz };
				double _r_x_[] = { 0, -rz, ry, rz, 0, -rx, -ry, rx, 0 };

				// R = cos(theta)*I + (1 - cos(theta))*r*rT + sin(theta)*[r_x]
				// where [r_x] is [0 -rz ry; rz 0 -rx; -ry rx 0]
				for( k = 0; k < 9; k++ )
					dst[k] = c*I[k] + c1*rrt[k] + s*_r_x_[k];

				if( J )
				{
					double drrt[] = { rx+rx, ry, rz, ry, 0, 0, rz, 0, 0,
						0, rx, 0, rx, ry+ry, rz, 0, rz, 0,
						0, 0, rx, 0, 0, ry, rx, ry, rz+rz };
					double d_r_x_[] = { 0, 0, 0, 0, 0, -1, 0, 1, 0,
						0, 0, 1, 0, 0, 0, -1, 0, 0,
						0, -1, 0, 1, 0, 0, 0, 0, 0 };
					for( i = 0; i < 3; i++ )
					{
						double ri = i == 0 ? rx : i == 1 ? ry : rz;
						double a0 = -s*ri, a1 = (s - 2*c1*itheta)*ri, a2 = c1*itheta;
						double a3 = (c - s*itheta)*ri, a4 = s*itheta;
						for( k = 0; k < 9; k++ )
							J[k*3+i] = a0*I[k] + a1*rrt[k] + a2*drrt[i*9+k] +
							a3*_r_x_[k] + a4*d_r_x_[i*9+k];
					}
				}
			}
		}
		
	}
	else	// matrix to vector
	{
		//sin(theta)*vecmul(r) = (R-R')/2
		// theta = cos-1( (r11+r22+r33-1)/2 )
		// r = 1/(2*sin(theta)) * [r32-r23; r13-r31; r21-r12];
		double r[3];
		r[0] = -(src[1*3+2]-src[2*3+1]) / 2;
		r[1] = (src[2]-src[2*3]) / 2;
		r[2] = -(src[1]-src[1*3]) / 2;
		double sint = Norm(3,r,0);
		
		double theta = 0;
		if(sint<DBL_EPSILON)
		{
			dst[0] = dst[1] = dst[2] = 0;
		}
		else
		{
			for( int i = 0; i < 3; i++ )
				r[i] /= sint;
			if(abs(r[0]-1)<DBL_EPSILON)
				theta = 1;
			else
			{
				double cost = (src[0] - r[0]*r[0]) / (1 - r[0]*r[0]);
				if(cost>0)
					theta = asin(sint);
				else
					theta = PI - asin(sint);
			}
			
			for( int i = 0; i < 3; i++ )
				dst[i] = r[i] * theta;
			/*
			theta = acos((src[0*3+0]+src[1*3+1]+src[2*3+2]-1)/2);

			*/
		}
	}
}

  void getRotationJacobianByRotationAngle(double * rotation, double * jacobian)
{
	double rx = rotation[0], ry = rotation[1], rz = rotation[2];
	double theta = sqrt(rx*rx + ry*ry + rz*rz);

	double c, s, c1;
	if( theta < DBL_EPSILON )
	{
		c = 1; s = 0;
		rx = ry = rz = 1;
	}
	else
	{
		c = cos(theta);
		s = sin(theta);
		
		double itheta = theta ? 1./theta : 0.;
		
		rx *= itheta; ry *= itheta; rz *= itheta;
	}
	const double I[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
	c1 = 1. - c;
		

	double d_X[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
	double d_B[] = {0, -1, 1, 1, 0, -1, -1, 1, 0};
	double r_x[] = {rx, ry, rz};
	double dInverse[] = { 0, rz, ry, rz, 0, rx, ry, rx, 0 };

	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 3; j++ )
		{
			double a0 = (i==j) ? -s : 0;
			jacobian[i*3+j] = s * (r_x[i] * r_x[j]) + a0 +
				c * dInverse[i*3+j] * d_B[i*3+j];
		}
}

  int CorrectRotationMatrix( double * rot )
{
	int nCorrect  = 0;
	double u[9], s[9], v[9];
	SVD(3, 3, rot, s, u, v);
	s[0] = s[4] = s[8] = 1;
	s[1] = s[2] = s[3] = s[5] = s[6] = s[7] = 0;
	double temp[9];
	double du = Det(u);
	double dv = Det(v);
	if(du*dv<0)
	{
		s[4] = -1;
		//blCorrect = false;
		if(du < 0)
			nCorrect = 1;
		else
			nCorrect = 2;
	}
	GEMM(3,3,3, u, s, 1, 0, 0, temp, 0);
	GEMM(3,3,3,temp,v,1,0,0, rot, MATOP_GEMM_B_T);
	return nCorrect;
}
  void CorrectRotationMatrix2D( double * rot )
{
	double u[4], s[4], v[4];
	SVD(2, 2, rot, s, u, v);
	s[0] = s[3] = 1;
	s[1] = s[2] = 0;
	double temp[4];
	if(Det(2, u)*Det(2, v)<0)
	{
		s[3] = -1;
	}
	GEMM(2,2,2, u, s, 1, 0, 0, temp, 0);
	GEMM(2,2,2,temp,v,1,0,0, rot, MATOP_GEMM_B_T);
}
  void ConvertRotationMatrix2EulerAngles(double * rot, double * angles)
{
	/*double * alpha = &angles[0];
	double * beta = &angles[1];
	double * gamma = &angles[2];

	double r11 = rot[0*3+0];
	double r21 = rot[1*3+0];
	double r31 = rot[2*3+0];
	double r32 = rot[2*3+1];
	double r33 = rot[2*3+2];

	*beta = atan2(-r31, sqrt(r11*r11+r21*r21));
	double cosb = cos(*beta);
	*alpha = atan2(r21/cosb, r11/cosb);
	*gamma = atan2(r32/cosb, r33/cosb);*/
	double * alpha = &angles[0];
	double * gamma = &angles[1];
	double * beta = &angles[2];

	double r12 = rot[0*3+1];
	double r22 = rot[1*3+1];
	double r32 = rot[2*3+1];
	double r31 = rot[2*3+0];
	double r33 = rot[2*3+2];

	*gamma = atan2(r32, sqrt(r12*r12+r22*r22));
	double cosr = cos(*gamma);
	*alpha = atan2(-r12/cosr, r22/cosr);
	*beta = atan2(-r31/cosr, r33/cosr);
}

  void ConvertRotationMatrix2XYZEulerAngles(double * rot, double * angles)
{
	/*double * alpha = &angles[0];
	double * beta = &angles[1];
	double * gamma = &angles[2];

	double r11 = rot[0*3+0];
	double r21 = rot[1*3+0];
	double r31 = rot[2*3+0];
	double r32 = rot[2*3+1];
	double r33 = rot[2*3+2];

	*beta = atan2(-r31, sqrt(r11*r11+r21*r21));
	double cosb = cos(*beta);
	*alpha = atan2(r21/cosb, r11/cosb);
	*gamma = atan2(r32/cosb, r33/cosb);*/
	double * alpha = &angles[0];
	double * beta = &angles[1];
	double * gamma = &angles[2];

	double r11 = rot[0*3+0];
	double r12 = rot[0*3+1];
	double r13 = rot[0*3+2];
	double r23 = rot[1*3+2];
	double r33 = rot[2*3+2];

	*beta = atan2(r13, sqrt(r11*r11+r12*r12));
	double cosb = cos(*beta);
	//*alpha = atan2(-r23/cosb, r33/cosb);
	//*gamma = atan2(-r12/cosb, r11/cosb);
	*alpha = atan2(-r23 / cosb, r33 / cosb);
	*gamma = atan2(-r12 / cosb, r11 / cosb);
}

  void ConvertRotationVector2XYZEulerAngles(double * vect, double * angles, double * Jacobian)
{
	double * alpha = &angles[0];
	double * beta = &angles[1];
	double * gamma = &angles[2];

	double rot[9], J[27];
	if(Jacobian)
		Rodrigues2(vect, rot, true, J);
	else
		Rodrigues2(vect, rot, true);

	double r11 = rot[0*3+0];
	double r12 = rot[0*3+1];
	double r13 = rot[0*3+2];
	double r23 = rot[1*3+2];
	double r33 = rot[2*3+2];

	*beta = atan2(r13, sqrt(r11*r11+r12*r12));
	double cosb = cos(*beta);
	//*alpha = atan2(-r23/cosb, r33/cosb);
	//*gamma = atan2(-r12/cosb, r11/cosb);
	*alpha = atan2(-r23, r33);
	*gamma = atan2(-r12, r11);

	if(Jacobian)
	{
		// alpha
		{
			double top_val = -r23;
			double bottom_val = r33;
			double atan_val = top_val / bottom_val;
			for( int i = 0; i < 3; i++ )
			{
				double r23_ = -J[(1*3+2)*3 + i];
				double r33_ =  J[(2*3+2)*3 + i];
				Jacobian[0*3+i] = 1. / (atan_val * atan_val + 1) * (r23_ * bottom_val - top_val * r33_) / (bottom_val * bottom_val);
			}
		}
		// beta
		{
			double top_val = r13;
			double bottom_val = sqrt(r11*r11 + r12*r12);
			double atan_val = top_val / bottom_val;
			for( int i = 0; i < 3; i++ )
			{
				double r13_ = J[(0*3+2)*3 + i];
				double r11_ = J[(0*3+0)*3 + i];
				double r12_ = J[(0*3+1)*3 + i];
				double bottom_ = 1./2 / bottom_val * (2*r11*r11_ + 2*r12*r12_);
				Jacobian[1*3+i] = 1. / (atan_val * atan_val + 1) * (r13_ * bottom_val - top_val * bottom_) / (bottom_val * bottom_val);
			}
		}
		// gamma
		{
			double top_val = -r12;
			double bottom_val = r11;
			double atan_val = top_val / bottom_val;
			for( int i = 0; i < 3; i++ )
			{
				double r12_ = -J[(0*3+1)*3 + i];
				double r11_ =  J[(0*3+0)*3 + i];
				Jacobian[2*3+i] = 1. / (atan_val * atan_val + 1) * (r12_ * bottom_val - top_val * r11_) / (bottom_val * bottom_val);
			}
		}
	}
}

  void ConvertEulerAngles2RotationMatrix(double * angles, double * rot)
{
	double alpha = angles[0];
	double beta = angles[2];
	double gamma = angles[1];
	double ca = cos(alpha);
	double sa = sin(alpha);
	double cb = cos(beta);
	double sb = sin(beta);
	double cr = cos(gamma);
	double sr = sin(gamma);

	rot[0*3+0] = ca*cb-sr*sa*sb;
	rot[0*3+1] = -sa*cr;
	rot[0*3+2] = ca*sb+sr*sa*cb;
	rot[1*3+0] = sa*cb+sr*ca*sb;
	rot[1*3+1] = ca*cr;
	rot[1*3+2] = sa*sb-sr*ca*cb;
	rot[2*3+0] = -cr*sb;
	rot[2*3+1] = sr;
	rot[2*3+2] = cr*cb;
}
  void ConvertXYZEulerAngles2RotationMatrix(double * angles, double * rot)
{
	double alpha = angles[0];
	double beta = angles[1];
	double gamma = angles[2];
	double ca = cos(alpha);
	double sa = sin(alpha);
	double cb = cos(beta);
	double sb = sin(beta);
	double cr = cos(gamma);
	double sr = sin(gamma);

	rot[0*3+0] = cb*cr;
	rot[0*3+1] = -cb*sr;
	rot[0*3+2] = sb;
	rot[1*3+0] = ca*sr+cr*sa*sb;
	rot[1*3+1] = ca*cr-sa*sb*sr;
	rot[1*3+2] = -cb*sa;
	rot[2*3+0] = sa*sr-ca*sb*cr;
	rot[2*3+1] = cr*sa+ca*sb*sr;
	rot[2*3+2] = ca*cb;
}

  inline void ConvertRotVect2RotAxisAngle(double * pRotVect, double * pRotAxisAngle)
{
	pRotAxisAngle[3] = Norm(3, pRotVect);
	for(int k = 0; k < 3; k++)
	{
		if(pRotAxisAngle[3] == 0.0f)
			pRotAxisAngle[k] = 0.0f;
		else
			pRotAxisAngle[k] = pRotVect[k] / pRotAxisAngle[3];
	}
}

  void GetAxisRotation(char chAxis, double angle, double * rot)
{
	double ca = cos(angle);
	double sa = sin(angle);
	memset(rot, 0, sizeof(double) * 9);
	switch(chAxis)
	{
	case 'Z':
		rot[0*3 + 0] = ca;
		rot[1*3 + 1] = ca;
		rot[0*3 + 1] = -sa;
		rot[1*3 + 0] = sa;
		rot[2*3 + 2] = 1;
		break;
	case 'X':
		rot[1*3 + 1] = ca;
		rot[2*3 + 2] = ca;
		rot[1*3 + 2] = -sa;
		rot[2*3 + 1] = sa;
		rot[0*3 + 0] = 1;
		break;
	case 'Y':
		rot[0*3 + 0] = ca;
		rot[2*3 + 2] = ca;
		rot[0*3 + 2] = sa;
		rot[2*3 + 0] = -sa;
		rot[1*3 + 1] = 1;
		break;
	default:
		rot[0*3 + 0] = rot[1*3 + 1] = rot[2*3 + 2] = 1;
		break;
	}
}

//  void DecomposeProjMat( double * P, double * K, double * R, double * T )
//{
//	CvMat matP = cvMat(3, 4, CV_64FC1, P);
//	CvMat matK = cvMat(3, 3, CV_64FC1, K);
//	CvMat matR = cvMat(3, 3, CV_64FC1, R);
//	double tempT[4];
//	CvMat matT = cvMat(4, 1, CV_64FC1, tempT);
//	cvDecomposeProjectionMatrix(&matP, &matK, &matR, &matT);
//	for( int i = 0; i < 3; i++ )
//		T[i] = tempT[i] / tempT[3];
//}
#pragma endregion

#pragma region LevMarq Section
CLevMarq::CLevMarq()
{
	mask = 0;
	prevParam = param = JtJ = JtJN = JtErr = JtJV = JtJW = 0;
	lambdaLg10 = 0; state = DONE;
	
	max_iter = 0;
	epsilon = 0;
	iters = 0;
}

CLevMarq::~CLevMarq()
{
	clear();
}

void CLevMarq::init(int _nparams, int _max_iter, double _epsilon)
{
	nparams = _nparams;
	max_iter = min(max(_max_iter,1),1000);
	epsilon = max(_epsilon,0);

	mask = new bool[nparams*1];
	//cvSet(mask, cvScalarAll(1));
	for( int i = 0; i < nparams; i++ )
		mask[i] = true;
	prevParam = new double[nparams*1];
	param = new double[nparams*1];
	JtJ = new double[nparams*nparams];
	JtJN = new double[nparams*nparams];
	JtJV = new double[nparams*nparams];
	JtJW = new double[nparams*nparams];
	JtErr = new double[nparams*1];

	prevErrNorm = DBL_MAX;
	lambdaLg10 = -3;
	
	state = STARTED;
	iters = 0;
}

// clears m vector with 0
#define dblZero( m, mat )				\
{										\
	memset(mat, 0, m*sizeof(double));	\
}

// copy m vector src to dst
#define dblCopy( m, src, dst )			\
{										\
	memcpy(dst, src, m*sizeof(double));	\
}

// norm(mat1-mat2)/norm(mat2)
// norm(mat) = sqrt(sig(mat[i]^2,i))
inline double cvNorm( int m, double * mat1, double * mat2 )
{
	double * A = mat1;
	double * B = mat2;
	double norm1 = 0, norm2 = 0;
	for( int i = 0; i < m; i++ )
	{
		norm1 += (A[i]-B[i]) * (A[i]-B[i]);
		norm2 += B[i] * B[i];
	}
	return sqrt(norm1 / norm2);
}


bool CLevMarq::update(const double *& _param, double *& _JtJ, double *& _JtErr, double *& _errNorm)
{
	double change;

	if( state == DONE )
	{
		_param = param;
		return false;
	}

	if( state == STARTED )
	{
		_param = param;
		dblZero( nparams * nparams, JtJ );
		dblZero( nparams * 1, JtErr );
		errNorm = 0;
		_JtJ = JtJ;
		_JtErr = JtErr;
		_errNorm = &errNorm;
		state = CALC_J;
		return true;
	}

	if( state == CALC_J )
	{
		dblCopy( nparams * 1, param, prevParam );
		step();
		_param = param;
		prevErrNorm = errNorm;
		errNorm = 0;
		_errNorm = &errNorm;
		state = CHECK_ERR;
		return true;
	}

	assert( state == CHECK_ERR );
	if( errNorm > prevErrNorm )
	{
		lambdaLg10++;
		step();
		_param = param;
		errNorm = 0;
		_errNorm = &errNorm;
		state = CHECK_ERR;
		return true;
	}

	lambdaLg10 = max(lambdaLg10-1, -16);
	if( ++iters >= max_iter ||
		(change = cvNorm(nparams*1, param, prevParam)) < epsilon )
	{
		_param = param;
		state = DONE;
		return false;
	}

	prevErrNorm = errNorm;
	dblZero( nparams * nparams, JtJ );
	dblZero( nparams * 1, JtErr );
	_param = param;
	_JtJ = JtJ;
	_JtErr = JtErr;
	state = CALC_J;
	return true;
}

void CLevMarq::step()
{
	const double LOG10 = log(10.);
	double lambda = exp(lambdaLg10*LOG10);
	int i, j;

	for( i = 0; i < nparams; i++ )
		if( mask[i] == false )
		{
			double *row = JtJ + i*nparams, *col = JtJ + i;
			for( j = 0; j < nparams; j++ )
				row[j] = col[j*nparams] = 0;
			JtErr[i] = 0;
		}

		dblCopy( nparams*nparams, JtJ, JtJN );
		for( i = 0; i < nparams; i++ )
			JtJN[(nparams+1)*i] *= 1. + lambda;
		/*for( i = 0; i < nparams; i++ )
			JtJ[(nparams+1)*i] *= 1. + lambda;*/

		//cvSVD( JtJN, JtJW, 0, JtJV, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T );
		//cvSVBkSb( JtJW, JtJV, JtJV, JtErr, param, CV_SVD_U_T + CV_SVD_V_T );
		SVD(nparams, nparams, JtJN, JtJW, 0, JtJV, MATHLIB_SVD_MODIFY_A + MATHLIB_SVD_U_T + MATHLIB_SVD_V_T);
		SVBkSb(nparams, nparams, 1, JtJW, JtJV, JtJV, JtErr, param, MATHLIB_SVD_MODIFY_A + MATHLIB_SVD_U_T + MATHLIB_SVD_V_T);
//#ifdef MATHLIB_WITH_OPENCV
//		CvMat matJtJN = cvMat(nparams, nparams, CV_64FC1, JtJN);
//		CvMat matJtJW = cvMat(nparams, nparams, CV_64FC1, JtJW);
//		CvMat matJtJV = cvMat(nparams, nparams, CV_64FC1, JtJV);
//		CvMat matJtErr = cvMat(nparams, 1, CV_64FC1, JtErr);
//		CvMat matParam = cvMat(nparams, 1, CV_64FC1, param);
//		cvSVD( &matJtJN, &matJtJW, 0, &matJtJV, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T );
//		cvSVBkSb( &matJtJW, &matJtJV, &matJtJV, &matJtErr, &matParam, CV_SVD_U_T + CV_SVD_V_T );
//#else
//		SVD(nparams, nparams, JtJN, JtJW, 0, JtJV);
//		//SVD(nparams, nparams, JtJ, JtJW, 0, JtJV);
//		SVBkSb(nparams, nparams, 1, JtJW, JtJV, JtJV, JtErr, param);
//#endif
		for( i = 0; i < nparams; i++ )
			param[i] = prevParam[i] - (mask[i] ? param[i] : 0);
}

void CLevMarq::clear()
{
	if(param)
	{
		delete [] mask;
		delete [] prevParam;
		delete [] param;
		delete [] JtJ;
		delete [] JtJN;
		delete [] JtJV;
		delete [] JtJW;
		delete [] JtErr;
	}
}
#pragma endregion

#pragma region Bundle Adjustment Section
CBundleAdjustment::CBundleAdjustment()
{
	mask = 0;
	prevParam = param = JtJU = JtJW = JtJV = JtErr = _JtJUmWViWtS = _JtJUmWViWtU = 0;
	_JtJVi = _JtJWVi = _JtJUmWViWt = _JtErrPM = 0;

	lambdaLg10 = 0; state = DONE;

	max_iter = 0;
	epsilon = 0;
	iters = 0;
}

CBundleAdjustment::~CBundleAdjustment()
{
	clear();
}

void CBundleAdjustment::init( int _nparams1, /*int _nsize1, */int _nsize2,/* int _ncount1,*/ int _ncount2, int _max_iter, double _epsilon)
{
	/*nsize1 = _nsize1;*/
	nsize2 = _nsize2;
	/*ncount1 = _ncount1;*/
	ncount2 = _ncount2;
	nparams1 = _nparams1;/*nsize1*ncount1;*/
	nparams2 = nsize2*ncount2;
	nparams = nparams1+nparams2;
	max_iter = min(max(_max_iter,1),1000);
	epsilon = max(_epsilon,0);

	mask = new bool[nparams*1];
	//cvSet(mask, cvScalarAll(1));
	for( int i = 0; i < nparams; i++ )
		mask[i] = true;
	prevParam = new double[nparams*1];
	param = new double[nparams*1];
	JtJU = new double[nparams1*nparams1];
	JtJW = new double[nparams1*nparams2];
	JtJV = new double[nsize2*nsize2*ncount2];
	_JtJUmWViWtS = new double[nparams1*nparams1];
	_JtJUmWViWtU = new double[nparams1*nparams1];
	JtErr = new double[nparams*1];

	_JtJVi = new double[nsize2*nsize2*ncount2];
	_JtJWVi = new double[nparams1*nparams2];
	_JtJUmWViWt = new double[nparams1*nparams1];
	_JtErrPM = new double[nparams*1];

	prevErrNorm = DBL_MAX;
	lambdaLg10 = -3;

	state = STARTED;
	iters = 0;
}

bool CBundleAdjustment::update(const double *& _param, double *& _JtJU, double *& _JtJW, double *& _JtJV, double *& _JtErr, double *& _errNorm)
{
	double change;

	if( state == DONE )
	{
		_param = param;
		return false;
	}

	if( state == STARTED )
	{
		_param = param;
		dblZero( nparams1 * nparams1, JtJU );
		dblZero( nparams1 * nparams2, JtJW );
		dblZero( nsize2 * nsize2 * ncount2, JtJV );
		dblZero( nparams * 1, JtErr );
		errNorm = 0;
		_JtJU = JtJU;
		_JtJW = JtJW;
		_JtJV = JtJV;
		_JtErr = JtErr;
		_errNorm = &errNorm;
		state = CALC_J;
		return true;
	}

	if( state == CALC_J )
	{
		dblCopy( nparams * 1, param, prevParam );
		step();
		_param = param;
		prevErrNorm = errNorm;
		errNorm = 0;
		_errNorm = &errNorm;
		state = CHECK_ERR;
		return true;
	}

	assert( state == CHECK_ERR );
	if( errNorm > prevErrNorm )
	{
		lambdaLg10++;
		step();
		_param = param;
		errNorm = 0;
		_errNorm = &errNorm;
		state = CHECK_ERR;
		return true;
	}

	lambdaLg10 = max(lambdaLg10-1, -16);
	if( ++iters >= max_iter ||
		(change = cvNorm(nparams*1, param, prevParam)) < epsilon )
	{
		_param = param;
		state = DONE;
		return false;
	}

	prevErrNorm = errNorm;
	dblZero( nparams1 * nparams1, JtJU );
	dblZero( nparams1 * nparams2, JtJW );
	dblZero( nsize2 * nsize2 * ncount2, JtJV );
	dblZero( nparams * 1, JtErr );
	_param = param;
	_JtJU = JtJU;
	_JtJW = JtJW;
	_JtJV = JtJV;
	_JtErr = JtErr;
	state = CALC_J;
	return true;
}

void CBundleAdjustment::step()
{
	const double LOG10 = log(10.);
	double lambda = exp(lambdaLg10*LOG10);
	int i, j, k, l;

	for( i = 0; i < nparams; i++ )
		if( mask[i] == false )
		{
			if(i<nparams1)
			{
				double *row = JtJU + i*nparams1, *col = JtJU + i;
				for( j = 0; j < nparams1; j++ )
					row[j] = col[j*nparams1] = 0;
				row = JtJW + i*nparams2;
				for( j = 0; j < nparams2; j++ )
					row[j] = 0;
			}
			else
			{
				double *row = JtJV + i*nsize2, *col = JtJV + i%nsize2 + (i/nsize2)*nsize2*nsize2;
				for( j = 0; j < nsize2; j++ )
					row[j] = col[j*nsize2] = 0;
				col = JtJW + i;
				for( j = 0; j < nparams1; j++ )
					col[j*nparams2] = 0;
			}
			JtErr[i] = 0;
		}

	// Add rambda to U and V.
	for( i = 0; i < nparams1; i++ )
		JtJU[(nparams1+1)*i] *= 1. + lambda;
	for( i = 0; i < ncount2; i++ )
		for( j = 0; j < nsize2; j++ )
			JtJV[i*nsize2*nsize2+(nsize2+1)*j] *= 1. + lambda;
	
	// (U-WViW')dP = ErrP - WViErrM
	{
		// Vi
		{
			for( i = 0; i < ncount2; i++ )
			{
				Pinv(nsize2, nsize2, &JtJV[i*nsize2*nsize2], &_JtJVi[i*nsize2*nsize2]);
			}
		}
		// WVi
		{
			for( i = 0; i < ncount2; i++ )
				for( j = 0; j < nparams1; j++ )
					for( k = 0; k < nsize2; k++ )
					{
						_JtJWVi[i*nsize2 + j*nparams2 + k] = 0;
						for( l = 0; l < nsize2; l++ )
							_JtJWVi[i*nsize2 + j*nparams2 + k] += 
								JtJW[i*nsize2 + j*nparams2 + l] * 
								_JtJVi[i*nsize2*nsize2 + l*nsize2 + k];
					}
		}
		// U-WViW'
		{
			GEMM(nparams1, nparams2, nparams1, _JtJWVi, JtJW, -1, JtJU, 1, _JtJUmWViWt, MATOP_GEMM_B_T, true);
		}
		// ErrP - WViErrM
		{
			GEMM(nparams1, nparams2, 1, _JtJWVi, &JtErr[nparams1], -1, JtErr, 1, _JtErrPM, 0, true);
		}
		// Calc dP
		{
			//SVD(nparams1, nparams1, _JtJUmWViWt, _JtJUmWViWtS, _JtJUmWViWtU, 0);
			//SVBkSb(nparams1, nparams1, 1, _JtJUmWViWtS, _JtJUmWViWtU, _JtJUmWViWtU, _JtErrPM, param);
			SVD(nparams1, nparams1, _JtJUmWViWt, _JtJUmWViWtS, 0, _JtJUmWViWtU, MATHLIB_SVD_MODIFY_A + MATHLIB_SVD_U_T + MATHLIB_SVD_V_T);
			SVBkSb(nparams1, nparams1, 1, _JtJUmWViWtS, _JtJUmWViWtU, _JtJUmWViWtU, _JtErrPM, param, MATHLIB_SVD_MODIFY_A + MATHLIB_SVD_U_T + MATHLIB_SVD_V_T);
		}
	}
	// dM = Vi(ErrM - W'dP)
	{
		// ErrM - W'dP
		{
			GEMM(nparams2, nparams1, 1, JtJW, param, -1, &JtErr[nparams1], 1, &_JtErrPM[nparams1], MATOP_GEMM_A_T, true);
		}
		// Calc dM
		{
			double * dM = &param[nparams1];
			for( i = 0; i < ncount2; i++ )
				for( j = 0; j < nsize2; j++ )
				{
					dM[i*nsize2+j] = 0;
					for( k = 0; k < nsize2; k++ )
					{
						dM[i*nsize2+j] += 
							_JtJVi[i*nsize2*nsize2 + j*nsize2 + k] * 
							_JtErrPM[nparams1 + i*nsize2 + k];
					}
				}
		}
	}
	for( i = 0; i < nparams; i++ )
		param[i] = prevParam[i] - (mask[i] ? param[i] : 0);
}

void CBundleAdjustment::clear()
{
	if(param)
	{
		delete [] mask;
		delete [] prevParam;
		delete [] param;
		delete [] JtJU;
		delete [] JtJW;
		delete [] JtJV;
		delete [] _JtJUmWViWtS;
		delete [] _JtJUmWViWtU;
		delete [] JtErr;

		delete [] _JtJVi;
		delete [] _JtJWVi;
		delete [] _JtJUmWViWt;
		delete [] _JtErrPM;
	}
}
#pragma endregion

