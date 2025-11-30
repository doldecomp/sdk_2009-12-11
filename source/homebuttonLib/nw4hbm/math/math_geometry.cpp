#include "geometry.h"

/*******************************************************************************
 * headers
 */

#include <algorithm>

#include <macros.h>

#include <revolution/types.h>

#include "arithmetic.h"
#include "triangular.h"
#include "types.h"

/*******************************************************************************
 * local function declarations
 */

namespace nw4hbm { namespace math
{
	static bool IntersectionRay3AABB_(f32 min, f32 max, f32 o, f32 dir,
	                                  f32 &tNear, f32 &tFar);
}} // namespace nw4hbm::math

/*******************************************************************************
 * variables
 */

namespace nw4hbm { namespace math { namespace detail
{
	// .data
	f32 epsilon = 1e-4f;
	f32 infinity = 1e+8f; // Thats the biggest number ever ! - someone
}}} // namespace nw4hbm::math::detail

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace math {

void PLANE::Set(VEC3 const *P0, VEC3 const *P1, VEC3 const *P2)
{
	VEC3 v0, v1, v2;

	VEC3Sub(&v0, P2, P0);
	VEC3Sub(&v1, P1, P0);

	VEC3Normalize(&N, VEC3Cross(&v2, &v0, &v1));

	d = -VEC3Dot(&N, P0);
}

void AABB::Normalize()
{
	if (Pmin.x > Pmax.x)
		std::swap(Pmin.x, Pmax.x);

	if (Pmin.y > Pmax.y)
		std::swap(Pmin.y, Pmax.y);

	if (Pmin.z > Pmax.z)
		std::swap(Pmin.z, Pmax.z);
}

void AABB::Set(VEC3 const *arrayPoint, unsigned numPoints)
{
	Pmin = arrayPoint[0];
	Pmax = arrayPoint[0];

	for (unsigned i = 1; i < numPoints; ++i)
	{
		if (arrayPoint[i].x < Pmin.x)
			Pmin.x = arrayPoint[i].x;
		else if (arrayPoint[i].x > Pmax.x)
			Pmax.x = arrayPoint[i].x;

		if (arrayPoint[i].y < Pmin.y)
			Pmin.y = arrayPoint[i].y;
		else if (arrayPoint[i].y > Pmax.y)
			Pmax.y = arrayPoint[i].y;

		if (arrayPoint[i].z < Pmin.z)
			Pmin.z = arrayPoint[i].z;
		else if (arrayPoint[i].z > Pmax.z)
			Pmax.z = arrayPoint[i].z;
	}
}

void AABB::Set(AABB const *box, MTX34 const *M)
{
	f32 x0, y0, z0;
	f32 x1, y1, z1;

	f32 a0, a1;
	f32 b0, b1;

#define Calculate_(axis_, row_)												\
	do																		\
	{																		\
		axis_ ## 0 = M->_ ## row_ ## 0 * box->Pmin.x + M->_ ## row_ ## 3;	\
		axis_ ## 1 = M->_ ## row_ ## 0 * box->Pmax.x + M->_ ## row_ ## 3;	\
																			\
		a0 = M->_ ## row_ ## 1 * box->Pmin.y;								\
		a1 = M->_ ## row_ ## 1 * box->Pmax.y;								\
																			\
		b0 = M->_ ## row_ ## 2 * box->Pmin.z;								\
		b1 = M->_ ## row_ ## 2 * box->Pmax.z;								\
																			\
		if (axis_ ## 0 > axis_ ## 1)										\
			std::swap(axis_ ## 0, axis_ ## 1);								\
																			\
		if (a0 < a1)														\
		{																	\
			axis_ ## 0 += a0;												\
			axis_ ## 1 += a1;												\
		}																	\
		else																\
		{																	\
			axis_ ## 0 += a1;												\
			axis_ ## 1 += a0;												\
		}																	\
																			\
		if (b0 < b1)														\
		{																	\
			axis_ ## 0 += b0;												\
			axis_ ## 1 += b1;												\
		}																	\
		else																\
		{																	\
			axis_ ## 0 += b1;												\
			axis_ ## 1 += b0;												\
		}																	\
																			\
		Pmin.axis_ = axis_ ## 0;											\
		Pmax.axis_ = axis_ ## 1;											\
	} while (false)

	Calculate_(x, 0);
	Calculate_(y, 1);
	Calculate_(z, 2);

#undef Calculate_
}

void SPHERE::Set(VEC3 const *arrayPoint, unsigned numPoints)
{
	AABB tmp;
	tmp.Set(arrayPoint, numPoints);

	VEC3Lerp(&C, &tmp.Pmin, &tmp.Pmax, 0.5f);

	f32 maxDistance = VEC3DistSq(&C, &arrayPoint[0]);
	for (unsigned i = 1; i < numPoints; ++i)
	{
		f32 dist = VEC3DistSq(&C, &arrayPoint[i]);

		if (dist > maxDistance)
			maxDistance = dist;
	}

	r = FSqrt(maxDistance);
}

void FRUSTUM::Set(f32 fovy, f32 aspect, f32 n, f32 f, MTX34 const &camera)
{
	f32 tan = TanDeg(fovy * 0.5f);
	f32 ny = tan * n;
	f32 nx = ny / aspect;
	Set(ny, -ny, -nx, nx, n, f, camera);
}

void FRUSTUM::Set(f32 top, f32 bottom, f32 left, f32 right, f32 n, f32 f,
                  const MTX34 &camera)
{
	MTX34 invCamera;
	MTX34Inv(&invCamera, &camera);
	MTX34Copy(&cam, &camera);

	VEC3 P0(0.0f, 0.0f, 0.0f);
	VEC3 P[8];

	f32 f_n = f / n;

#define SetPlane_(index_, x_, y_, z_)	\
	do									\
	{									\
		P[index_].x = (x_);				\
		P[index_].y = (y_);				\
		P[index_].z = (z_);				\
	} while (false)

	SetPlane_(0, left, top, -n);
	SetPlane_(1, right, top, -n);
	SetPlane_(2, right, bottom, -n);
	SetPlane_(3, left, bottom, -n);
	SetPlane_(4, f_n * left, f_n * top, -f);
	SetPlane_(5, f_n * right, f_n * top, -f);
	SetPlane_(6, f_n * right, f_n * bottom, -f);
	SetPlane_(7, f_n * left, f_n * bottom, -f);

#undef SetPlane_

	near = -n;
	far = -f;

	leftPlane.Set(&P0, &P[3], &P[0]);
	rightPlane.Set(&P0, &P[1], &P[2]);
	topPlane.Set(&P0, &P[3], &P[1]);
	bottomPlane.Set(&P0, &P[2], &P[3]);

	int i; // Very Cool
	for (i = 0; i < (int)ARRAY_LENGTH(P); ++i)
		VEC3TransformCoord(&P[i], &invCamera, &P[i]);

	VEC3TransformCoord(&P0, &invCamera, &P[0]);

	box.Set(P, 8);

	planes[0].Set(&P0,   &P[3], &P[0]);
	planes[1].Set(&P0,   &P[1], &P[2]);
	planes[2].Set(&P[0], &P[1], &P[2]);
	planes[3].Set(&P[4], &P[7], &P[6]);
	planes[4].Set(&P0,   &P[3], &P[1]);
	planes[5].Set(&P0,   &P[2], &P[3]);
}

f32 DistSqPoint3ToLine3(VEC3 const *P, LINE3 const *L, f32 *t)
{
	f32 distSq;

	VEC3 LP;
	f32 t_ = VEC3Dot(&L->d, VEC3Sub(&LP, P, &L->P));

	VEC3 PP;
	VEC3Add(&PP, &L->P, VEC3Scale(&PP, &L->d, t_));

	VEC3 PP_P;
	VEC3Sub(&PP_P, P, &PP);

	distSq = VEC3LenSq(&PP_P);

	if (t)
		*t = t_;

	return distSq;
}

f32 DistSqPoint3ToRay3(VEC3 const *P, RAY3 const *R, f32 *t)
{
	f32 distSq;
	f32 t_;
	distSq = DistSqPoint3ToLine3(P, reinterpret_cast<LINE3 const *>(R), &t_);

	if (t_ < 0.0f)
	{
		t_ = 0.0f;

		VEC3 vec;
		VEC3Sub(&vec, P, &R->P);

		distSq = VEC3LenSq(&vec);
	}

	if (t)
		*t = t_;

	return distSq;
}

f32 DistSqPoint3ToSegment3(VEC3 const *P, SEGMENT3 const *S, f32 *t)
{
	f32 distSq;
	f32 t_;
	LINE3 L;
	L.Set(S);

	distSq = DistSqPoint3ToLine3(P, &L, &t_);

	if (t_ < 0.0f)
	{
		t_ = 0.0f;

		VEC3 vec;
		VEC3Sub(&vec, P, &S->P0);

		distSq = VEC3LenSq(&vec);
	}
	else
	{
		t_ = 1.0f;

		VEC3 vec;
		VEC3Sub(&vec, P, &S->P1);

		distSq = VEC3LenSq(&vec);
	}

	if (t)
		*t = t_;

	return distSq;
}

f32 DistSqPoint3ToPlane(VEC3 const *P, PLANE const *J, VEC3 *Q)
{
	f32 k = J->Test(*P);

	if (Q)
	{
		VEC3 tmp;

		VEC3Sub(Q, P, VEC3Scale(&tmp, &J->N, k));
	}

	return k * k;
}

f32 DistSqSphereToPlane(SPHERE const *S, PLANE const *J)
{
	f32 distance = J->Test(S->C);

	if (distance > S->r)
		return (distance - S->r) * (distance - S->r);
	else if (distance < -S->r)
		return (distance + S->r) * (distance + S->r);
	else
		return 0.0f;
}

f32 DistSqPoint3ToPolyline3(VEC3 const *P, VEC3 const *vertices,
                            unsigned nVertices)
{
	unsigned nSegments = nVertices - 1;

	f32 dSq = detail::infinity;
	f32 xMinusA, yMinusB, zMinusC;
	f32 xNextMinusA, yNextMinusB, zNextMinusC;
	f32 xMinusASq, yMinusBSq, zMinusCSq;
	f32 xNextMinusASq, yNextMinusBSq, zNextMinusCSq;

	xMinusA = vertices[0].x - P->x;
	yMinusB = vertices[0].y - P->y;
	zMinusC = vertices[0].z - P->z;

	xMinusASq = xMinusA * xMinusA;
	yMinusBSq = yMinusB * yMinusB;
	zMinusCSq = zMinusC * zMinusC;

	xNextMinusA = vertices[1].x - P->x;
	yNextMinusB = vertices[1].y - P->y;
	zNextMinusC = vertices[1].z - P->z;

	xNextMinusASq = xNextMinusA * xNextMinusA;
	yNextMinusBSq = yNextMinusB * yNextMinusB;
	zNextMinusCSq = zNextMinusC * zNextMinusC;

	f32 t;
	LINE3 L(vertices[0], vertices[1] - vertices[0], false);
	dSq = DistSqPoint3ToLine3(P, &L, &t);

	if (t < 0.0f)
		dSq = xMinusASq + yMinusBSq + zMinusCSq;
	else if (t > 1.0f)
		dSq = xNextMinusASq + yNextMinusBSq + zNextMinusCSq;

	for (unsigned i = 1; i < nSegments - 1; ++i)
	{
		// ERRATA: Implicit conversion turns bool into floating-point number
		/* The greater-than check should be outside the call to FAbs, but is
		 * instead inside.
		 */
#if defined(__MWERKS__)
# define FAbsCheck3_(a, b)	FAbs((a) > (b))
#else
# define FAbsCheck3_(a, b)	(FAbs(a) > (b))
#endif

		if ((FAbs(xMinusASq) >= dSq && FAbs(xNextMinusASq) >= dSq
		     && FAbsCheck3_(xMinusA * xNextMinusA, 0.0f))
		    || (FAbs(yMinusBSq) >= dSq && FAbs(yNextMinusBSq) >= dSq
		        && FAbsCheck3_(yMinusB * yNextMinusB, 0.0f))
		    || (FAbs(zMinusCSq) >= dSq && FAbs(zNextMinusCSq) >= dSq
		        && FAbsCheck3_(zMinusC * zNextMinusC, 0.0f)))
		{
			// This block makes the if condition look less cluttered
#undef FAbsCheck3_
		}
		else
		{
			f32 dSq_;
			f32 t;
			LINE3 L(vertices[i], vertices[i + 1] - vertices[i], false);
			dSq_ = DistSqPoint3ToLine3(P, &L, &t);

			if (t < 0.0f)
				dSq_ = xMinusASq + yMinusBSq + zMinusCSq;
			else if (t > 1.0f)
				dSq_ = xNextMinusASq + yNextMinusBSq + zNextMinusCSq;

			if (dSq_ < dSq)
				dSq = dSq_;
		}

		if (i != nSegments - 2)
		{
			xMinusA = xNextMinusA;
			yMinusB = yNextMinusB;
			zMinusC = zNextMinusC;

			xNextMinusA = vertices[i + 2].x - P->x;
			yNextMinusB = vertices[i + 2].y - P->y;
			zNextMinusC = vertices[i + 2].z - P->z;
		}
	}

	return dSq;
}

f32 DistSqLine3ToLine3(LINE3 const *L0, LINE3 const *L1, f32 *s, f32 *t)
{
	VEC3 u;
	VEC3Sub(&u, &L0->P, &L1->P);

	f32 b, d, e, f, det;

	b = VEC3Dot(&L0->d, &L1->d);
	d = VEC3Dot(&L0->d, &u);
	e = VEC3Dot(&L1->d, &u);
	f = VEC3LenSq(&u);
	det = 1.0f - b * b;

	if (det < detail::epsilon)
	{
		if (s)
			*s = 0.0f;

		if (t)
			*t = e;

		return f;
	}
	else
	{
		f32 invDet;
		f32 s_, t_;

		invDet = 1.0f / det;
		s_ = invDet * (b * e - d);
		t_ = invDet * (e - b * d);

		if (s)
			*s = s_;

		if (t)
			*t = t_;

		VEC3 tmp0, tmp1, tmp2;
		VEC3Sub(&tmp2, VEC3Scale(&tmp0, &L0->d, s_),
		        VEC3Scale(&tmp1, &L1->d, t_));

		VEC3Add(&tmp2, &tmp2, &u);

		return VEC3LenSq(&tmp2);
	}
}

f32 DistSqSegment3ToSegment3(SEGMENT3 const *S1, SEGMENT3 const *S2, f32 *s,
                             f32 *t)
{
	VEC3 u = S1->P1 - S1->P0;
	VEC3 v = S2->P1 - S2->P0;
	VEC3 w = S1->P0 - S2->P0;
	f32 a = VEC3LenSq(&u);
	f32 b = VEC3Dot(&u, &v);
	f32 c = VEC3LenSq(&v);
	f32 d = VEC3Dot(&u, &w);
	f32 e = VEC3Dot(&v, &w);
	f32 D = a * c - b * b;
	f32 sc, sN, sD;
	f32 tc, tN, tD;

	sD = D;
	tD = D;

	if (D < detail::epsilon)
	{
		sN = 0.0f;
		sD = 1.0f;

		tN = e;
		tD = c;
	}
	else
	{
		sN = b * e - c * d;
		tN = a * e - b * d;

		if (sN < 0.0f)
		{
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD)
		{
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	// NOTE the use of a double here instead of a float. Possibly an ERRATUM.
	if (tN < 0.0)
	{
		tN = 0.0f;

		if (-d < 0.0f)
		{
			sN = 0.0f;
		}
		else if (-d > a)
		{
			sN = sD;
		}
		else
		{
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD)
	{
		tN = tD;

		if (-d + b < 0.0f)
		{
			sN = 0.0f;
		}
		else if (-d + b > a)
		{
			sN = sD;
		}
		else
		{
			sN = -d + b;
			sD = a;
		}
	}

	sc = FAbs(sN) < detail::epsilon ? 0.0f : sN / sD;
	tc = FAbs(tN) < detail::epsilon ? 0.0f : tN / tD;

	if (s)
		*s = sc;

	if (t)
		*t = tc;

	VEC3 dP = w + u * sc - v * tc;

	return VEC3LenSq(&dP);
}

f32 DistSqLine3ToRay3(LINE3 const *L, RAY3 const *R, f32 *s, f32 *t)
{
	VEC3 u;
	f32 b, d, e, det;
	f32 sNum, tNum, tDenom, sDenom;

	VEC3Sub(&u, &L->P, &R->P);
	b = VEC3Dot(&L->d, &R->d);
	d = VEC3Dot(&L->d, &u);
	e = VEC3Dot(&R->d, &u);
	det = 1.0f - b * b;

	if (det < detail::epsilon)
	{
		sNum = 0.0f;
		tNum = e;

		tDenom = 1.0f;
		sDenom = det;
	}
	else
	{
		sNum = b * e - d;
		tNum = e - b * d;

		sDenom = tDenom = det;
	}

	if (tNum < 0.0f)
	{
		tNum = 0.0f;
		sNum = -d;

		sDenom = 1.0f;
	}

	f32 s_ = sNum / sDenom;
	f32 t_ = tNum / tDenom;

	if (s)
		*s = s_;

	if (t)
		*t = t_;

	VEC3 v;
	VEC3 tmp;

	VEC3Add(&v, &L->P, VEC3Scale(&tmp, &L->d, s_));
	VEC3Sub(&v, &v, &R->P);
	VEC3Sub(&v, &v, VEC3Scale(&tmp, &R->d, t_));

	return VEC3LenSq(&v);
}

f32 DistSqLine3ToSegment3(LINE3 const *L0, SEGMENT3 const *S, f32 *s, f32 *t)
{
	VEC3 segDir;
	VEC3Sub(&segDir, &S->P1, &S->P0);

	VEC3 u;
	f32 b, c, d, e, det;
	f32 sNum, tNum, tDenom, sDenom;

	VEC3Sub(&u, &L0->P, &S->P0);

	b = VEC3Dot(&L0->d, &segDir);
	c = VEC3LenSq(&segDir);
	d = VEC3Dot(&L0->d, &u);
	e = VEC3Dot(&segDir, &u);
	det = c - b * b;

	if (det < detail::epsilon)
	{
		sNum = 0.0f;
		tNum = e;

		sDenom = det;
		tDenom = c;
	}
	else
	{
		sNum = b * e - c * d;
		tNum = e - b * d;

		sDenom = tDenom = det;
	}

	if (tNum < 0.0f)
	{
		tNum = 0.0f;
		sNum = -d;

		sDenom = 1.0f;
	}
	else if (tNum > tDenom)
	{
		tNum = tDenom;
		sNum = -d + b;

		sDenom = 1.0f;
	}

	f32 s_ = sNum / sDenom;
	f32 t_ = tNum / tDenom;

	if (s)
		*s = s_;

	if (t)
		*t = t_;

	VEC3 v;
	VEC3 tmp;

	VEC3Add(&v, &L0->P, VEC3Scale(&tmp, &L0->d, s_));
	VEC3Sub(&v, &v, &S->P0);
	VEC3Sub(&v, &v, VEC3Scale(&tmp, &segDir, t_));

	return VEC3LenSq(&v);
}

f32 DistSqRay3ToRay3(RAY3 const *R0, RAY3 const *R1, f32 *s, f32 *t)
{
	VEC3 u;
	f32 b, d, e, det;
	f32 sNum, tNum, tDenom, sDenom;

	VEC3Sub(&u, &R0->P, &R1->P);
	b = VEC3Dot(&R0->d, &R1->d);
	d = VEC3Dot(&R0->d, &u);
	e = VEC3Dot(&R1->d, &u);
	det = 1.0f - b * b;

	if (det < detail::epsilon)
	{
		sNum = 0.0f;
		tNum = e;

		tDenom = 1.0f;
		sDenom = det;
	}
	else
	{
		sNum = b * e - d;
		tNum = e - b * d;

		sDenom = tDenom = det;
	}

	if (sNum < 0.0f)
	{
		sNum = 0.0f;
		tNum = e;

		tDenom = 1.0f;
	}

	if (tNum < 0.0f)
	{
		tNum = 0.0f;

		if (-d < 0.0f)
		{
			sNum = 0.0f;
		}
		else
		{
			sNum = -d;
			sDenom = 1.0f;
		}
	}

	f32 s_ = sNum / sDenom;
	f32 t_ = tNum / tDenom;

	if (s)
		*s = s_;

	if (t)
		*t = t_;

	VEC3 v;
	VEC3 tmp;

	VEC3Add(&v, &R0->P, VEC3Scale(&tmp, &R0->d, s_));
	VEC3Sub(&v, &v, &R1->P);
	VEC3Sub(&v, &v, VEC3Scale(&tmp, &R1->d, t_));

	return VEC3LenSq(&v);
}

f32 DistSqRay3ToSegment3(RAY3 const *R0, SEGMENT3 const *S, f32 *s, f32 *t)
{
	VEC3 segDir;
	VEC3 u;

	VEC3Sub(&segDir, &S->P1, &S->P0);
	f32 b, c, d, e, det;
	f32 sNum, tNum, tDenom, sDenom;

	VEC3Sub(&u, &R0->P, &S->P0);

	b = VEC3Dot(&R0->d, &segDir);
	c = VEC3LenSq(&segDir);
	d = VEC3Dot(&R0->d, &u);
	e = VEC3Dot(&segDir, &u);
	det = c - b * b;

	if (det < detail::epsilon)
	{
		sNum = 0.0f;
		tNum = e;

		tDenom = c;
		sDenom = det;
	}
	else
	{
		sNum = b * e - c * d;
		tNum = e - b * d;

		tDenom = sDenom = det;
	}

	if (sNum < 0.0f)
	{
		sNum = 0.0f;
		tNum = e;

		tDenom = c;
	}

	if (tNum < 0.0f)
	{
		tNum = 0.0f;

		if (-d < 0.0f)
		{
			sNum = 0.0f;
		}
		else
		{
			sNum = -d;
			sDenom = 1.0f;
		}
	}
	else if (tNum > tDenom)
	{
		tNum = tDenom;

		if (-d + b < 0.0f)
		{
			sNum = 0.0f;
		}
		else
		{
			sNum = -d + b;
			sDenom = 1.0f;
		}
	}

	f32 s_ = sNum / sDenom;
	f32 t_ = tNum / tDenom;

	if (s)
		*s = s_;

	if (t)
		*t = t_;

	VEC3 v;
	VEC3 tmp;

	VEC3Add(&v, &R0->P, VEC3Scale(&tmp, &R0->d, s_));
	VEC3Sub(&v, &v, &S->P0);
	VEC3Sub(&v, &v, VEC3Scale(&tmp, &segDir, t_));

	return VEC3LenSq(&v);
}

IntersectionResult IntersectionLine3Plane(LINE3 const *L, PLANE const *J,
                                          f32 *t, VEC3 *I)
{
	f32 denom = VEC3Dot(&L->d, &J->N);

	if (FAbs(denom) < detail::epsilon)
	{
		if (FAbs(J->d + VEC3Dot(&J->N, &L->P)) < detail::epsilon)
			return INTERSECTION_LINE3_ON_PLANE;
		else
			return INTERSECTION_NONE;
	}

	if (I || t)
	{
		f32 t_ = -(J->d + VEC3Dot(&J->N, &L->P)) / denom;

		if (I)
		{
			VEC3 tmp;

			VEC3Add(I, &L->P, VEC3Scale(&tmp, &L->d, t_));
		}

		if (t)
			*t = t_;
	}

	return INTERSECTION_1;
}

IntersectionResult IntersectionRay3Plane(RAY3 const *R, PLANE const *J, f32 *t,
                                         VEC3 *I)
{
	f32 t_;
	VEC3 I_;
	IntersectionResult result = IntersectionLine3Plane(
		reinterpret_cast<LINE3 const *>(R), J, &t_, I ? &I_ : nullptr);

	if (result == INTERSECTION_1)
	{
		if (t_ >= 0.0f)
		{
			if (t)
				*t = t_;

			if (I)
				*I = I_;

			return result;
		}
		else
		{
			return INTERSECTION_NONE;
		}
	}
	else
	{
		return result;
	}
}

IntersectionResult IntersectionSegment3Plane(SEGMENT3 const *S, PLANE const *J,
                                             f32 *t, VEC3 *I)
{
	f32 t_;
	VEC3 I_;
	VEC3 dir;
	VEC3Sub(&dir, &S->P1, &S->P0);

	LINE3 L(S->P0, dir, false);

	IntersectionResult result =
		IntersectionLine3Plane(&L, J, &t_, I ? &I_ : nullptr);

	if (result == INTERSECTION_1)
	{
		if (t_ >= 0.0f && t_ <= 1.0f)
		{
			if (t)
				*t = t_;

			if (I)
				*I = I_;

			return result;
		}
		else
		{
			return INTERSECTION_NONE;
		}
	}
	else
	{
		return result;
	}
}

IntersectionResult IntersectionLine3Sphere(LINE3 const *L, SPHERE const *sphere,
                                           f32 *t0, f32 *t1)
{
	f32 b, c, discrm;
	VEC3 PmC;

	VEC3Sub(&PmC, &L->P, &sphere->C);

	b = 2.0f * VEC3Dot(&L->d, &PmC);
	c = VEC3LenSq(&PmC) - sphere->r * sphere->r;
	discrm = b * b - 4.0f * c;

	if (discrm > 0.0f)
	{
		f32 sq = FSqrt(discrm);

		if (t0)
			*t0 = 0.5f * (-b - sq);
		if (t1)
			*t1 = 0.5f * (-b + sq);

		return INTERSECTION_2;
	}
	else if (discrm == 0.0f)
	{
		if (t0)
			*t0 = -0.5f * b;

		return INTERSECTION_1;
	}
	else
	{
		return INTERSECTION_NONE;
	}
}

IntersectionResult IntersectionRay3Sphere(RAY3 const *R, SPHERE const *sphere,
                                          f32 *t0, f32 *t1)
{
	IntersectionResult result;
	f32 t0_, t1_;

	result = IntersectionLine3Sphere(reinterpret_cast<LINE3 const *>(R), sphere,
	                                 &t0_, &t1_);

	if (result != INTERSECTION_NONE)
	{
		if (result == INTERSECTION_1)
		{
			if (t0_ >= 0.0f)
			{
				if (t0)
					*t0 = t0_;

				return result;
			}
			else
			{
				return INTERSECTION_NONE;
			}
		}

		if (t0_ >= 0.0f)
		{
			if (t0)
				*t0 = t0_;

			if (t1)
				*t1 = t1_;

			return result;
		}

		if (t1_ >= 0.0f)
		{
			if (t0)
				*t0 = t1_;

			return INTERSECTION_1;
		}
		else
		{
			return INTERSECTION_NONE;
		}
	}
	else
	{
		return result;
	}
}

bool IntersectionRay3Sphere(RAY3 const *R, SPHERE const *sphere)
{
	VEC3 w;
	VEC3Sub(&w, &sphere->C, &R->P);

	f32 wsq = VEC3LenSq(&w);
	f32 proj = VEC3Dot(&w, &R->d);
	f32 rsq = sphere->r * sphere->r;

	if (proj < 0.0f && wsq > rsq)
		return false;

	f32 vsq = VEC3LenSq(&R->d);

	if (vsq * wsq - proj * proj <= vsq * rsq)
		return true;
	else
		return false;
}

IntersectionResult IntersectionSegment3Sphere(SEGMENT3 const *S,
                                              SPHERE const *sphere, f32 *t0,
                                              f32 *t1)
{
	VEC3 dir;
	VEC3Sub(&dir, &S->P1, &S->P0);

	LINE3 L(S->P0, dir, false);
	f32 t0_, t1_;
	IntersectionResult result;

	result = IntersectionLine3Sphere(&L, sphere, &t0_, &t1_);

	if (result != INTERSECTION_NONE)
	{
		if (result == INTERSECTION_1)
		{
			if (t0_ >= 0.0f && t0_ <= 1.0f)
			{
				if (t0)
					*t0 = t0_;

				return result;
			}
			else
			{
				return INTERSECTION_NONE;
			}
		}

		if (t0_ >= 0.0f)
		{
			if (t1_ <= 1.0f)
			{
				if (t0)
					*t0 = t0_;

				if (t1)
					*t1 = t1_;

				return result;
			}

			if (t0_ <= 1.0f)
			{
				if (t0)
					*t0 = t0_;

				return INTERSECTION_1;
			}
			else
			{
				return INTERSECTION_NONE;
			}
		}

		if (t1_ >= 0.0f && t1_ <= 1.0f)
		{
			if (t0)
				*t0 = t1_;

			return INTERSECTION_1;
		}
		else
		{
			return INTERSECTION_NONE;
		}
	}
	else
	{
		return result;
	}
}

static bool IntersectionRay3AABB_(f32 min, f32 max, f32 o, f32 dir, f32 &tNear,
                                  f32 &tFar)
{
	f32 inv_dir;
	f32 t0;
	f32 t1;

	if (FAbs(dir) < detail::epsilon)
	{
		if (o < min || o > max)
			return false;

		return true;
	}

	inv_dir = 1.0f / dir;
	t0 = inv_dir * (min - o);
	t1 = inv_dir * (max - o);

	if (t0 > t1)
		std::swap(t0, t1);

	if (t0 > tNear)
		tNear = t0;

	if (t1 < tFar)
		tFar = t1;

	if (tNear > tFar)
		return false;

	if (tFar < 0.0f)
		return false;

	return true;
}

bool IntersectionRay3AABB(RAY3 const *R, AABB const *box, f32 *t)
{
    f32 tNear = -detail::infinity;
    f32 tFar = +detail::infinity;
    bool result;

	result = IntersectionRay3AABB_(box->Pmin.x, box->Pmax.x, R->P.x, R->d.x,
	                               tNear, tFar);
	if (!result)
		return false;

	result = IntersectionRay3AABB_(box->Pmin.y, box->Pmax.y, R->P.y, R->d.y,
	                               tNear, tFar);
	if (!result)
		return false;

	result = IntersectionRay3AABB_(box->Pmin.z, box->Pmax.z, R->P.z, R->d.z,
	                               tNear, tFar);
	if (!result)
		return false;

	if (t)
	{
		if (tNear > 0.0f)
			*t = tNear;
		else
			*t = tFar;
	}

	return true;
}

bool IntersectionAABB(AABB const *a, AABB const *b)
{
	if (a->Pmin.x > b->Pmax.x || b->Pmin.x > a->Pmax.x
	 || a->Pmin.y > b->Pmax.y || b->Pmin.y > a->Pmax.y
	 || a->Pmin.z > b->Pmax.z || b->Pmin.z > a->Pmax.z)
	{
		return false;
	}

	return true;
}

bool IntersectionSphereAABB(SPHERE const *sphere, AABB const *aabb)
{
	f32 distSq = 0.0f;

	if (sphere->C.x < aabb->Pmin.x)
		distSq += (sphere->C.x - aabb->Pmin.x) * (sphere->C.x - aabb->Pmin.x);
	else if (sphere->C.x > aabb->Pmax.x)
		distSq += (sphere->C.x - aabb->Pmax.x) * (sphere->C.x - aabb->Pmax.x);

	if (sphere->C.y < aabb->Pmin.y)
		distSq += (sphere->C.y - aabb->Pmin.y) * (sphere->C.y - aabb->Pmin.y);
	else if (sphere->C.y > aabb->Pmax.y)
		distSq += (sphere->C.y - aabb->Pmax.y) * (sphere->C.y - aabb->Pmax.y);

	if (sphere->C.z < aabb->Pmin.z)
		distSq += (sphere->C.z - aabb->Pmin.z) * (sphere->C.z - aabb->Pmin.z);
	else if (sphere->C.z > aabb->Pmax.z)
		distSq += (sphere->C.z - aabb->Pmax.z) * (sphere->C.z - aabb->Pmax.z);


	if (distSq <= sphere->r * sphere->r)
		return true;
	else
		return false;
}

bool IntersectionSphere(SPHERE const *s0, SPHERE const *s1)
{
	VEC3 centerDiff;
	VEC3Sub(&centerDiff, &s0->C, &s1->C);

	f32 radiusSum = s0->r + s1->r;

	if (VEC3LenSq(&centerDiff) <= radiusSum * radiusSum)
		return true;
	else
		return false;
}

bool IntersectionCapsule(CAPSULE const *C0, CAPSULE const *C1)
{
	f32 radiusSum = C0->r + C1->r;

	if (DistSqSegment3ToSegment3(&C0->S, &C1->S, nullptr, nullptr)
	    <= radiusSum * radiusSum)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool IntersectionRay3Capsule(RAY3 const *R, CAPSULE const *C)
{
	if (DistSqRay3ToSegment3(R, &C->S, nullptr, nullptr) <= C->r * C->r)
		return true;
	else
		return false;
}

bool IntersectionLine3Capsule(LINE3 const *L, CAPSULE const *C)
{
	if (DistSqLine3ToSegment3(L, &C->S, nullptr, nullptr) <= C->r * C->r)
		return true;
	else
		return false;
}

bool IntersectionPlaneCapsule(PLANE const *J, CAPSULE const *C)
{
	f32 s0 = J->Test(C->S.P0);
	f32 s1 = J->Test(C->S.P1);

	if (s0 * s1 < 0.0f || FAbs(s0) <= C->r || FAbs(s1) <= C->r)
		return true;
	else
		return false;
}

bool FRUSTUM::IntersectSphere(SPHERE const *S) const
{
	f32 Dist;
	VEC3 viewPos;

	viewPos.z =
		cam._20 * S->C.x + cam._21 * S->C.y + cam._22 * S->C.z + cam._23;

	if (viewPos.z - S->r > near)
		return false;
	if (viewPos.z + S->r < far)
		return false;

	viewPos.x =
		cam._00 * S->C.x + cam._01 * S->C.y + cam._02 * S->C.z + cam._03;

	Dist = viewPos.x * leftPlane.N.x + viewPos.z * leftPlane.N.z;
	if (Dist > S->r)
		return false;

	Dist = viewPos.x * rightPlane.N.x + viewPos.z * rightPlane.N.z;
	if (Dist > S->r)
		return false;

	viewPos.y =
		cam._10 * S->C.x + cam._11 * S->C.y + cam._12 * S->C.z + cam._13;

	Dist = viewPos.y * topPlane.N.y + viewPos.z * topPlane.N.z;
	if (Dist > S->r)
		return false;

	Dist = viewPos.y * bottomPlane.N.y + viewPos.z * bottomPlane.N.z;
	if (Dist > S->r)
		return false;

	return true;
}

bool FRUSTUM::IntersectAABB(AABB const *B) const
{
	if (!IntersectionAABB(B, &box))
		return false;

	int i;
	VEC3 p;

	for (i = 0; i < (int)ARRAY_LENGTH(planes); ++i)
	{
		p.x = planes[i].N.x >= 0.0f ? B->Pmin.x : B->Pmax.x;
		p.y = planes[i].N.y >= 0.0f ? B->Pmin.y : B->Pmax.y;
		p.z = planes[i].N.z >= 0.0f ? B->Pmin.z : B->Pmax.z;

		if (planes[i].Test(p) > 0.0f)
			return false;
	}

	return true;
}

IntersectionResult FRUSTUM::IntersectAABB_Ex(AABB const *B) const
{
	if (!IntersectionAABB(B, &box))
		return INTERSECTION_OUTSIDE;

	IntersectionResult result = INTERSECTION_INSIDE;

	int i;
	VEC3 p, n;

	for (i = 0; i < (int)ARRAY_LENGTH(planes); ++i)
	{
		if (planes[i].N.x >= 0.0f)
		{
			p.x = B->Pmin.x;
			n.x = B->Pmax.x;
		}
		else
		{
			p.x = B->Pmax.x;
			n.x = B->Pmin.x;
		}

		if (planes[i].N.y >= 0.0f)
		{
			p.y = B->Pmin.y;
			n.y = B->Pmax.y;
		}
		else
		{
			p.y = B->Pmax.y;
			n.y = B->Pmin.y;
		}

		if (planes[i].N.z >= 0.0f)
		{
			p.z = B->Pmin.z;
			n.z = B->Pmax.z;
		}
		else
		{
			p.z = B->Pmax.z;
			n.z = B->Pmin.z;
		}

		if (planes[i].Test(p) > 0.0f)
			return INTERSECTION_OUTSIDE;

		if (planes[i].Test(n) > 0.0f)
			result = INTERSECTION_INTERSECT;
	}

	return result;
}

SPHERE *MergeSphere(SPHERE *s2, SPHERE const *s0, SPHERE const *s1)
{
	VEC3 diff;
	VEC3Sub(&diff, &s1->C, &s0->C);

	f32 distsq = VEC3LenSq(&diff);
	f32 radiusDiff = s1->r - s0->r;

	if (distsq <= radiusDiff * radiusDiff)
	{
		if (s0->r > s1->r)
			*s2 = *s0;
		else
			*s2 = *s1;
	}
	else
	{
		f32 dist = FSqrt(distsq);
		f32 newRadius = (s0->r + s1->r + dist) * 0.5f;

		if (FAbs(dist) > detail::epsilon)
		{
			VEC3Add(&s2->C, &s0->C,
			        VEC3Scale(&diff, &diff, (newRadius - s0->r) / dist));
		}
		else
		{
			s2->C = s0->C;
		}

		s2->r = newRadius;
	}

	return s2;
}

AABB *MergeAABB(AABB *a2, AABB const *a0, AABB const *a1)
{
	a2->Pmin.x = std::min(a0->Pmin.x, a1->Pmin.x);
	a2->Pmin.y = std::min(a0->Pmin.y, a1->Pmin.y);
	a2->Pmin.z = std::min(a0->Pmin.z, a1->Pmin.z);

	a2->Pmax.x = std::max(a0->Pmax.x, a1->Pmax.x);
	a2->Pmax.y = std::max(a0->Pmax.y, a1->Pmax.y);
	a2->Pmax.z = std::max(a0->Pmax.z, a1->Pmax.z);

	return a2;
}

}} // namespace nw4hbm::math
