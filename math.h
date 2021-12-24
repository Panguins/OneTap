#pragma once
#include <xmmintrin.h>
#include <cmath>

enum
{
	PITCH = 0, // up / down
	YAW, // left / right
	ROLL // fall over
};

namespace math
{
	static constexpr float pi = 3.14159265358979323846f;
	static constexpr float radpi = 57.295779513082f;
	static constexpr float pirad = 0.01745329251f;

	// sorry just in case u want to not be retarded
	static constexpr float deg2rad( const float& x )
	{
		return x * pirad;
	}

	static constexpr float rad2deg( const float& x )
	{
		return x * radpi;
	}

	__forceinline float normalize( float angle )
	{
		auto revolutions = angle / 360.f;
		if ( angle > 180.f || angle < -180.f )
		{
			revolutions = round( abs( revolutions ) );
			if ( angle < 0.f )
				angle = ( angle + 360.f * revolutions );
			else
				angle = ( angle - 360.f * revolutions );
			return angle;
		}
		return angle;
	}

	__forceinline float __cdecl DotProduct_ASM( const float _v1[3], const float _v2[3] )
	{
		float dotret;
		__asm
		{
			mov ecx, _v1
			mov eax, _v2
			; optimized dot product; 15 cycles
			fld dword ptr[eax + 0]; starts & ends on cycle 0
			fmul dword ptr[ecx + 0]; starts on cycle 1
			fld dword ptr[eax + 4]; starts & ends on cycle 2
			fmul dword ptr[ecx + 4]; starts on cycle 3
			fld dword ptr[eax + 8]; starts & ends on cycle 4
			fmul dword ptr[ecx + 8]; starts on cycle 5
			fxch st( 1 ); no cost
			faddp st( 2 ), st( 0 ); starts on cycle 6, stalls for cycles 7 - 8
			faddp st( 1 ), st( 0 ); starts on cycle 9, stalls for cycles 10 - 12
			fstp dword ptr[dotret]; starts on cycle 13, ends on cycle 14
		}
		return dotret;
	}

	// even more fps friendly :D
	__forceinline void VectorTransformASM( const float* in1, const matrix3x4_t& in2, float* out1 )
	{
		out1[0] = DotProduct_ASM( in1, in2[0] ) + in2[0][3];
		out1[1] = DotProduct_ASM( in1, in2[1] ) + in2[1][3];
		out1[2] = DotProduct_ASM( in1, in2[2] ) + in2[2][3];
	}

	__forceinline void vector_transform( vec3& in1, const matrix3x4_t& in2, vec3& out )
	{
		out.x = in1.dot( in2.m_flMatVal[0] ) + in2.m_flMatVal[0][3];
		out.y = in1.dot( in2.m_flMatVal[1] ) + in2.m_flMatVal[1][3];
		out.z = in1.dot( in2.m_flMatVal[2] ) + in2.m_flMatVal[2][3];
	}

	inline vec3 vector_transform( vec3 vec, matrix3x4_t matrix )
	{
		vec3 out;
		out.x = vec.dot( matrix[0] ) + matrix[0][3];
		out.y = vec.dot( matrix[1] ) + matrix[1][3];
		out.z = vec.dot( matrix[2] ) + matrix[2][3];
		return out;
	}

	inline vec2 angles_to_pixels( const vec3& angles )
	{
		static  const  auto m_pitch  = csgo.m_engine_cvars( )->FindVar( xors( "m_pitch" ) );
		static  const  auto m_yaw    = csgo.m_engine_cvars( )->FindVar( xors( "m_yaw" ) );

		float x = angles.x / m_pitch->get_float( );
		float y = angles.y / m_yaw->get_float( );

		return vec2( -y, x );
	}

	// league aimer
	inline vec3 pixels_to_angles( const vec2& pixels )
	{
		static const auto m_pitch  = csgo.m_engine_cvars( )->FindVar( xors( "m_pitch" ) );
		static const auto m_yaw    = csgo.m_engine_cvars( )->FindVar( xors( "m_yaw" ) );

		float x = pixels.y * m_pitch->get_float( );
		float y = -pixels.x * m_yaw->get_float( );

		return vec3( x, y, 0 );
	}

	// rotate 2d by distance
	inline void find_position_rotation( float& x, float& y, const float& screen_width, const float& screen_height )
	{
		// ty mrs diana euler
		// my algebra teacher
		// https://stackoverflow.com/questions/8489792/is-it-legal-to-take-acos-of-1-0f-or-1-0f/8490249?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa

		const  vec2   delta     = vec2( screen_width / 2 - x, screen_height / 2 - y );
		const  float  hypot     = std::hypot( delta.x, delta.y );		
		const  float  rotation  = rad2deg( std::acos( delta.x / hypot ) );
		
		x += std::sin( rotation );
		y += std::cos( rotation );
	}

	void ConcatTransforms( const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out );

	inline vec3 MatrixGetPosition( const matrix3x4_t& src )
	{
		return vec3( src[0][3], src[1][3], src[2][3] );
	}

