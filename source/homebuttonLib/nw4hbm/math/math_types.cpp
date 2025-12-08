#include "types.h"

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "constants.h"
#include "triangular.h"

// #include <revolution/os.h>
// #include <revolution/mtx.h>

#include <context_rvl.h>

/*******************************************************************************
 * macros
 */

#define qr0				0

// TODO: Is there a way to load constants within an asm block?
/* NOTE: Using a digraph here tricks (my) syntax highlighter into not entering
 * an extra block context and fucking up the rest of the file
 */
#define LFS_(x_, v_)	}(x_)=(v_);asm<%

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace math {

VEC2 *VEC2Maximize(register VEC2 *pOut, register VEC2 const *p1,
                   register VEC2 const *p2)
{
	register f32 a, b, c, d;

#if defined(__MWERKS__)
	asm
	{
		// x
		lfs		a, 0(p1)
		lfs		b, 0(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 0(pOut)

		// y
		lfs		a, 4(p1)
		lfs		b, 4(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 4(pOut)
	}
#else
# pragma unused(p1, p2, a, b, c, d)
#endif

	return pOut;
}

VEC2 *VEC2Minimize(register VEC2 *pOut, register VEC2 const *p1,
                   register VEC2 const *p2)
{
	register f32 a, b, c, d;

#if defined(__MWERKS__)
	asm
	{
		// x
		lfs		a, 0(p1)
		lfs		b, 0(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 0(pOut)

		// y
		lfs		a, 4(p1)
		lfs		b, 4(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 4(pOut)
	}
#else
# pragma unused(p1, p2, a, b, c, d)
#endif

	return pOut;
}

VEC2 *VEC2Normalize(register VEC2 *pOut, register VEC2 const *p)
{
	register f32 /* const */ c_half = 0.5f;
	register f32 /* const */ c_three = 3.0f;
	register f32 v1_xy;
	register f32 xx_yy;
	register f32 sqsum;
	register f32 rsqrt;
	register f32 nwork0, nwork1;

#if defined(__MWERKS__)
	asm
	{
		psq_l		v1_xy, 0(p), false, qr0

		ps_mul		xx_yy, v1_xy, v1_xy
		ps_sum0		sqsum, xx_yy, xx_yy, xx_yy
		frsqrte		rsqrt, sqsum

		fmuls		nwork0, rsqrt, rsqrt
		fmuls		nwork1, rsqrt, c_half
		fnmsubs		nwork0, nwork0, sqsum, c_three
		fmuls		rsqrt, nwork0, nwork1
		ps_muls0	v1_xy, v1_xy, rsqrt

		psq_st		v1_xy, 0(pOut), false, qr0
	}
#else
# pragma unused(p, c_half, c_three, v1_xy, xx_yy, sqsum, rsqrt, nwork0, nwork1)
#endif

	return pOut;
}

VEC3 *VEC3Maximize(register VEC3 *pOut, register VEC3 const *p1,
                   register VEC3 const *p2)
{
	register f32 a, b, c, d;

#if defined(__MWERKS__)
	asm
	{
		// x
		lfs		a, 0(p1)
		lfs		b, 0(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 0(pOut)

		// y
		lfs		a, 4(p1)
		lfs		b, 4(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 4(pOut)

		// z
		lfs		a, 8(p1)
		lfs		b, 8(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 8(pOut)
	}
#else
# pragma unused(p1, p2, a, b, c, d)
#endif

	return pOut;
}

VEC3 *VEC3Minimize(register VEC3 *pOut, register VEC3 const *p1,
                   register VEC3 const *p2)
{
	register f32 a, b, c, d;

#if defined(__MWERKS__)
	asm
	{
		// x
		lfs		a, 0(p1)
		lfs		b, 0(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 0(pOut)

		// y
		lfs		a, 4(p1)
		lfs		b, 4(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 4(pOut)

		// z
		lfs		a, 8(p1)
		lfs		b, 8(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 8(pOut)
	}
#else
# pragma unused(p1, p2, a, b, c, d)
#endif

	return pOut;
}

VEC4 *VEC4Add(register VEC4 *pOut, register VEC4 const *p1,
              register VEC4 const *p2)
{
	register f32 a, b, c;

#if defined(__MWERKS__)
	asm
	{
		// xy
		psq_l	a, 0(p1), false, qr0
		psq_l	b, 0(p2), false, qr0

		ps_add	c, a, b
		psq_st	c, 0(pOut), false, qr0

		// zw
		psq_l	a, 8(p1), false, qr0
		psq_l	b, 8(p2), false, qr0

		ps_add	c, a, b
		psq_st	c, 8(pOut), false, qr0
	}
#else
# pragma unused(p1, p2, a, b, c)
#endif

	return pOut;
}

VEC4 *VEC4Sub(register VEC4 *pOut, register VEC4 const *p1,
              register VEC4 const *p2)
{
	register f32 a, b, c;

#if defined(__MWERKS__)
	asm
	{
		// xy
		psq_l	a, 0(p1), false, qr0
		psq_l	b, 0(p2), false, qr0

		ps_sub	c, a, b
		psq_st	c, 0(pOut), false, qr0

		// zw
		psq_l	a, 8(p1), false, qr0
		psq_l	b, 8(p2), false, qr0

		ps_sub	c, a, b
		psq_st	c, 8(pOut), false, qr0
	}
#else
# pragma unused(p1, p2, a, b, c)
#endif

	return pOut;
}

VEC4 *VEC4Scale(register VEC4 *pOut, register VEC4 const *p, register f32 scale)
{
	register f32 a, b;

#if defined(__MWERKS__)
	asm
	{
		// xy
		psq_l		a, 0(p), false, qr0
		ps_muls0	b, a, scale
		psq_st		b, 0(pOut), false, qr0

		// zw
		psq_l		a, 8(p), false, qr0
		ps_muls0	b, a, scale
		psq_st		b, 8(pOut), false, qr0
	}
#else
# pragma unused(p, scale, a, b)
#endif

	return pOut;
}

VEC4 *VEC4Lerp(VEC4 *pOut, VEC4 const *p1, VEC4 const *p2, f32 t)
{
	pOut->x = p1->x + (p2->x - p1->x) * t;
	pOut->y = p1->y + (p2->y - p1->y) * t;
	pOut->z = p1->z + (p2->z - p1->z) * t;
	pOut->w = p1->w + (p2->w - p1->w) * t;

	return pOut;
}

f32 VEC4Dot(register VEC4 const *p1, register VEC4 const *p2)
{
	register f32 _v1, _v2, _v3, _v4, _v5;

#if defined(__MWERKS__)
	asm
	{
		// xy
		psq_l	_v2, 0(p1), false, qr0
		psq_l	_v3, 0(p2), false, qr0

		ps_mul	_v2, _v2, _v3

		// zw
		psq_l	_v5, 8(p1), false, qr0
		psq_l	_v4, 8(p2), false, qr0

		ps_mul	_v5, _v5, _v4
		ps_add	_v1, _v2, _v5
		ps_sum0	_v1, _v1, _v1, _v1
	}
#else
# pragma unused(p1, p2, _v2, _v3, _v4, _v5)
	_v1 = 0;
#endif

	return _v1;
}

f32 VEC4LenSq(register VEC4 const *p)
{
	register f32 vxy, vzw, sqmag;

#if defined(__MWERKS__)
	asm
	{
		psq_l	vxy, 0(p), false, qr0
		ps_mul	vxy, vxy, vxy

		psq_l	vzw, 8(p), false, qr0
		ps_madd	sqmag, vzw, vzw, vxy
		ps_sum0	sqmag, sqmag, sqmag, sqmag
	}
#else
# pragma unused(p, vxy, vzw)
	sqmag = 0;
#endif

	return sqmag;
}

f32 VEC4Len(register VEC4 const *p)
{
	register f32 vxy, vzw, sqmag;
	register f32 rmag, nwork0, nwork1;
	register f32 /* const */ c_three, c_half, c_zero;

#if defined(__MWERKS__)
	asm
	{
		LFS_	(c_half, 0.5f)
		psq_l	vxy, 0(p), false, qr0
		ps_mul	vxy, vxy, vxy
		psq_l	vzw, 8(p), false, qr0
		fsubs	c_zero, c_half, c_half

		ps_madd	sqmag, vzw, vzw, vxy
		ps_sum0	sqmag, sqmag, sqmag, sqmag

		fcmpu	sqmag, c_zero
		beq		@1

		frsqrte	rmag, sqmag
		LFS_	(c_three, 3.0f)
		fmuls	nwork0, rmag, rmag
		fmuls	nwork1, rmag, c_half
		fnmsubs	nwork0, nwork0, sqmag, c_three
		fmuls	rmag, nwork0, nwork1
		fmuls	sqmag, sqmag, rmag
	@1:
	}
#else
# pragma unused(p, vxy, vzw, rmag, nwork0, nwork1, c_three, c_half, c_zero)
	sqmag = 0;
#endif

	return sqmag;
}

VEC4 *VEC4Normalize(register VEC4 *pOut, register VEC4 const *p)
{
	register f32 /* const */ c_half = 0.5f;
	register f32 /* const */ c_three = 3.0f;
	register f32 v1_xy, v1_zw;
	register f32 xx_zz_yy_ww, xx_yy;
	register f32 sqsum;
	register f32 rsqrt;
	register f32 nwork0, nwork1;

#if defined(__MWERKS__)
	asm
	{
		psq_l		v1_xy, 0(p), false, qr0
		ps_mul		xx_yy, v1_xy, v1_xy
		psq_l		v1_zw, 8(p), false, qr0
		ps_madd		xx_zz_yy_ww, v1_zw, v1_zw, xx_yy
		ps_sum0		sqsum, xx_zz_yy_ww, xx_zz_yy_ww, xx_zz_yy_ww
		frsqrte		rsqrt, sqsum

		fmuls		nwork0, rsqrt, rsqrt
		fmuls		nwork1, rsqrt, c_half
		fnmsubs		nwork0, nwork0, sqsum, c_three
		fmuls		rsqrt, nwork0, nwork1
		ps_muls0	v1_xy, v1_xy, rsqrt
		psq_st		v1_xy, 0(pOut), false, qr0
		ps_muls0	v1_zw, v1_zw, rsqrt
		psq_st		v1_zw, 8(pOut), false, qr0
	}
#else
# pragma unused(p, c_half, c_three, v1_xy, v1_zw, xx_zz_yy_ww, xx_yy, sqsum)
# pragma unused(rsqrt, nwork0, nwork1)
#endif

	return pOut;
}

f32 VEC4DistSq(register VEC4 const *p1, register VEC4 const *p2)
{
	register f32 _v1, _v2, vxy, vzw, sqmag;

#if defined(__MWERKS__)
	asm
	{
		// xy
		psq_l	_v1, 0(p1), false, qr0
		psq_l	_v2, 0(p2), false, qr0
		ps_sub	vxy, _v1, _v2
		ps_mul	vxy, vxy, vxy

		// zw
		psq_l	_v1, 8(p1), false, qr0
		psq_l	_v2, 8(p2), false, qr0
		ps_sub	vzw, _v1, _v2
		ps_madd	sqmag, vzw, vzw, vxy
		ps_sum0	sqmag, sqmag, sqmag, sqmag
	}
#else
# pragma unused(p1, p2, _v1, _v2, vxy, vzw)
	sqmag = 0;
#endif

	return sqmag;
}

VEC4 *VEC4Maximize(register VEC4 *pOut, register VEC4 const *p1,
                   register VEC4 const *p2)
{
	register f32 a, b, c, d;

#if defined(__MWERKS__)
	asm
	{
		// x
		lfs		a, 0(p1)
		lfs		b, 0(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 0(pOut)

		// y
		lfs		a, 4(p1)
		lfs		b, 4(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 4(pOut)

		// z
		lfs		a, 8(p1)
		lfs		b, 8(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 8(pOut)

		// w
		lfs		a, 12(p1)
		lfs		b, 12(p2)
		fsubs	c, a, b
		fsel	d, c, a, b
		stfs	d, 12(pOut)
	}
#else
# pragma unused(p1, p2, a, b, c, d)
#endif

	return pOut;
}

VEC4 *VEC4Minimize(register VEC4 *pOut, register VEC4 const *p1,
                   register VEC4 const *p2)
{
	register f32 a, b, c, d;

#if defined(__MWERKS__)
	asm
	{
		// x
		lfs		a, 0(p1)
		lfs		b, 0(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 0(pOut)

		// y
		lfs		a, 4(p1)
		lfs		b, 4(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 4(pOut)

		// z
		lfs		a, 8(p1)
		lfs		b, 8(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 8(pOut)

		// w
		lfs		a, 12(p1)
		lfs		b, 12(p2)
		fsubs	c, b, a
		fsel	d, c, a, b
		stfs	d, 12(pOut)
	}
#else
# pragma unused(p1, p2, a, b, c, d)
#endif

	return pOut;
}

MTX33 *MTX33Copy(register MTX33 *pOut, register MTX33 const *p)
{
	register f32 _fp0, _fp1, _fp2, _fp3, _fp4;

#if defined(__MWERKS__)
	asm
	{
		psq_l	_fp0, 0(p), false, qr0
		psq_st	_fp0, 0(pOut), false, qr0
		psq_l	_fp1, 8(p), false, qr0
		psq_st	_fp1, 8(pOut), false, qr0
		psq_l	_fp2, 16(p), false, qr0
		psq_st	_fp2, 16(pOut), false, qr0
		psq_l	_fp3, 24(p), false, qr0
		psq_st	_fp3, 24(pOut), false, qr0
		psq_l	_fp4, 32(p), true, qr0
		psq_st	_fp4, 32(pOut), true, qr0
	}
#else
# pragma unused(p, _fp0, _fp1, _fp2, _fp3, _fp4)
#endif

	return pOut;
}

MTX33 *MTX33Zero(register MTX33 *pOut)
{
	register f32 zero = 0.0f;

#if defined(__MWERKS__)
	asm
	{
		psq_st	zero, 0(pOut), false, qr0
		psq_st	zero, 8(pOut), false, qr0
		psq_st	zero, 16(pOut), false, qr0
		psq_st	zero, 24(pOut), false, qr0
		psq_st	zero, 32(pOut), true, qr0
	}
#else
# pragma unused(zero)
#endif

	return pOut;
}

MTX33 *MTX33Identity(register MTX33 *pOut)
{
	register f32 /* const */ c_00 = 0.0f;
	register f32 /* const */ c_11 = 1.0f;
	register f32 c_10;

#if defined(__MWERKS__)
	asm
	{
		ps_merge00	c_10, c_11, c_00

		psq_st		c_10, 0(pOut), false, qr0
		psq_st		c_00, 8(pOut), false, qr0
		psq_st		c_10, 16(pOut), false, qr0
		psq_st		c_00, 24(pOut), false, qr0
		stfs		c_11, 32(pOut)
	}
#else
# pragma unused(c_00, c_11, c_10)
#endif

	return pOut;
}

MTX33 *MTX34ToMTX33(register MTX33 *pOut, register MTX34 const *pM)
{
	register f32 _fp0, _fp1, _fp2, _fp3, _fp4, _fp5;

#if defined(__MWERKS__)
	asm
	{
		psq_l	_fp0, 0(pM), false, qr0
		psq_st	_fp0, 0(pOut), false, qr0
		psq_l	_fp1, 8(pM), false, qr0
		psq_st	_fp1, 8(pOut), true, qr0
		psq_l	_fp2, 16(pM), false, qr0
		psq_st	_fp2, 12(pOut), false, qr0
		psq_l	_fp3, 24(pM), false, qr0
		psq_st	_fp3, 20(pOut), true, qr0
		psq_l	_fp4, 32(pM), false, qr0
		psq_st	_fp4, 24(pOut), false, qr0
		psq_l	_fp5, 40(pM), false, qr0
		psq_st	_fp5, 32(pOut), true, qr0
	}
#else
# pragma unused(pM, _fp0, _fp1, _fp2, _fp3, _fp4, _fp5)
#endif

	return pOut;
}

MTX33 *MTX33MAdd(register MTX33 *pOut, register f32 t, register MTX33 const *p1,
                 register MTX33 const *p2)
{
	register f32 a, b, c;

#if defined(__MWERKS__)
	asm
	{
		psq_l		a, 0(p1), false, qr0
		psq_l		b, 0(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 0(pOut), false, qr0

		psq_l		a, 8(p1), false, qr0
		psq_l		b, 8(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 8(pOut), false, qr0

		psq_l		a, 16(p1), false, qr0
		psq_l		b, 16(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 16(pOut), false, qr0

		psq_l		a, 24(p1), false, qr0
		psq_l		b, 24(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 24(pOut), false, qr0

		psq_l		a, 32(p1), true, qr0
		psq_l		b, 32(p2), true, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 32(pOut), true, qr0
	}
#else
# pragma unused(t, p1, p2, a, b, c)
#endif

	return pOut;
}

#if defined(__MWERKS__)
asm u32 MTX34InvTranspose(register MTX33 *inv, register MTX34 const *src)
{
	nofralloc

	psq_l		f0, 0(src), true, qr0
	psq_l		f1, 4(src), false, qr0
	psq_l		f2, 16(src), true, qr0
	ps_merge10	f6, f1, f0
	psq_l		f3, 20(src), false, qr0
	psq_l		f4, 32(src), true, qr0
	ps_merge10	f7, f3, f2
	psq_l		f5, 36(src), false, qr0
	ps_mul		f11, f3, f6
	ps_merge10	f8, f5, f4
	ps_mul		f13, f5, f7
	ps_msub		f11, f1, f7, f11
	ps_mul		f12, f1, f8
	ps_msub		f13, f3, f8, f13
	ps_msub		f12, f5, f6, f12
	ps_mul		f10, f3, f4
	ps_mul		f9, f0, f5
	ps_mul		f8, f1, f2
	ps_msub		f10, f2, f5, f10
	ps_msub		f9, f1, f4, f9
	ps_msub		f8, f0, f3, f8
	ps_mul		f7, f0, f13
	ps_sub		f1, f1, f1
	ps_madd		f7, f2, f12, f7
	ps_madd		f7, f4, f11, f7
	ps_cmpo0	cr0, f7, f1
	bne			@1
	li			r3, false
	blr
@1:	fres		f0, f7
	ps_add		f6, f0, f0
	ps_mul		f5, f0, f0
	ps_nmsub	f0, f7, f5, f6
	ps_add		f6, f0, f0
	ps_mul		f5, f0, f0
	ps_nmsub	f0, f7, f5, f6
	ps_muls0	f13, f13, f0
	ps_muls0	f12, f12, f0
	psq_st		f13, 0(inv), false, qr0
	ps_muls0	f11, f11, f0
	psq_st		f12, 12(inv), false, qr0
	ps_muls0	f10, f10, f0
	psq_st		f11, 24(inv), false, qr0
	ps_muls0	f9, f9, f0
	psq_st		f10, 8(inv), true, qr0
	ps_muls0	f8, f8, f0
	psq_st		f9, 20(inv), true, qr0
	psq_st		f8, 32(inv), true, qr0
	li			r3, true
	blr
}
#endif

MTX34 *MTX34Zero(register MTX34 *pOut)
{
	register f32 zero = 0.0f;

#if defined(__MWERKS__)
	asm
	{
		psq_st	zero, 0(pOut), false, qr0
		psq_st	zero, 8(pOut), false, qr0
		psq_st	zero, 16(pOut), false, qr0
		psq_st	zero, 24(pOut), false, qr0
		psq_st	zero, 32(pOut), false, qr0
		psq_st	zero, 40(pOut), false, qr0
	}
#else
# pragma unused(zero)
#endif

	return pOut;
}

bool MTX34IsIdentity(MTX34 const *p)
{
	return p->_00 == 1.0f && p->_01 == 0.0f && p->_02 == 0.0f && p->_03 == 0.0f
	    && p->_10 == 0.0f && p->_11 == 1.0f && p->_12 == 0.0f && p->_13 == 0.0f
	    && p->_20 == 0.0f && p->_21 == 0.0f && p->_22 == 1.0f && p->_23 == 0.0f;
}

MTX34 *MTX34Add(register MTX34 *pOut, register MTX34 const *p1,
                register MTX34 const *p2)
{
	register f32 a, b, c;

#if defined(__MWERKS__)
	asm
	{
		psq_l	a, 0(p1), false, qr0
		psq_l	b, 0(p2), false, qr0
		ps_add	c, a, b
		psq_st	c, 0(pOut), false, qr0

		psq_l	a, 8(p1), false, qr0
		psq_l	b, 8(p2), false, qr0
		ps_add	c, a, b
		psq_st	c, 8(pOut), false, qr0

		psq_l	a, 16(p1), false, qr0
		psq_l	b, 16(p2), false, qr0
		ps_add	c, a, b
		psq_st	c, 16(pOut), false, qr0

		psq_l	a, 24(p1), false, qr0
		psq_l	b, 24(p2), false, qr0
		ps_add	c, a, b
		psq_st	c, 24(pOut), false, qr0

		psq_l	a, 32(p1), false, qr0
		psq_l	b, 32(p2), false, qr0
		ps_add	c, a, b
		psq_st	c, 32(pOut), false, qr0

		psq_l	a, 40(p1), false, qr0
		psq_l	b, 40(p2), false, qr0
		ps_add	c, a, b
		psq_st	c, 40(pOut), false, qr0
	}
#else
# pragma unused(p1, p2, a, b, c)
#endif

	return pOut;
}

MTX34 *MTX34Sub(register MTX34 *pOut, register MTX34 const *p1,
                register MTX34 const *p2)
{
	register f32 a, b, c;

#if defined(__MWERKS__)
	asm
	{
		psq_l	a, 0(p1), false, qr0
		psq_l	b, 0(p2), false, qr0
		ps_sub	c, a, b
		psq_st	c, 0(pOut), false, qr0

		psq_l	a, 8(p1), false, qr0
		psq_l	b, 8(p2), false, qr0
		ps_sub	c, a, b
		psq_st	c, 8(pOut), false, qr0

		psq_l	a, 16(p1), false, qr0
		psq_l	b, 16(p2), false, qr0
		ps_sub	c, a, b
		psq_st	c, 16(pOut), false, qr0

		psq_l	a, 24(p1), false, qr0
		psq_l	b, 24(p2), false, qr0
		ps_sub	c, a, b
		psq_st	c, 24(pOut), false, qr0

		psq_l	a, 32(p1), false, qr0
		psq_l	b, 32(p2), false, qr0
		ps_sub	c, a, b
		psq_st	c, 32(pOut), false, qr0

		psq_l	a, 40(p1), false, qr0
		psq_l	b, 40(p2), false, qr0
		ps_sub	c, a, b
		psq_st	c, 40(pOut), false, qr0
	}
#else
# pragma unused(p1, p2, a, b, c)
#endif

	return pOut;
}

MTX34 *MTX34Mult(register MTX34 *pOut, register MTX34 const *p, register f32 f)
{
	register f32 a, b;

#if defined(__MWERKS__)
	asm
	{
		psq_l		a, 0(p), false, qr0
		ps_muls0	b, a, f
		psq_st		b, 0(pOut), false, qr0

		psq_l		a, 8(p), false, qr0
		ps_muls0	b, a, f
		psq_st		b, 8(pOut), false, qr0

		psq_l		a, 16(p), false, qr0
		ps_muls0	b, a, f
		psq_st		b, 16(pOut), false, qr0

		psq_l		a, 24(p), false, qr0
		ps_muls0	b, a, f
		psq_st		b, 24(pOut), false, qr0

		psq_l		a, 32(p), false, qr0
		ps_muls0	b, a, f
		psq_st		b, 32(pOut), false, qr0

		psq_l		a, 40(p), false, qr0
		ps_muls0	b, a, f
		psq_st		b, 40(pOut), false, qr0
	}
#else
# pragma unused(p, f, a, b)
#endif

	return pOut;
}

MTX34 *MTX34Scale(register MTX34 *pOut, register MTX34 const *pM,
                  register VEC3 const *pS)
{
	register f32 xy, z1;
	register f32 row0a, row0b;
	register f32 row1a, row1b;
	register f32 row2a, row2b;

#if defined(__MWERKS__)
	asm
	{
		psq_l xy, 0(pS), false, qr0
		psq_l z1, 8(pS), true, qr0

		psq_l row0a, 0(pM), false, qr0
		psq_l row0b, 8(pM), false, qr0
		psq_l row1a, 16(pM), false, qr0
		psq_l row1b, 24(pM), false, qr0
		psq_l row2a, 32(pM), false, qr0
		psq_l row2b, 40(pM), false, qr0

		ps_mul row0a, row0a, xy
		ps_mul row1a, row1a, xy
		ps_mul row2a, row2a, xy
		ps_mul row0b, row0b, z1
		ps_mul row1b, row1b, z1
		ps_mul row2b, row2b, z1

		psq_st row0a, 0(pOut), false, qr0
		psq_st row0b, 8(pOut), false, qr0
		psq_st row1a, 16(pOut), false, qr0
		psq_st row1b, 24(pOut), false, qr0
		psq_st row2a, 32(pOut), false, qr0
		psq_st row2b, 40(pOut), false, qr0
	}
#else
# pragma unused(pM, pS, xy, z1, row0a, row0b, row1a, row1b, row2a, row2b)
#endif

	return pOut;
}

MTX34 *MTX34Trans(register MTX34 *pOut, register MTX34 const *pM,
                  register VEC3 const *pT)
{
	register f32 xy, z1;
	register f32 vv0, vv1, vv2, vv3, vv4, vv5;
	register f32 tmp0, tmp1, tmp2;

#if defined(__MWERKS__)
	asm
	{
		psq_l	vv0, 0(pM), false, qr0
		psq_st	vv0, 0(pOut), false, qr0
		psq_l	vv1, 8(pM), false, qr0
		psq_st	vv1, 8(pOut), false, qr0
		psq_l	vv2, 16(pM), false, qr0
		psq_st	vv2, 16(pOut), false, qr0
		psq_l	vv3, 24(pM), false, qr0
		psq_st	vv3, 24(pOut), false, qr0
		psq_l	vv4, 32(pM), false, qr0
		psq_st	vv4, 32(pOut), false, qr0
		psq_l	vv5, 40(pM), false, qr0
		psq_st	vv5, 40(pOut), false, qr0

		psq_l	xy, 0(pT), false, qr0
		psq_l	z1, 8(pT), true, qr0

		ps_mul	tmp0, vv0, xy
		ps_madd	tmp1, vv1, z1, tmp0
		ps_sum0	tmp2, tmp1, tmp2, tmp1
		psq_st	tmp2, 12(pOut), true, qr0

		ps_mul	tmp0, vv2, xy
		ps_madd	tmp1, vv3, z1, tmp0
		ps_sum0	tmp2, tmp1, tmp2, tmp1
		psq_st	tmp2, 28(pOut), true, qr0

		ps_mul	tmp0, vv4, xy
		ps_madd	tmp1, vv5, z1, tmp0
		ps_sum0	tmp2, tmp1, tmp2, tmp1
		psq_st	tmp2, 44(pOut), true, qr0
	}
#else
# pragma unused(pM, pT, xy, z1, vv0, vv1, vv2, vv3, vv4, vv5, tmp0, tmp1, tmp2)
#endif

	return pOut;
}

MTX34 *MTX34MAdd(register MTX34 *pOut, register f32 t, register MTX34 const *p1, register MTX34 const *p2)
{
	register f32 a, b, c;

#if defined(__MWERKS__)
	asm
	{
		psq_l		a, 0(p1), false, qr0
		psq_l		b, 0(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 0(pOut), false, qr0

		psq_l		a, 8(p1), false, qr0
		psq_l		b, 8(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 8(pOut), false, qr0

		psq_l		a, 16(p1), false, qr0
		psq_l		b, 16(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 16(pOut), false, qr0

		psq_l		a, 24(p1), false, qr0
		psq_l		b, 24(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 24(pOut), false, qr0

		psq_l		a, 32(p1), false, qr0
		psq_l		b, 32(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 32(pOut), false, qr0

		psq_l		a, 40(p1), false, qr0
		psq_l		b, 40(p2), false, qr0
		ps_muls0	a, a, t
		ps_add		c, a, b
		psq_st		c, 40(pOut), false, qr0
	}
#else
# pragma unused(t, p1, p2, a, b, c)
#endif

	return pOut;
}

MTX34 *MTX34RotAxisFIdx(MTX34 *pOut, VEC3 const *pAxis, f32 fIdx)
{
	MTXRotAxisRad(*pOut, *pAxis, fIdx * convert::FIdx2Rad);

	return pOut;
}

MTX34 *MTX34RotXYZFIdx(MTX34 *pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ)
{
	f32 sinx, cosx;
	f32 siny, cosy;
	f32 sinz, cosz;
	f32 f1, f2;

	SinCosFIdx(&sinx, &cosx, fIdxX);
	SinCosFIdx(&siny, &cosy, fIdxY);
	SinCosFIdx(&sinz, &cosz, fIdxZ);

	pOut->_20 = -siny;

	pOut->_00 = cosz * cosy;
	pOut->_10 = sinz * cosy;

	pOut->_21 = cosy * sinx;
	pOut->_22 = cosy * cosx;

	f1 = cosx * sinz;
	f2 = sinx * cosz;

	pOut->_01 = f2 * siny - f1;
	pOut->_12 = f1 * siny - f2;

	f1 = sinx * sinz;
	f2 = cosx * cosz;

	pOut->_02 = f2 * siny + f1;
	pOut->_11 = f1 * siny + f2;

	pOut->_03 = 0.0f;
	pOut->_13 = 0.0f;
	pOut->_23 = 0.0f;

	return pOut;
}

MTX34 *MTX34RotXYZTransFIdx(MTX34 *pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ,
                            VEC3 const *pT)
{
	MTX34RotXYZFIdx(pOut, fIdxX, fIdxY, fIdxZ);

	pOut->_03 = pT->x;
	pOut->_13 = pT->y;
	pOut->_23 = pT->z;

	return pOut;
}

VEC3 *VEC3TransformNormal(VEC3 *pOut, MTX34 const *pM, VEC3 const *pV)
{
	VEC3 tmp;

	tmp.x = pM->_00 * pV->x + pM->_01 * pV->y + pM->_02 * pV->z;
	tmp.y = pM->_10 * pV->x + pM->_11 * pV->y + pM->_12 * pV->z;
	tmp.z = pM->_20 * pV->x + pM->_21 * pV->y + pM->_22 * pV->z;

	pOut->x = tmp.x;
	pOut->y = tmp.y;
	pOut->z = tmp.z;

	return pOut;
}

VEC3 *VEC3TransformNormalArray(VEC3 *pOut, MTX34 const *pM, VEC3 const *pV,
                               u32 count)
{
	u32 i;
	VEC3 tmp;
	VEC3 *pDst = pOut;

	/* Technically this means that the original probably had all three
	 * increments at the end of the loop instead of in the iteration-expression,
	 * but it's neater to just put them there anyways with a void cast.
	 */
	for (i = 0; i < count; ++pV, ++pDst, (void)++i)
	{
		tmp.x = pM->_00 * pV->x + pM->_01 * pV->y + pM->_02 * pV->z;
		tmp.y = pM->_10 * pV->x + pM->_11 * pV->y + pM->_12 * pV->z;
		tmp.z = pM->_20 * pV->x + pM->_21 * pV->y + pM->_22 * pV->z;

		pDst->x = tmp.x;
		pDst->y = tmp.y;
		pDst->z = tmp.z;
	}

	return pOut;
}

MTX44 *MTX44Zero(register MTX44 *pOut)
{
	register f32 zero = 0.0f;

#if defined(__MWERKS__)
	asm
	{
		psq_st	zero, 0(pOut), false, qr0
		psq_st	zero, 8(pOut), false, qr0
		psq_st	zero, 16(pOut), false, qr0
		psq_st	zero, 24(pOut), false, qr0
		psq_st	zero, 32(pOut), false, qr0
		psq_st	zero, 40(pOut), false, qr0
		psq_st	zero, 48(pOut), false, qr0
		psq_st	zero, 56(pOut), false, qr0
	}
#else
# pragma unused(zero)
#endif

	return pOut;
}

MTX44 *MTX44Identity(register MTX44 *pOut)
{
	register f32 /* const */ c_zero = 0.0f;
	register f32 /* const */ c_one = 1.0f;
	register f32 c_01;
	register f32 c_10;

#if defined(__MWERKS__)
	asm
	{
		psq_st		c_zero, 8(pOut), false, qr0
		ps_merge01	c_01, c_zero, c_one
		psq_st		c_zero, 24(pOut), false, qr0
		ps_merge10	c_10, c_one, c_zero
		psq_st		c_zero, 32(pOut), false, qr0
		psq_st		c_01, 16(pOut), false, qr0
		psq_st		c_10, 0(pOut), false, qr0
		psq_st		c_10, 40(pOut), false, qr0
		psq_st		c_zero, 48(pOut), false, qr0
		psq_st		c_01, 56(pOut), false, qr0
	}
#else
# pragma unused(c_zero, c_one, c_01, c_10)
#endif

	return pOut;
}

MTX44 *MTX44Copy(register MTX44 *pOut, register MTX44 const *p)
{
	register f32 _0, _1, _2, _3, _4, _5, _6, _7;

#if defined(__MWERKS__)
	asm
	{
		psq_l	_0, 0(p), false, qr0
		psq_st	_0, 0(pOut), false, qr0
		psq_l	_1, 8(p), false, qr0
		psq_st	_1, 8(pOut), false, qr0
		psq_l	_2, 16(p), false, qr0
		psq_st	_2, 16(pOut), false, qr0
		psq_l	_3, 24(p), false, qr0
		psq_st	_3, 24(pOut), false, qr0
		psq_l	_4, 32(p), false, qr0
		psq_st	_4, 32(pOut), false, qr0
		psq_l	_5, 40(p), false, qr0
		psq_st	_5, 40(pOut), false, qr0
		psq_l	_6, 48(p), false, qr0
		psq_st	_6, 48(pOut), false, qr0
		psq_l	_7, 56(p), false, qr0
		psq_st	_7, 56(pOut), false, qr0
	}
#else
# pragma unused(p, _0, _1, _2, _3, _4, _5, _6, _7)
#endif

	return pOut;
}

bool MTX44IsIdentity(MTX44 const *p)
{
	return p->_00 == 1.0f && p->_01 == 0.0f && p->_02 == 0.0f && p->_03 == 0.0f
	    && p->_10 == 0.0f && p->_11 == 1.0f && p->_12 == 0.0f && p->_13 == 0.0f
	    && p->_20 == 0.0f && p->_21 == 0.0f && p->_22 == 1.0f && p->_23 == 0.0f
	    && p->_30 == 0.0f && p->_31 == 0.0f && p->_32 == 0.0f && p->_33 == 1.0f;
}

VEC4 *VEC3Transform(VEC4 *pOut, MTX44 const *pM, VEC3 const *pV)
{
	VEC4 tmp;

	tmp.x = pM->_00 * pV->x + pM->_01 * pV->y + pM->_02 * pV->z + pM->_03;
	tmp.y = pM->_10 * pV->x + pM->_11 * pV->y + pM->_12 * pV->z + pM->_13;
	tmp.z = pM->_20 * pV->x + pM->_21 * pV->y + pM->_22 * pV->z + pM->_23;
	tmp.w = pM->_30 * pV->x + pM->_31 * pV->y + pM->_32 * pV->z + pM->_33;

	pOut->x = tmp.x;
	pOut->y = tmp.y;
	pOut->z = tmp.z;
	pOut->w = tmp.w;

	return pOut;
}

VEC3 *VEC3TransformCoord(VEC3 *pOut, MTX44 const *pM, VEC3 const *pV)
{
	VEC3 tmp;
	f32 rw;

	tmp.x = pM->_00 * pV->x + pM->_01 * pV->y + pM->_02 * pV->z + pM->_03;
	tmp.y = pM->_10 * pV->x + pM->_11 * pV->y + pM->_12 * pV->z + pM->_13;
	tmp.z = pM->_20 * pV->x + pM->_21 * pV->y + pM->_22 * pV->z + pM->_23;

	rw = pM->_30 * pV->x + pM->_31 * pV->y + pM->_32 * pV->z + pM->_33;
	rw = 1.0f / rw;

	pOut->x = rw * tmp.x;
	pOut->y = rw * tmp.y;
	pOut->z = rw * tmp.z;

	return pOut;
}

VEC4 *VEC3TransformArray(VEC4 *pOut, MTX44 const *pM, VEC3 const *pV, u32 count)
{
	u32 i;
	VEC4 *pDst = pOut;

	for (i = 0; i < count; ++pDst, ++pV, (void)++i)
		VEC3Transform(pDst, pM, pV);

	return pOut;
}

VEC3 *VEC3TransformCoordArray(VEC3 *pOut, MTX44 const *pM, VEC3 const *pV,
                              u32 count)
{
	u32 i;
	VEC3 *pDst = pOut;

	for (i = 0; i < count; ++pDst, ++pV, (void)++i)
		VEC3TransformCoord(pDst, pM, pV);

	return pOut;
}

VEC4 *VEC4Transform(VEC4 *pOut, MTX44 const *pM, VEC4 const *pV)
{
	VEC4 tmp;

	tmp.x = pM->_00 * pV->x + pM->_01 * pV->y + pM->_02 * pV->z + pM->_03 * pV->w;
	tmp.y = pM->_10 * pV->x + pM->_11 * pV->y + pM->_12 * pV->z + pM->_13 * pV->w;
	tmp.z = pM->_20 * pV->x + pM->_21 * pV->y + pM->_22 * pV->z + pM->_23 * pV->w;
	tmp.w = pM->_30 * pV->x + pM->_31 * pV->y + pM->_32 * pV->z + pM->_33 * pV->w;

	pOut->x = tmp.x;
	pOut->y = tmp.y;
	pOut->z = tmp.z;
	pOut->w = tmp.w;

	return pOut;
}

VEC4 *VEC4TransformArray(VEC4 *pOut, MTX44 const *pM, VEC4 const *pV, u32 count)
{
	u32 i;
	VEC4 *pDst = pOut;

	for (i = 0; i < count; ++pDst, ++pV, (void)++i)
		VEC4Transform(pDst, pM, pV);

	return pOut;
}

void VEC2::Report(bool bNewline, char const *name) const
{
	if (name)
		OSReport("%s", name);
	else
		OSReport("VEC2");

	OSReport("<%f, %f>", x, y);

	if (bNewline)
		OSReport("\n");
}

void VEC3::Report(bool bNewline, char const *name) const
{
	if (name)
		OSReport("%s", name);
	else
		OSReport("VEC3");

	OSReport("<%f, %f, %f>", x, y, z);

	if (bNewline)
		OSReport("\n");
}

void VEC4::Report(bool bNewline, char const *name) const
{
	if (name)
		OSReport("%s", name);
	else
		OSReport("VEC4");

	OSReport("<%f, %f, %f, %f>", x, y, z, w);

	if (bNewline)
		OSReport("\n");
}

void QUAT::Report(bool bNewline, char const *name) const
{
	if (name)
		OSReport("%s", name);
	else
		OSReport("QUAT");

	OSReport("<%f, %f, %f, %f>", x, y, z, w);

	if (bNewline)
		OSReport("\n");
}

void MTX33::Report(bool bNewline, char const *name) const
{
	if (name)
		OSReport("%s", name);
	else
		OSReport("MTX33");

	OSReport(     "<%f, %f, %f\n",  _00, _01, _02);
	OSReport("      %f, %f, %f\n",  _10, _11, _12);
	OSReport("      %f, %f, %f>\n", _20, _21, _22);

	if (bNewline)
		OSReport("\n");
}

void MTX34::Report(bool bNewline, char const *name) const
{
	if (name)
		OSReport("%s", name);
	else
		OSReport("MTX34");

	OSReport(     "<%f, %f, %f, %f\n",  _00, _01, _02, _03);
	OSReport("      %f, %f, %f, %f\n",  _10, _11, _12, _13);
	OSReport("      %f, %f, %f, %f>\n", _20, _21, _22, _23);

	if (bNewline)
		OSReport("\n");
}

void MTX44::Report(bool bNewline, char const *name) const
{
	if (name)
		OSReport("%s", name);
	else
		OSReport("MTX44");

	OSReport(     "<%f, %f, %f, %f\n",  _00, _01, _02, _03);
	OSReport("      %f, %f, %f, %f\n",  _10, _11, _12, _13);
	OSReport("      %f, %f, %f, %f\n",  _20, _21, _22, _23);
	OSReport("      %f, %f, %f, %f>\n", _30, _31, _32, _33);

	if (bNewline)
		OSReport("\n");
}

}} // namespace nw4hbm::math
