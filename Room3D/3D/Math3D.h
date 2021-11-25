#ifndef _3DMATH_H_
#define _3DMATH_H_

#include <assert.h>
#include <math.h>

#ifdef _WIN32
#include <limits>
#else
#include <limits.h>
#endif

#include <memory.h>
#include <stdlib.h>
#include <float.h>

#define fzero			      float(0)
#define fzero_5             float(0.5)
#define fone			      float(1.0)
#define ftwo			      float(2)
#define fhalf_pi            float(3.14159265358979323846264338327950288419716939937510582 * 0.5)
#define fquarter_pi         float(3.14159265358979323846264338327950288419716939937510582 * 0.25)
#define fpi			      float(3.14159265358979323846264338327950288419716939937510582)
#define ftwo_pi			  float(3.14159265358979323846264338327950288419716939937510582 * 2.0)
#define foo_pi			  fone / ps_pi
#define foo_two_pi	      fone / ps_two_pi
#define foo_255   	      fone / float(255)
#define foo_128   	      fone / float(128)
#define fto_rad             fpi / float(180)
#define fto_deg             float(180) / ps_pi
#define feps		          float(10e-6)
#define fdouble_eps	      float(10e-6) * ftwo
#define fbig_eps            float(10e-2)
#define fsmall_eps          float(10e-6)
#define fsqrthalf           float(0.7071067811865475244)

struct vec2;
struct vec2t;
struct vec3;
struct vec3t;
struct vec4;
struct vec4t;

struct vec2
{
	vec2() { }
	vec2(float x, float y) : x(x), y(y) { }
	vec2(const float* xy) : x(xy[0]), y(xy[1]) { }
	vec2(const vec2& u) : x(u.x), y(u.y) { }
	vec2(const vec3&);

	bool operator==(const vec2 & u) const
	{
		return (u.x == x && u.y == y) ? true : false;
	}

	bool operator!=(const vec2 & u) const
	{
		return !(*this == u );
	}

	vec2 & operator*=(const float & lambda)
	{
		x*= lambda;
		y*= lambda;
		return *this;
	}

	vec2 & operator-=(const vec2 & u)
	{
		x-= u.x;
		y-= u.y;
		return *this;
	}

	vec2 & operator+=(const vec2 & u)
	{
		x+= u.x;
		y+= u.y;
		return *this;
	}

	float & operator[](int i)
	{
		return vec_array[i];
	}

	const float operator[](int i) const
	{
		return vec_array[i];
	}

	operator float*() { return this->vec_array; }
	operator const float*() const { return this->vec_array; }

	float sq_norm() const { return x * x + y * y; }
	float norm() const { return sqrtf(sq_norm()); }

	union {
		struct {
			float x,y;          // standard names for components
		};
		struct {
			float s,t;          // standard names for components
		};
		float vec_array[2];     // array access
	};
};

inline const vec2 operator+(const vec2& u, const vec2& v)
{
	return vec2(u.x + v.x, u.y + v.y);
}

inline const vec2 operator-(const vec2& u, const vec2& v)
{
	return vec2(u.x - v.x, u.y - v.y);
}

inline const vec2 operator*(const float s, const vec2& u)
{
	return vec2(s * u.x, s * u.y);
}

inline const vec2 operator/(const vec2& u, const float s)
{
	return vec2(u.x / s, u.y / s);
}

inline const vec2 operator*(const vec2&u, const vec2&v)
{
	return vec2(u.x * v.x, u.y * v.y);
}

struct vec3
{
	vec3() { }
	vec3(float x, float y, float z) : x(x), y(y), z(z) { }
	vec3(const float* xyz) : x(xyz[0]), y(xyz[1]), z(xyz[2]) { }
	vec3(const vec2& u) : x(u.x), y(u.y), z(1.0f) { }
	vec3(const vec3& u) : x(u.x), y(u.y), z(u.z) { }
	vec3(const vec4&);

	bool operator==(const vec3 & u) const
	{
		return (u.x == x && u.y == y && u.z == z) ? true : false;
	}

	bool operator!=( const vec3& rhs ) const
	{
		return !(*this == rhs );
	}

	vec3 & operator*=(const float & lambda)
	{
		x*= lambda;
		y*= lambda;
		z*= lambda;
		return *this;
	}

	vec3 operator - () const
	{
		return vec3(-x, -y, -z);
	}

	vec3 & operator-=(const vec3 & u)
	{
		x-= u.x;
		y-= u.y;
		z-= u.z;
		return *this;
	}