	inline void MatrixSetPosition( matrix3x4_t& src, vec3 position )
	{
		src[0][3] = position.x;
		src[1][3] = position.y;
		src[2][3] = position.z;
	}

	inline void AngleMatrix( vec3 angles, matrix3x4_t& matrix )
	{
		float sr, sp, sy, cr, cp, cy;

		sp = sinf( angles.x * pirad );
		cp = cosf( angles.x * pirad );
		sy = sinf( angles.y * pirad );
		cy = cosf( angles.y * pirad );
		sr = sinf( angles.z * pirad );
		cr = cosf( angles.z * pirad );

		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;
		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = ( sp*crcy + srsy );
		matrix[1][2] = ( sp*crsy - srcy );
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.f;
		matrix[1][3] = 0.f;
		matrix[2][3] = 0.f;
	}

	inline void AngleIMatrix( vec3 angles, matrix3x4_t& matrix )
	{
		float sr, sp, sy, cr, cp, cy;

		sp = sinf( angles.x * pirad );
		cp = cosf( angles.x * pirad );
		sy = sinf( angles.y * pirad );
		cy = cosf( angles.y * pirad );
		sr = sinf( angles.z * pirad );
		cr = cosf( angles.z * pirad );

		matrix[0][0] = cp * cy;
		matrix[0][1] = cp * sy;
		matrix[0][2] = -sp;
		matrix[1][0] = sr * sp * cy + cr * -sy;
		matrix[1][1] = sr * sp * sy + cr * cy;
		matrix[1][2] = sr * cp;
		matrix[2][0] = ( cr * sp * cy + -sr * -sy );
		matrix[2][1] = ( cr * sp * sy + -sr * cy );
		matrix[2][2] = cr * cp;
		matrix[0][3] = 0.f;
		matrix[1][3] = 0.f;
		matrix[2][3] = 0.f;
	}

	inline void AngleMatrix( vec3 angles, matrix3x4_t& matrix, vec3 origin )
	{
		AngleMatrix( angles, matrix );
		MatrixSetPosition( matrix, origin );
	}

	inline vec3 VectorRotate( vec3& vec, float rot )
	{
		rot = -rot + 90.f;

		float radius = sqrtf(vec.x * vec.x + vec.y * vec.y);
		float radian = rot * pirad;

		vec.x = sin(radian) * radius;
		vec.y = cos(radian) * radius;
		return vec;
	}

	inline vec3 VectorRotate( const vec3& vec, const vec3& in2 )
	{
		vec3 out;

		matrix3x4_t rotate;
		AngleMatrix( in2, rotate );
		out.x = DotProduct_ASM( reinterpret_cast< const float* >( &vec ), rotate[0] );
		out.y = DotProduct_ASM( reinterpret_cast< const float* >( &vec ), rotate[1] );
		out.z = DotProduct_ASM( reinterpret_cast< const float* >( &vec ), rotate[2] );

		return out;
	}


	inline void MatrixCopy( const matrix3x4_t& src, matrix3x4_t& dst )
	{
		dst[0][0] = src[0][0];
		dst[1][0] = src[1][0];
		dst[2][0] = src[2][0];

		dst[0][1] = src[0][1];
		dst[1][1] = src[1][1];
		dst[2][1] = src[2][1];

		dst[0][2] = src[0][2];
		dst[1][2] = src[1][2];
		dst[2][2] = src[2][2];

		dst[0][3] = src[0][3];
		dst[1][3] = src[1][3];
		dst[2][3] = src[2][3];
	}

	inline void MatrixAngles( const matrix3x4_t& matrix, vec3& angles )
	{
		float forward[3];
		float left[3];
		float up[3];

		// Extract the basis vectors from the matrix. Since we only need the Z
		// component of the up vector, we don't get X and Y.
		forward[0] = matrix[0][0];
		forward[1] = matrix[1][0];
		forward[2] = matrix[2][0];
		left[0] = matrix[0][1];
		left[1] = matrix[1][1];
		left[2] = matrix[2][1];
		up[2] = matrix[2][2];

		float xyDist = sqrtf( forward[0] * forward[0] + forward[1] * forward[1] );

		// enough here to get angles?
		if ( xyDist > 0.001f )
		{
			// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
			angles.y = rad2deg( atan2f( forward[1], forward[0] ) );

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles.x = rad2deg( atan2f( -forward[2], xyDist ) );

			// (roll)	z = ATAN( left.z, up.z );
			angles.z = rad2deg( atan2f( left[2], up[2] ) );
		}
		else	// forward is mostly Z, gimbal lock-
		{
			// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
			angles.y = rad2deg( atan2f( -left[0], left[1] ) );

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles.x = rad2deg( atan2f( -forward[2], xyDist ) );

			// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
			angles.z = 0;
		}
	}

