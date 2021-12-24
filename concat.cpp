#include "context.h"
#include "math.h"
#include <emmintrin.h>

typedef __m128 fltx4;
typedef __m128 i32x4;
typedef __m128 u32x4;

#define DECL_ALIGN(x) __declspec(align(x))
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN16_POST
#define MM_SHUFFLE_REV(a, b, c, d) _MM_SHUFFLE(d, c, b, a)

const unsigned int ALIGN16 g_SIMD_ComponentMask[4][4] ALIGN16_POST =
{
	{ 0xFFFFFFFF, 0, 0, 0 },{ 0, 0xFFFFFFFF, 0, 0 },{ 0, 0, 0xFFFFFFFF, 0 },{ 0, 0, 0, 0xFFFFFFFF }
};

__forceinline fltx4 LoadUnalignedSIMD( const void *pSIMD )
{
	return _mm_loadu_ps( reinterpret_cast<const float *>( pSIMD ) );
}

__forceinline fltx4 SplatXSIMD( fltx4 const & a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 0, 0, 0, 0 ) );
}

__forceinline fltx4 SplatYSIMD( fltx4 const &a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 1, 1, 1, 1 ) );
}

__forceinline fltx4 SplatZSIMD( fltx4 const &a )
{
	return _mm_shuffle_ps( a, a, MM_SHUFFLE_REV( 2, 2, 2, 2 ) );
}

__forceinline fltx4 MulSIMD( const fltx4 & a, const fltx4 & b )				// a*b
{
	return _mm_mul_ps( a, b );
};

__forceinline fltx4 AddSIMD( const fltx4 & a, const fltx4 & b )				// a+b
{
	return _mm_add_ps( a, b );
};

__forceinline fltx4 AndSIMD( const fltx4 & a, const fltx4 & b )				// a & b
{
	return _mm_and_ps( a, b );
}

__forceinline void StoreUnalignedSIMD( float *pSIMD, const fltx4 & a )
{
	*( reinterpret_cast< fltx4 *> ( pSIMD ) ) = a;
}

void math::ConcatTransforms( const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out )
{
	fltx4 lastMask = *( fltx4 * ) ( &g_SIMD_ComponentMask[3] );
	fltx4 rowA0 = LoadUnalignedSIMD( in1[0] );
	fltx4 rowA1 = LoadUnalignedSIMD( in1[1] );
	fltx4 rowA2 = LoadUnalignedSIMD( in1[2] );

	fltx4 rowB0 = LoadUnalignedSIMD( in2[0] );
	fltx4 rowB1 = LoadUnalignedSIMD( in2[1] );
	fltx4 rowB2 = LoadUnalignedSIMD( in2[2] );

	// now we have the rows of m0 and the columns of m1
	// first output row
	fltx4 A0 = SplatXSIMD( rowA0 );
	fltx4 A1 = SplatYSIMD( rowA0 );
	fltx4 A2 = SplatZSIMD( rowA0 );
	fltx4 mul00 = MulSIMD( A0, rowB0 );
	fltx4 mul01 = MulSIMD( A1, rowB1 );
	fltx4 mul02 = MulSIMD( A2, rowB2 );
	fltx4 out0 = AddSIMD( mul00, AddSIMD( mul01, mul02 ) );

	// second output row
	A0 = SplatXSIMD( rowA1 );
	A1 = SplatYSIMD( rowA1 );
	A2 = SplatZSIMD( rowA1 );
	fltx4 mul10 = MulSIMD( A0, rowB0 );
	fltx4 mul11 = MulSIMD( A1, rowB1 );
	fltx4 mul12 = MulSIMD( A2, rowB2 );
	fltx4 out1 = AddSIMD( mul10, AddSIMD( mul11, mul12 ) );

	// third output row
	A0 = SplatXSIMD( rowA2 );
	A1 = SplatYSIMD( rowA2 );
	A2 = SplatZSIMD( rowA2 );
	fltx4 mul20 = MulSIMD( A0, rowB0 );
	fltx4 mul21 = MulSIMD( A1, rowB1 );
	fltx4 mul22 = MulSIMD( A2, rowB2 );
	fltx4 out2 = AddSIMD( mul20, AddSIMD( mul21, mul22 ) );

	// add in translation vector
	A0 = AndSIMD( rowA0, lastMask );
	A1 = AndSIMD( rowA1, lastMask );
	A2 = AndSIMD( rowA2, lastMask );
	out0 = AddSIMD( out0, A0 );
	out1 = AddSIMD( out1, A1 );
	out2 = AddSIMD( out2, A2 );

	// write to output
	StoreUnalignedSIMD( out[0], out0 );
	StoreUnalignedSIMD( out[1], out1 );
	StoreUnalignedSIMD( out[2], out2 );
}