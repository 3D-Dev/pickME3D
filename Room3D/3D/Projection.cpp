#include "Projection.h"
#include "Mathlib.h"

// A : 3 by 3 Calibration Matrix
// r : 3 by 1 rotation vector
// t : 3 by 1 translation vector
// k : 5 by 1 distortion vector
// PP : 3 by 1 matrix World Point
// Image : 2 by 1 matrix Image Point
// dA : 2 * 5 jacobian
// dr : 2 * 3 jacobian
// dt : 2 * 3 jacobian
// dk : 2 * 5 jacobian
// dP : 2 * 3 jacobian
void Project_Metric( double * A, double * r, double * t, double * k, double * PP, double * Image )
{
	double Rot[9];
	double dR_r[27];
	Rodrigues2(r, Rot, true, dR_r);
	double X, Y, Z;
	double x, y, z, xdot, ydot, xd2, yd2, u, v;
	double k1, k2, k3, p1, p2, r2, rcoef;
	double fx, fy, cx, cy, s;

	int i, j, l;

	// Project
	{
		k1 = k[0]; k2 = k[1]; k3 = k[2]; p1 = k[3]; p2 = k[4];
		fx = A[0]; fy = A[4]; cx = A[2]; cy = A[5]; s = A[1];

		X = PP[0];
		Y = PP[1];
		Z = PP[2];

		x = Rot[0] * X + Rot[1] * Y + Rot[2] * Z + t[0];
		y = Rot[3] * X + Rot[4] * Y + Rot[5] * Z + t[1];
		z = Rot[6] * X + Rot[7] * Y + Rot[8] * Z + t[2];

		xdot = x / z;
		ydot = y / z;

		r2 = xdot * xdot + ydot * ydot;
		rcoef = 1 + k1*r2 + k2*r2*r2 + k3*r2*r2*r2;

		xd2 = xdot*rcoef + 2*p1*xdot*ydot + p2*(r2+2*xdot*xdot);
		yd2 = ydot*rcoef + p1*(r2+2*ydot*ydot) + 2*p2*xdot*ydot;

		u = fx*xd2 + s*yd2 + cx;
		v = fy*yd2 + cy;

		Image[0] = u; Image[1] = v;
	}
}