	inline void RotateMatrix( vec3 qAngles, vec3 vecOrigin, float flDegrees, matrix3x4_t& pMatrix )
	{
		qAngles.y += flDegrees;
		qAngles.normalize( );

		vec3 qRotated( 0, flDegrees, 0 );
		matrix3x4_t pRotatedMatrix;
		AngleMatrix( qRotated, pRotatedMatrix );

		vec3 vecDelta = MatrixGetPosition( pMatrix ) - vecOrigin;
		vec3 vecOut;
		VectorTransformASM( &vecDelta[0], pRotatedMatrix, &vecOut[0] );
		vecOut += vecOrigin;

		matrix3x4_t pBoneRotation, pOut;
		MatrixCopy( pMatrix, pBoneRotation );

		MatrixSetPosition( pBoneRotation, vec3( 0.f, 0.f, 0.f ) );
		ConcatTransforms( pRotatedMatrix, pBoneRotation, pOut );
		MatrixAngles( pOut, qAngles );
		AngleMatrix( qAngles, pMatrix, vecOut );
	}

	// mfn epic compiler
	inline vec3 _vector_angles( const vec3& start, const vec3& end )
	{
		const vec3 delta = end - start;

		return vec3( rad2deg( std::atan2( -delta.z, std::sqrt( delta.x * delta.x + delta.y * delta.y ) ) ),
		             rad2deg( std::atan2( delta.y, delta.x ) ),
		             0.0f );
	}

	inline void vector_angles( const vec3& forward, vec3& angles )
	{
		angles = vec3( rad2deg( std::atan2( -forward.z, std::sqrt( forward.x * forward.x + forward.y * forward.y ) ) ),
		               rad2deg( std::atan2( forward.y, forward.x ) ),
		               0.0f );
	}

	inline void __vector_angles( const vec3& forward, vec3& angles )
	{
		angles.x += rad2deg( std::atan( std::sqrt( forward.x * forward.x + forward.y * forward.y ) ) );
		angles.z = -rad2deg( std::atan2( forward.x, forward.y ) );
	}

	inline vec3 vector_ma( const vec3& start, float scale, const vec3& dir )
	{
		return start + dir * scale;
	}

	inline void sin_cos( float radians, float* sine, float* cosine )
	{
		*sine = sin( radians );
		*cosine = cos( radians );
	}

	inline void angle_vectors( const vec3& angles, vec3* forward = nullptr, vec3* right = nullptr, vec3* up = nullptr )
	{
		float sr, sp, sy, cr, cp, cy;

		sin_cos( deg2rad( angles[YAW] ), &sy, &cy );
		sin_cos( deg2rad( angles[PITCH] ), &sp, &cp );
		sin_cos( deg2rad( angles[ROLL] ), &sr, &cr );

		if ( forward )
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if ( right )
		{
			right->x = ( -1 * sr * sp * cy + -1 * cr * -sy );
			right->y = ( -1 * sr * sp * sy + -1 * cr * cy );
			right->z = -1 * sr * cp;
		}

		if ( up )
		{
			up->x = ( cr * sp * cy + -sr * -sy );
			up->y = ( cr * sp * sy + -sr * cy );
			up->z = cr * cp;
		}
	}

	inline float float_normalize( float val, float min, float max)
	{
		float step = max - min;

		while ( val > max )
			val -= step;

		while ( val < min )
			val += step;

		return val;
	}

	inline float yaw_diff( float start, float end )
	{
		start = float_normalize( start, -180.f, 180.f );
		end = float_normalize( end, -180.f, 180.f );

		return float_normalize( end - start, -180.f, 180.f );
	}

	namespace imports
	{
		__forceinline uint32_t md5_pseudo_random( uint32_t seed )
		{
			static auto pseudo_random = memory::pattern::first_code_match< uint32_t(__cdecl*)( uint32_t ) >( csgo.m_client.module( ), xors( "55 8B EC 83 E4 F8 83 EC 70 6A 58" ) );

			if ( pseudo_random )
				return pseudo_random( seed );

			return 0;
		}

		__forceinline float random( float min, float max )
		{
			static auto random_float = reinterpret_cast< float(__cdecl*)( float, float ) >( GetProcAddress( GetModuleHandle( xors( "vstdlib.dll" ) ), xors( "RandomFloat" ) ) );

			if ( random_float )
				return random_float( min, max );

			return min;
		}

		__forceinline int random( int min, int max )
		{
			static auto random_int = reinterpret_cast< int(__cdecl*)( int, int ) >( GetProcAddress( GetModuleHandle( xors( "vstdlib.dll" ) ), xors( "RandomInt" ) ) );

			if ( random_int )
				return random_int( min, max );

			return min;
		}

		__forceinline void random_seed( uint32_t seed )
		{
			static auto random_seed = reinterpret_cast< void(__cdecl*)( uint32_t ) >( GetProcAddress( GetModuleHandle( xors( "vstdlib.dll" ) ), xors( "RandomSeed" ) ) );

			if ( random_seed )
				return random_seed( seed );
		}
	}

	namespace time
	{
		static inline float tick_interval( )
		{
			return csgo.m_globals( )->interval_per_tick;
		}

		static inline int to_ticks( const float& time )
		{
			return static_cast< int >( 0.5f + time / tick_interval( ) );
		}

		static inline float to_time( const int& ticks )
		{
			return tick_interval( ) * ticks;
		}

		static constexpr int tick_never_think = -1;
	}
}
