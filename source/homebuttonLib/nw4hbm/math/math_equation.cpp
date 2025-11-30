#include "equation.h"

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "arithmetic.h"
#include "constants.h"
#include "triangular.h"

/*******************************************************************************
 * local function declarations
 */

namespace nw4hbm { namespace math { namespace
{
	template <int n>
	inline f32 spow(f32 x)
	{
		return x * spow<n - 1>(x);
	}

	template <>
	inline f32 spow<1>(f32 x)
	{
		return x;
	}

	int SolveEquation2(f32 *root, f32 b, f32 c);
	f32 SolveEquation3(f32 b, f32 c, f32 d);
}}} // namespace nw4hbm::math::(unnamed)

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace math {

namespace {

int SolveEquation2(f32 *root, f32 b, f32 c)
{
	if (b == 0.0f)
	{
		if (c < -2e-4f)
		{
			f32 r_c = FSqrt(-c);

			root[0] = r_c;
			root[1] = -r_c;

			return 2;
		}
		else if (c <= 2e-4f)
		{
			root[0] = 0.0f;

			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		f32 A = b / 2.0f;
		f32 B = c / spow<2>(A);
		f32 D = 1.0f - B;

		if (D > 0.0f)
		{
			f32 C = -1.0f - FSqrt(D);

			root[0] = A * C;
			root[1] = A * B / C;

			return 2;
		}
		else if (FAbs(D) < 2e-4f)
		{
			root[0] = -A;

			return 1;
		}
		else
		{
			return 0;
		}
	}
}

f32 SolveEquation3(f32 b, f32 c, f32 d)
{
	f32 q = (spow<2>(b) - 3.0f * c) / 9.0f;
	f32 r = (2.0f * spow<3>(b) - 9.0f * b * c + 27.0f * d) / 54.0f;
	f32 D = spow<3>(q) - spow<2>(r);
	f32 b_3 = b / 3.0f;

	if (D > 2e-4f)
	{
		f32 theta = AcosRad(r / FSqrt(spow<3>(q)));
		f32 theta_3 = theta / 3.0f;
		f32 r_q2 = FSqrt(q) * -2.0f;

		return r_q2 * CosRad(theta_3) - b_3;
	}
	else if (D < -2e-4f)
	{
		f32 r3_Dr = FCbrt(FSqrt(-D) + FAbs(r));
		f32 xp = r3_Dr + q / r3_Dr;

		return -FCopySign(xp, r) - b_3;
	}
	else
	{
		f32 xp = FSqrt(q);

		return FCopySign(xp, r) - b_3;
	}
}

} // unnamed namespace

int SolveEquation2(f32 *root, f32 a, f32 b, f32 c)
{
	if (b != 0.0f)
	{
		f32 A = b / (a * 2.0f);
		f32 B = c / (a * A * A);
		f32 D = 1.0f - B;

		if (D > 2e-4f)
		{
			f32 C = -1.0f - FSqrt(D);

			root[0] = A * C;
			root[1] = A * B / C;

			return 2;
		}
		else if (D >= -2e-4f)
		{
			root[0] = -A;

			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		f32 c_a = -c / a;

		if (c_a > 2e-4f)
		{
			f32 r_c_a = FSqrt(c_a);

			root[0] = r_c_a;
			root[1] = -r_c_a;

			return 2;
		}
		else if (c_a >= -2e-4f)
		{
			root[0] = 0.0f;

			return 1;
		}
		else
		{
			return 0;
		}
	}
}

int SolveEquation3(f32 *root, f32 a, f32 b, f32 c, f32 d)
{
	b /= a;
	c /= a;
	d /= a;

	f32 q = (spow<2>(b) - 3.0f * c) / 9.0f;
	f32 r = (2.0f * spow<3>(b) - 9.0f * b * c + 27.0f * d) / 54.0f;
	f32 D = spow<3>(q) - spow<2>(r);
	f32 b_3 = b / 3.0f;

	if (D > 2e-4f)
	{
		f32 theta = AcosRad(r / FSqrt(spow<3>(q)));
		f32 theta_3 = theta / 3.0f;
		f32 r_q2 = FSqrt(q) * -2.0f;

		root[0] = r_q2 * CosRad(theta_3) - b_3;
		root[1] = r_q2 * CosRad(theta_3 + Pi / 1.5f) - b_3;
		root[2] = r_q2 * CosRad(theta_3 + Pi / 0.75f) - b_3;

		return 3;
	}
	else if (D < -2e-4f)
	{
		f32 r3_Dr = FCbrt(FSqrt(-D) + FAbs(r));
		f32 xp = r3_Dr + q / r3_Dr;

		root[0] = -FCopySign(xp, r) - b_3;

		return 1;
	}
	else
	{
		f32 xp = FSqrt(q);
		f32 sxp = FCopySign(xp, r);

		root[0] = sxp - b_3;
		root[1] = -2.0f * sxp - b_3;

		return 2;
	}
}

int SolveEquation4(f32 *root, f32 a, f32 b, f32 c, f32 d, f32 e)
{
	b /= a;
	c /= a;
	d /= a;
	e /= a;

	f32 m, n, y;

	{ // 0xc82 wants lexical_block
		f32 p = -3.0f * spow<2>(b) / 8.0f + c;
		f32 q = spow<3>(b) / 8.0f - b * c / 2.0f + d;
		f32 r = -3.0f * spow<4>(b) / 256.0f + c * spow<2>(b) / 16.0f
		      - b * d / 4.0f + e;

		if (q != 0.0f)
		{
			y = SolveEquation3(-p / 2.0f, -r, p * r / 2.0f - spow<2>(q) / 8.0f);
			n = FSqrt(spow<2>(y) - r);
			m = -q / (2.0f * n);
		}
		else
		{
			y = p / 2.0f;
			n = FSqrt(spow<2>(y) - r);
			m = 0.0f;
		}
	}

	int nRoot = 0;
	f32 b4 = b / 4.0f;

	{ // 0xcd2 wants lexical_block
		f32 root01[2];

		int nRoot01 = SolveEquation2(root01, m, y + n);

		root[nRoot + 0] = root01[0] - b4;
		root[nRoot + 1] = root01[1] - b4;

		nRoot += nRoot01;
	}

	{ // 0xd00 wants lexical_block
		f32 root23[2];

		int nRoot23 = SolveEquation2(root23, -m, y - n);

		root[nRoot + 0] = root23[0] - b4;
		root[nRoot + 1] = root23[1] - b4;

		nRoot += nRoot23;
	}

	return nRoot;
}

}} // namespace nw4hbm::math