	vec3 & operator+=(const vec3 & u)
	{
		x+= u.x;
		y+= u.y;
		z+= u.z;
		return *this;
	}
	float normalize();
	void orthogonalize( const vec3& v );
	void orthonormalize( const vec3& v )
	{
		orthogonalize( v ); //  just orthogonalize...
		normalize();        //  ...and normalize it
	}

	float sq_norm() const { return x * x + y * y + z * z; }
	float norm() const { return sqrtf(sq_norm()); }

	float & operator[](int i)
	{
		return vec_array[i];
	}

	const float operator[](int i) const
	{
		return vec_array[i];
	}

	operator float*() { return this->vec_array; }
	operator const float*() const { return this->vec_array; }

	union {
		struct {
			float x,y,z;        // standard names for components
		};
		struct {
			float s,t,r;        // standard names for components
		};
		struct {
			float r,g,b;
		};

		float vec_array[3];     // array access
	};
};

inline const vec3 operator+(const vec3& u, const vec3& v)
{
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline const vec3 operator-(const vec3& u, const vec3& v)
{
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline const vec3 operator^(const vec3& u, const vec3& v)
{
	return vec3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline const vec3 operator*(const float s, const vec3& u)
{
	return vec3(s * u.x, s * u.y, s * u.z);
}

inline const vec3 operator/(const vec3& u, const float s)
{
	return vec3(u.x / s, u.y / s, u.z / s);
}

inline const vec3 operator*(const vec3& u, const vec3& v)
{
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec2::vec2(const vec3& u)
{
	float k = 1 / u.z;
	x = k * u.x;
	y = k * u.y;
}

struct vec4
{
	vec4() { }
	vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
	vec4(const float* xyzw) : x(xyzw[0]), y(xyzw[1]), z(xyzw[2]), w(xyzw[3]) { }
	vec4(const vec3& u) : x(u.x), y(u.y), z(u.z), w(1.0f) { }
	vec4(const vec4& u) : x(u.x), y(u.y), z(u.z), w(u.w) { }

	bool operator==(const vec4 & u) const
	{
		return (u.x == x && u.y == y && u.z == z && u.w == w) ? true : false;
	}

	bool operator!=( const vec4& rhs ) const
	{
		return !(*this == rhs );
	}


	vec4 & operator*=(const float & lambda)
	{
		x*= lambda;
		y*= lambda;
		z*= lambda;
		w*= lambda;
		return *this;
	}

	vec4 & operator-=(const vec4 & u)
	{
		x-= u.x;
		y-= u.y;
		z-= u.z;
		w-= u.w;
		return *this;
	}

	vec4 & operator+=(const vec4 & u)
	{
		x+= u.x;
		y+= u.y;
		z+= u.z;
		w+= u.w;
		return *this;
	}

	vec4 operator - () const
	{
		return vec4(-x, -y, -z, -w);
	}

	float & operator[](int i)
	{
		return vec_array[i];
	}

	const float operator[](int i) const
	{
		return vec_array[i];
	}

	operator float*() { return this->vec_array; }
	operator const float*() const { return this->vec_array; }

	union {
		struct {
			float x,y,z,w;          // standard names for components
		};
		struct {
			float s,t,p,q;          // standard names for components
		};
		struct {
			float r,g,b,a;
		};

		float vec_array[4];     // array access
	};
};

inline const vec4 operator+(const vec4& u, const vec4& v)
{
	return vec4(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);
}

inline const vec4 operator-(const vec4& u, const vec4& v)
{
	return vec4(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);
}

inline const vec4 operator*(const float s, const vec4& u)
{
	return vec4(s * u.x, s * u.y, s * u.z, s * u.w);
}

inline const vec4 operator/(const vec4& u, const float s)
{
	return vec4(u.x / s, u.y / s, u.z / s, u.w / s);
}

inline const vec4 operator*(const vec4& u, const vec4& v)
{
	return vec4(u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w);
}

inline vec3::vec3(const vec4& u)
{
	x = u.x;
	y = u.y;
	z = u.z;
}

// quaternion
struct quat;  

/*
for all the matrices...a<x><y> indicates the element at row x, col y

For example:
a01 <-> row 0, col 1 
*/

struct mat3
{
	mat3();
	mat3(const float * array);
	mat3(const mat3 & M);
	mat3( const float& f0,  const float& f1,  const float& f2,  
		const float& f3,  const float& f4,  const float& f5,  
		const float& f6,  const float& f7,  const float& f8 )
		: a00( f0 ), a10( f1 ), a20( f2 ), 
		a01( f3 ), a11( f4 ), a21( f5 ),
		a02( f6 ), a12( f7 ), a22( f8) { }

	const vec3 col(const int i) const
	{
		return vec3(&mat_array[i * 3]);
	}

	const vec3 operator[](int i) const
	{
		return vec3(mat_array[i], mat_array[i + 3], mat_array[i + 6]);
	}

	const float& operator()(const int& i, const int& j) const
	{
		return mat_array[ j * 3 + i ];
	}

	float& operator()(const int& i, const int& j)
	{
		return  mat_array[ j * 3 + i ];
	}

	mat3 & operator*=(const float & lambda)
	{
		for (int i = 0; i < 9; ++i)
			mat_array[i] *= lambda;
		return *this;
	}

	mat3 & operator-=(const mat3 & M)
	{
		for (int i = 0; i < 9; ++i)
			mat_array[i] -= M.mat_array[i];
		return *this;
	}

	operator float*() { return this->mat_array; }
	operator const float*() const { return this->mat_array; }

	void set_row(int i, const vec3 & v)
	{
		mat_array[i] = v.x;
		mat_array[i + 3] = v.y;
		mat_array[i + 6] = v.z;
	}

	void set_col(int i, const vec3 & v)
	{
		mat_array[i * 3] = v.x;
		mat_array[i * 3 + 1] = v.y;
		mat_array[i * 3 + 2] = v.z;
	}

	void set_rot(const float & theta, const vec3 & v);
	void set_rot(const vec3 & u, const vec3 & v);

	// Matrix norms...
	// Compute || M ||
	//                1
	float norm_one();

	// Compute || M ||
	//                +inf
	float norm_inf();

	union {
		struct {
			float a00, a10, a20;        // standard names for components
			float a01, a11, a21;        // standard names for components
			float a02, a12, a22;        // standard names for components
		};
		float mat_array[9];     // array access
	};
};

const vec3 operator*(const mat3&, const vec3&);
const vec3 operator*(const vec3&, const mat3&);

struct mat4
{
	mat4();
	mat4(const float * array);
	mat4(const mat4 & M);

	mat4( const float& f0,  const float& f1,  const float& f2,  const float& f3,
		const float& f4,  const float& f5,  const float& f6,  const float& f7,
		const float& f8,  const float& f9,  const float& f10, const float& f11,
		const float& f12, const float& f13, const float& f14, const float& f15 )
		: a00( f0 ), a10( f1 ), a20( f2 ), a30( f3 ),
		a01( f4 ), a11( f5 ), a21( f6 ), a31( f7 ),
		a02( f8 ), a12( f9 ), a22( f10), a32( f11),
		a03( f12), a13( f13), a23( f14), a33( f15) { }

	const vec4 col(const int i) const
	{
		return vec4(&mat_array[i * 4]);
	}

	const vec4 operator[](const int& i) const
	{
		return vec4(mat_array[i], mat_array[i + 4], mat_array[i + 8], mat_array[i + 12]);
	}

	const float& operator()(const int& i, const int& j) const
	{
		return mat_array[ j * 4 + i ];
	}

	float& operator()(const int& i, const int& j)
	{
		return  mat_array[ j * 4 + i ];
	}

	operator float*() { return this->mat_array; }
	operator const float*() const { return this->mat_array; }

	void set_col(int i, const vec4 & v)
	{
		mat_array[i * 4] = v.x;
		mat_array[i * 4 + 1] = v.y;
		mat_array[i * 4 + 2] = v.z;
		mat_array[i * 4 + 3] = v.w;
	}

	void set_row(int i, const vec4 & v)
	{
		mat_array[i] = v.x;
		mat_array[i + 4] = v.y;
		mat_array[i + 8] = v.z;
		mat_array[i + 12] = v.w;
	}

	mat3 & get_rot(mat3 & M) const;
	quat & get_rot(quat & q) const;
	void set_rot(const quat & q);
	void set_rot(const mat3 & M);
	void set_rot(const float & theta, const vec3 & v);
	void set_rot(const vec3 & u, const vec3 & v);

	void set_scale(const vec3& s);
	vec3& get_scale(vec3& s) const;
	void set_translation(const vec3 & t);
	vec3 & get_translation(vec3 & t) const;

	mat4 operator*(const mat4&) const;

	union {
		struct {
			float a00, a10, a20, a30;   // standard names for components
			float a01, a11, a21, a31;   // standard names for components
			float a02, a12, a22, a32;   // standard names for components
			float a03, a13, a23, a33;   // standard names for components
		};
		struct {
			float _11, _12, _13, _14;   // standard names for components
			float _21, _22, _23, _24;   // standard names for components
			float _31, _32, _33, _34;   // standard names for components
			float _41, _42, _43, _44;   // standard names for components
		};
		union {
			struct {
				float b00, b10, b20, p; // standard names for components
				float b01, b11, b21, q; // standard names for components
				float b02, b12, b22, r; // standard names for components
				float x, y, z, w;       // standard names for components
			};
		};
		float mat_array[16];     // array access
	};
};

const vec4 operator*(const mat4&, const vec4&);
const vec4 operator*(const vec4&, const mat4&);

/*****************************************************************************/
/*                                                                           */
/* ivec2                                                                     */
/*                                                                           */
/*****************************************************************************/

struct ivec2 {
	ivec2(void) : a(0), b(0) { }
	ivec2(long _a,long _b) : a(_a), b(_b) { }
	ivec2(const long *iv) : a(iv[0]), b(iv[1]) { }
	ivec2(const ivec2 &iv) : a(iv.a), b(iv.b) { }

	int operator==(const ivec2 &iv) { return ((this->a == iv.a) && (this->b == iv.b)); }
	int operator!=(const ivec2 &iv) { return !(*this == iv); }

	ivec2 &operator=(long _i) { this->x=_i; this->y=_i; return (*this); }
	const ivec2 operator*(long _i) const { return ivec2(this->a * _i,this->b * _i); }
	const ivec2 operator/(long _i) const { return ivec2(this->a / _i,this->b / _i); }
	const ivec2 operator+(const ivec2 &iv) const { return ivec2(this->a + iv.a,this->b + iv.b); }
	const ivec2 operator-() const { return ivec2(-this->a,-this->b); }
	const ivec2 operator-(const ivec2 &iv) const { return ivec2(this->a - iv.a,this->b - iv.b); }

	ivec2 &operator*=(long _i) { return *this = *this * _i; }
	ivec2 &operator/=(long _i) { return *this = *this / _i; }
	ivec2 &operator+=(const ivec2 &iv) { return *this = *this + iv; }
	ivec2 &operator-=(const ivec2 &iv) { return *this = *this - iv; }

	long operator*(const ivec2 &iv) const { return this->a * iv.a + this->b * iv.b; }

	operator long*() { return this->i; }
	operator const long*() const { return this->i; }
	//	long &operator[](int _i) { return this->i[_i]; }
	//	const long &operator[](int _i) const { return this->i[_i]; }

	void set(long _a,long _b) { this->a = _a; this->b = _b; }
	void reset(void) { this->a = this->b = 0; }
	void swap(ivec2 &iv) { long tmp=a; a=iv.a; iv.a=tmp; tmp=b; b=iv.b; iv.b=tmp; }
	void swap(ivec2 *iv) { this->swap(*iv); }

	union {
		struct {long a,b;};
		struct {long x,y;};
		long i[2];
	};
};

/*****************************************************************************/
/*                                                                           */
/* ivec3                                                                     */
/*                                                                           */
/*****************************************************************************/
struct ivec4;

struct ivec3 {
	ivec3(void) : a(0), b(0), c(0) { }
	ivec3(long _a,long _b,long _c) : a(_a), b(_b), c(_c) { }
	ivec3(const long *iv) : a(iv[0]), b(iv[1]), c(iv[2]) { }
	ivec3(const ivec3 &iv) : a(iv.a), b(iv.b), c(iv.c) { }
	ivec3(const ivec4 &iv);

	int operator==(const ivec3 &iv) { return ((this->a == iv.a) && (this->b == iv.b) && (this->c == iv.c)); }
	int operator!=(const ivec3 &iv) { return !(*this == iv); }

	ivec3 &operator=(long _i) { this->x=_i; this->y=_i; this->z=_i; return (*this); }
	const ivec3 operator*(long _i) const { return ivec3(this->a * _i,this->b * _i,this->c * _i); }
	const ivec3 operator/(long _i) const { return ivec3(this->a / _i,this->b / _i,this->c / _i); }
	const ivec3 operator+(const ivec3 &iv) const { return ivec3(this->a + iv.a,this->b + iv.b,this->c + iv.c); }
	const ivec3 operator-() const { return ivec3(-this->a,-this->b,-this->c); }
	const ivec3 operator-(const ivec3 &iv) const { return ivec3(this->a - iv.a,this->b - iv.b,this->c - iv.c); }

	ivec3 &operator*=(long _i) { return *this = *this * _i; }
	ivec3 &operator/=(long _i) { return *this = *this / _i; }
	ivec3 &operator+=(const ivec3 &iv) { return *this = *this + iv; }
	ivec3 &operator-=(const ivec3 &iv) { return *this = *this - iv; }

	long operator*(const ivec3 &iv) const { return this->a * iv.a + this->b * iv.b + this->c * iv.c; }
	long operator*(const ivec4 &iv) const;

	operator long*() { return this->i; }
	operator const long*() const { return this->i; }
	//	long &operator[](int _i) { return this->i[_i]; }
	//	const long &operator[](int _i) const { return this->i[_i]; }

	void set(long _a,long _b,long _c) { this->a = _a; this->b = _b; this->c = _c; }
	void reset(void) { this->a = this->b = this->c = 0; }
	void swap(ivec3 &iv) { long tmp=a; a=iv.a; iv.a=tmp; tmp=b; b=iv.b; iv.b=tmp; tmp=c; c=iv.c; iv.c=tmp; }
	void swap(ivec3 *iv) { this->swap(*iv); }

	union {
		struct {long a,b,c;};
		struct {long x,y,z;};
		struct {long red,green,blue;};
		long i[3];
	};
};

/*****************************************************************************/
/*                                                                           */
/* ivec4                                                                     */
/*                                                                           */
/*****************************************************************************/

struct ivec4 {
	ivec4(void) : a(0), b(0), c(0), d(1) { }
	ivec4(long _a,long _b,long _c,long _d) : a(_a), b(_b), c(_c), d(_d) { }
	ivec4(const long *iv) : a(iv[0]), b(iv[1]), c(iv[2]), d(iv[3]) { }
	ivec4(const ivec3 &iv) : a(iv.a), b(iv.b), c(iv.c), d(1) { }
	ivec4(const ivec3 &iv,long _d) : a(iv.a), b(iv.b), c(iv.c), d(_d) { }
	ivec4(const ivec4 &iv) : a(iv.a), b(iv.b), c(iv.c), d(iv.d) { }

	int operator==(const ivec4 &iv) { return ((this->a == iv.a) && (this->b == iv.b) && (this->c == iv.c) && (this->d == iv.d)); }
	int operator!=(const ivec4 &iv) { return !(*this == iv); }

	ivec4 &operator=(long _i) { this->x=_i; this->y=_i; this->z=_i; this->w=_i; return (*this); }
	const ivec4 operator*(long _i) const { return ivec4(this->a * _i,this->b * _i,this->c * _i,this->d * _i); }
	const ivec4 operator/(long _i) const { return ivec4(this->a / _i,this->b / _i,this->c / _i,this->d / _i); }
	const ivec4 operator+(const ivec4 &iv) const { return ivec4(this->a + iv.a,this->b + iv.b,this->c + iv.c,this->d + iv.d); }
	const ivec4 operator-() const { return ivec4(-this->a,-this->b,-this->c,-this->d); }
	const ivec4 operator-(const ivec4 &iv) const { return ivec4(this->a - iv.a,this->b - iv.b,this->c - iv.c,this->d - iv.d); }

	ivec4 &operator*=(long _i) { return *this = *this * _i; }
	ivec4 &operator/=(long _i) { return *this = *this / _i; }
	ivec4 &operator+=(const ivec4 &iv) { return *this = *this + iv; }
	ivec4 &operator-=(const ivec4 &iv) { return *this = *this - iv; }

	long operator*(const ivec3 &iv) const { return this->a * iv.a + this->b * iv.b + this->c * iv.c + this->d; }
	long operator*(const ivec4 &iv) const { return this->a * iv.a + this->b * iv.b + this->c * iv.c + this->d * iv.d; }

	operator long*() { return this->i; }
	operator const long*() const { return this->i; }
	//	long &operator[](int _i) { return this->i[_i]; }
	//	const long &operator[](int _i) const { return this->i[_i]; }

	void set(long _a,long _b,long _c,long _d) { this->a = _a; this->b = _b; this->c = _c; this->d = _d; }
	void reset(void) { this->a = this->b = this->c = this->d = 0; }
	void swap(ivec4 &iv) { long tmp=a; a=iv.a; iv.a=tmp; tmp=b; b=iv.b; iv.b=tmp; tmp=c; c=iv.c; iv.c=tmp; tmp=d; d=iv.d; iv.d=tmp; }
	void swap(ivec4 *iv) { this->swap(*iv); }

	union {
		struct {long a,b,c,d;};
		struct {long x,y,z,w;};
		struct {long red,green,blue,alpha;};
		long i[4];
	};
};

inline ivec3::ivec3(const ivec4 &iv) {
	this->a = iv.a;
	this->b = iv.b;
	this->c = iv.c;
}

inline long ivec3::operator*(const ivec4 &iv) const {
	return this->a * iv.a + this->b * iv.b + this->c * iv.c + iv.d;
}

// quaternion
struct quat {
public:
	quat();
	quat(float x, float y, float z, float w);
	quat(const quat& quat);
	quat(const vec3& axis, float angle);
	quat(const mat3& rot);
	quat& operator=(const quat& quat);
	quat operator-()
	{
		return quat(-x, -y, -z, -w);
	}
	quat Inverse();
	void Normalize();
	void FromMatrix(const mat3& mat);
	void ToMatrix(mat3& mat) const;
	quat& operator*=(const quat& q);
	static const quat Identity;
	float& operator[](int i) { return comp[i]; }
	const float operator[](int i) const { return comp[i]; }
	union {
		struct {
			float x, y, z, w;
		};
		float comp[4];
	};
};
const quat operator*(const quat&, const quat&);
extern quat & add_quats(quat & p, const quat & q1, const quat & q2);
extern float dot(const quat & p, const quat & q);
extern quat & dot(float s, const quat & p, const quat & q);
extern quat & slerp_quats(quat & p, float s, const quat & q1, const quat & q2);
extern quat & axis_to_quat(quat & q, const vec3 & a, const float phi);
extern mat3 & quat_2_mat(mat3 &M, const quat &q );
extern quat & mat_2_quat(quat &q,const mat3 &M);

// constant algebraic values
static const float array16_id[] =        { fone, fzero, fzero, fzero,
fzero, fone, fzero, fzero,
fzero, fzero, fone, fzero,
fzero, fzero, fzero, fone};

static const float array16_null[] =      { fzero, fzero, fzero, fzero,
fzero, fzero, fzero, fzero,
fzero, fzero, fzero, fzero,
fzero, fzero, fzero, fzero};

static const float array16_scale_bias[] = { fzero_5, fzero,   fzero,   fzero,
fzero,   fzero_5, fzero,   fzero,
fzero,   fzero,   fzero_5, fzero,
fzero_5, fzero_5, fzero_5, fone};

static const float array9_id[] =         { fone, fzero, fzero,
fzero, fone, fzero,
fzero, fzero, fone};


static const vec2      vec2_null(fzero,fzero);
static const vec4      vec4_one(fone,fone,fone,fone);
static const vec3      vec3_one(fone,fone,fone);
static const vec3      vec3_null(fzero,fzero,fzero);
static const vec3      vec3_x(fone,fzero,fzero);
static const vec3      vec3_y(fzero,fone,fzero);
static const vec3      vec3_z(fzero,fzero,fone);
static const vec3      vec3_neg_x(-fone,fzero,fzero);
static const vec3      vec3_neg_y(fzero,-fone,fzero);
static const vec3      vec3_neg_z(fzero,fzero,-fone);
static const vec4      vec4_null(fzero,fzero,fzero,fzero);
static const vec4      vec4_x(fone,fzero,fzero,fzero);
static const vec4      vec4_neg_x(-fone,fzero,fzero,fzero);
static const vec4      vec4_y(fzero,fone,fzero,fzero);
static const vec4      vec4_neg_y(fzero,-fone,fzero,fzero);
static const vec4      vec4_z(fzero,fzero,fone,fzero);
static const vec4      vec4_neg_z(fzero,fzero,-fone,fzero);
static const vec4      vec4_w(fzero,fzero,fzero,fone);
static const vec4      vec4_neg_w(fzero,fzero,fzero,-fone);
static const quat      quat_id(fzero,fzero,fzero,fone);
static const mat4      mat4_id(array16_id);
static const mat3      mat3_id(array9_id);
static const mat4      mat4_null(array16_null);
static const mat4      mat4_scale_bias(array16_scale_bias);

// normalizes a vector and return a reference of itself
extern vec2 & normalize(vec2 & u);
extern vec3 & normalize(vec3 & u);
extern vec4 & normalize(vec4 & u);

// Computes the squared magnitude
inline float sq_norm(const vec3 & n)
{ return n.x * n.x + n.y * n.y + n.z * n.z; }

inline float sq_norm(const vec4 & n)
{ return n.x * n.x + n.y * n.y + n.z * n.z + n.w * n.w; }

// Computes the magnitude
inline float norm(const vec3 & n)
{ return sqrtf(sq_norm(n)); }

inline float norm(const vec4 & n)
{ return sqrtf(sq_norm(n)); }


// computes the cross product ( v cross w) and stores the result in u
// i.e.     u = v cross w
extern vec3 & cross(vec3 & u, const vec3 & v, const vec3 & w);

// computes the dot product ( v dot w) and stores the result in u
// i.e.     u = v dot w
extern float & dot(float & u, const vec3 & v, const vec3 & w);
extern float dot(const vec3 & v, const vec3 & w);
extern float & dot(float & u, const vec4 & v, const vec4 & w);
extern float dot(const vec4 & v, const vec4 & w);
extern float & dot(float & u, const vec3 & v, const vec4 & w);
extern float dot(const vec3 & v, const vec4 & w);
extern float & dot(float & u, const vec4 & v, const vec3 & w);
extern float dot(const vec4 & v, const vec3 & w);

// compute the reflected vector R of L w.r.t N - vectors need to be 
// normalized
//
//                R     N     L
//                  _       _
//                 |\   ^   /|
//                   \  |  /
//                    \ | /
//                     \|/
//                      +
extern vec3 & reflect(vec3 & r, const vec3 & n, const vec3 & l);

// Computes u = v * lambda + u
extern vec3 & madd(vec3 & u, const vec3 & v, const float & lambda);
// Computes u = v * lambda
extern vec3 & mult(vec3 & u, const vec3 & v, const float & lambda);
// Computes u = v * w
extern vec3 & mult(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v + w
extern vec3 & add(vec3 & u, const vec3 & v, const vec3 & w);
// Computes u = v - w
extern vec3 & sub(vec3 & u, const vec3 & v, const vec3 & w);

// Computes u = u * s
extern vec2 & scale(vec2 & u, const float s);
extern vec3 & scale(vec3 & u, const float s);
extern vec4 & scale(vec4 & u, const float s);

// Computes u = M * v
extern vec3 & mult(vec3 & u, const mat3 & M, const vec3 & v);
extern vec4 & mult(vec4 & u, const mat4 & M, const vec4 & v);

// Computes u = v * M
extern vec3 & mult(vec3 & u, const vec3 & v, const mat3 & M);
extern vec4 & mult(vec4 & u, const vec4 & v, const mat4 & M);

// Computes u = M(4x4) * v and divides by w
extern vec3 & mult_pos(vec3 & u, const mat4 & M, const vec3 & v);
// Computes u = M(4x4) * v
extern vec3 & mult_dir(vec3 & u, const mat4 & M, const vec3 & v);
// Computes u = M(4x4) * v and does not divide by w (assumed to be 1)
extern vec3 & mult(vec3& u, const mat4& M, const vec3& v);

// Computes u = v * M(4x4) and divides by w
extern vec3 & mult_pos(vec3 & u, const vec3 & v, const mat4 & M);
// Computes u = v * M(4x4)
extern vec3 & mult_dir(vec3 & u, const vec3 & v, const mat4 & M);
// Computes u = v * M(4x4) and does not divide by w (assumed to be 1)
extern vec3 & mult(vec3& u, const vec3& v, const mat4& M);

// Computes A += B
extern mat4 & add(mat4 & A, const mat4 & B);
extern mat3 & add(mat3 & A, const mat3 & B);

// Computes C = A + B
extern mat4 & add(mat4 & C, const mat4 & A, const mat4 & B);
extern mat3 & add(mat3 & C, const mat3 & A, const mat3 & B);

// Computes C = A * B
extern mat4 & mult(mat4 & C, const mat4 & A, const mat4 & B);
extern mat3 & mult(mat3 & C, const mat3 & A, const mat3 & B);

// Compute M = -M
extern mat4 & negate(mat4 & M);
extern mat3 & negate(mat3 & M);

// Computes B = Transpose(A)
//       T
//  B = A
extern mat3 & transpose(mat3 & B, const mat3 & A);
extern mat4 & transpose(mat4 & B, const mat4 & A);

// Computes B = Transpose(B)
//       T
//  B = B
extern mat3 & transpose(mat3 & B);
extern mat4 & transpose(mat4 & B);

// Computes B = inverse(A)
//       -1
//  B = A
extern mat4 & invert(mat4 & B, const mat4 & A);
extern mat3 & invert(mat3 & B, const mat3 & A);

// Computes B = inverse(A)
//                                       T  T
//                   (R t)             (R -R t)
// assuming that A = (0 1) so that B = (0    1)
//  B = A
extern mat4 & invert_rot_trans(mat4 & B, const mat4 & A);

extern mat4 & look_at(mat4 & M, const vec3 & eye, const vec3 & center, const vec3 & up);
extern mat4 & frustum(mat4 & M, const float l, const float r, const float b, 
					  const float t, const float n, const float f);

extern mat4 & perspective(mat4 & M, const float fovy, const float aspect, const float n, const float f);
extern mat4 & ortho(mat4 & M, const float left, 
					const float right, 
					const float bottom, 
					const float top,
					const float n,
					const float f);

/* Decompose Affine Matrix 
*    A = TQS, where
* A is the affine transform
* T is the translation vector
* Q is the rotation (quaternion)
* S is the scale vector
* f is the sign of the determinant
*/
extern void decomp_affine(const mat4 & A, vec3 & T, quat & Q, quat & U, vec3 & S, float & f);
// quaternion
extern quat & normalize(quat & p);
extern quat & conj(quat & p);
extern quat & conj(quat & p, const quat & q);
extern quat & add_quats(quat & p, const quat & q1, const quat & q2);
extern quat & axis_to_quat(quat & q, const vec3 & a, const float phi);
extern mat3 & quat_2_mat(mat3 &M, const quat &q );
extern quat & mat_2_quat(quat &q,const mat3 &M);
extern quat & mat_2_quat(quat &q,const mat4 &M);

// surface properties
extern mat3 & tangent_basis(mat3 & basis,const vec3 & v0,const vec3 & v1,const vec3 & v2,const vec2 & t0,const vec2 & t1,const vec2 & t2, const vec3 & n);

// linear interpolation
inline float lerp(float t, float a, float b)
{ return a * (fone - t) + t * b; }

inline vec3 & lerp(vec3 & w, const float & t, const vec3 & u, const vec3 & v)
{ w.x = lerp(t, u.x, v.x); w.y = lerp(t, u.y, v.y); w.z = lerp(t, u.z, v.z); return w; }

inline vec4 & lerp(vec4 & w, const float & t, const vec4 & u, const vec4 & v)
{ w.x = lerp(t, u.x, v.x); w.y = lerp(t, u.y, v.y); w.z = lerp(t, u.z, v.z); w.w = lerp(t, u.w, v.w); return w; }

// utilities
inline float ps_min(const float & lambda, const float & n)
{ return (lambda < n ) ? lambda : n; }

inline float ps_max(const float & lambda, const float & n)
{ return (lambda > n ) ? lambda : n; }

inline float ps_clamp(float u, const float min, const float max)
{ u = (u < min) ? min : u; u = (u > max) ? max : u; return u; }

extern float ps_random();

extern quat & trackball(quat & q, vec2 & pt1, vec2 & pt2, float trackballsize);

extern vec3 & cube_map_normal(int i, int x, int y, int cubesize, vec3 & v);

// Componentwise maximum and minium 
inline void ps_max(vec3 & vOut, const vec3 & vFirst, const vec3 & vSecond)
{
	vOut.x = ps_max(vFirst.x, vSecond.x);
	vOut.y = ps_max(vFirst.y, vSecond.y);
	vOut.z = ps_max(vFirst.z, vSecond.z);
}

inline void ps_min(vec3 & vOut, const vec3 & vFirst, const vec3 & vSecond)
{
	vOut.x = ps_min(vFirst.x, vSecond.x);
	vOut.y = ps_min(vFirst.y, vSecond.y);
	vOut.z = ps_min(vFirst.z, vSecond.z);
}



// fast cosine functions
extern float fast_cos(const float x);
extern float ffast_cos(const float x);

// determinant
float det(const mat3 & A);

float GetRandomFloat(float lowBound, float highBound);
void GetRandomVector(vec2& out, const vec2& min, const vec2& max);
void GetRandomVector(vec3& out, const vec3& min, const vec3& max);
void GetRandomVector(vec4& out, const vec4& min, const vec4& max);

#endif //_3DMATH_H_