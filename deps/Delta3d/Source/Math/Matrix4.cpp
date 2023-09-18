#include "PrecompiledHeader.h"
#include "Matrix4.h"
#include "Quaternion.h"

namespace Delta3D::Math
{

const Matrix4 Matrix4::Null( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
const Matrix4 Matrix4::Identity( 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f );

Matrix4 Matrix4::operator*( const Matrix4& other ) const
{
	Matrix4 result;

	__m128 r0 = _mm_loadu_ps( &other._11 );
	__m128 r1 = _mm_loadu_ps( &other._21 );
	__m128 r2 = _mm_loadu_ps( &other._31 );
	__m128 r3 = _mm_loadu_ps( &other._41 );

	__m128 l = _mm_loadu_ps( &_11 );
	__m128 t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	__m128 t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	__m128 t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	__m128 t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._11, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_21 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._21, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_31 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._31, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_41 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._41, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	return result;
}

Matrix4 Matrix4::operator*( float fv ) const
{
	Matrix4 result;

	for ( int i = 0; i < 16; i++ )
		result.f[i] = f[i] * fv;

	return result;
}

Matrix4 Matrix4::operator+( const Matrix4 & other ) const
{
	Matrix4 result;

	for ( int i = 0; i < 16; i++ )
		result.f[i] = f[i] + other.f[i];

	return result;
}

Matrix4 Matrix4::operator*( const Matrix4Int& other ) const
{
	Matrix4 result;

	__m128 r0 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*) & other._11 ) ), _mm_set1_ps( 256.f ) );
	__m128 r1 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*) & other._21 ) ), _mm_set1_ps( 256.f ) );
	__m128 r2 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*) & other._31 ) ), _mm_set1_ps( 256.f ) );
	__m128 r3 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*) & other._41 ) ), _mm_set1_ps( 256.f ) );

	__m128 l = _mm_loadu_ps( &_11 );
	__m128 t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	__m128 t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	__m128 t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	__m128 t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._11, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_21 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._21, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_31 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._31, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_loadu_ps( &_41 );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._41, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	return result;
}

Matrix4& Matrix4::operator=( const Matrix4Int& other )
{
	_mm_storeu_ps( &_11, _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._11 ) ), _mm_set1_ps( 256.f ) ) );
	_mm_storeu_ps( &_21, _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._21 ) ), _mm_set1_ps( 256.f ) ) );
	_mm_storeu_ps( &_31, _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._31 ) ), _mm_set1_ps( 256.f ) ) );
	_mm_storeu_ps( &_41, _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._41 ) ), _mm_set1_ps( 256.f ) ) );

	return *this;
}

Matrix4 & Matrix4::operator+=( const Matrix4 & other )
{
	for ( int i = 0; i < 16; i++ )
		f[i] += other.f[i];

	return *this;
}

Vector3 Matrix4::operator*( const Vector3& other ) const
{
	__m128 m0 = _mm_mul_ps(_mm_loadu_ps( &_11 ), _mm_set1_ps(other.x));
	__m128 m1 = _mm_mul_ps(_mm_loadu_ps( &_21 ), _mm_set1_ps(other.y));
	__m128 m2 = _mm_mul_ps(_mm_loadu_ps( &_31 ), _mm_set1_ps(other.z));

	float ret[4];
	_mm_storeu_ps( &ret[0], _mm_add_ps( _mm_add_ps( _mm_add_ps( m0, m1 ), m2 ), _mm_loadu_ps( &_41 ) ) );

	return Vector3( ret[0], ret[1], ret[2] );
}


void Matrix4::RotateAxis( const Vector3& axis, const float angle )
{
	Quaternion q( axis, angle );

	*this = q.ToMatrix();
}

void Matrix4::TranslateAxis( const Vector3& origin, const Vector3& axisX, const Vector3& axisY, const Vector3& axisZ )
{
	_11 = axisX.x;  _12 = axisX.y;  _13 = axisX.z;  _14 = 0.0f;
	_21 = axisY.x;  _22 = axisY.y;  _23 = axisY.z;  _24 = 0.0f;
	_31 = axisZ.x;  _32 = axisZ.y;  _33 = axisZ.z;  _34 = 0.0f;
	_41 = origin.x; _42 = origin.y; _43 = origin.z; _44 = 1.0f;
}

void Matrix4::RotateX( const float rad )
{
	m[1][1] = cosf( rad );
	m[2][2] = m[1][1];
	m[1][2] = sinf( rad );
	m[2][1] = -m[1][2];
}

void Matrix4::RotateY( const float rad )
{
	m[0][0] = cosf( rad );
	m[2][2] = m[0][0];
	m[2][0] = sinf( rad );
	m[0][2] = -m[2][0];
}

void Matrix4::RotateZ( const float rad )
{
	m[0][0] = cosf( rad );
	m[1][1] = m[0][0];
	m[0][1] = sinf( rad );
	m[1][0] = -m[0][1];
}

Matrix4 Matrix4::FlippedYZ()
{
	Math::Matrix4 result = *this;

	for( int i = 0; i < 4; i++ )
		std::swap( result.m[i][1], result.m[i][2] );

	return result;
}

Matrix4 Matrix4::Inverse() const
{
	/*
	float v0 = m[2][0]* m[3][1] - m[2][1]* m[3][0];
    float v1 = m[2][0]* m[3][2] - m[2][2]* m[3][0];
    float v2 = m[2][0]* m[3][3] - m[2][3]* m[3][0];
    float v3 = m[2][1]* m[3][2] - m[2][2]* m[3][1];
    float v4 = m[2][1]* m[3][3] - m[2][3]* m[3][1];
    float v5 = m[2][2]* m[3][3] - m[2][3]* m[3][2];

    float i00 = (v5* m[1][1] - v4* m[1][2] + v3* m[1][3]);
    float i10 = -(v5* m[1][0] - v2* m[1][2] + v1* m[1][3]);
    float i20 = (v4* m[1][0] - v2* m[1][1] + v0* m[1][3]);
    float i30 = -(v3* m[1][0] - v1* m[1][1] + v0* m[1][2]);

    float invDet = 1.0f / (i00* m[0][0] + i10* m[0][1] + i20* m[0][2] + i30* m[0][3]);

    i00 *= invDet;
    i10 *= invDet;
    i20 *= invDet;
    i30 *= invDet;

    float i01 = -(v5* m[0][1] - v4* m[0][2] + v3* m[0][3])* invDet;
    float i11 = (v5* m[0][0] - v2* m[0][2] + v1* m[0][3])* invDet;
    float i21 = -(v4* m[0][0] - v2* m[0][1] + v0* m[0][3])* invDet;
    float i31 = (v3* m[0][0] - v1* m[0][1] + v0* m[0][2])* invDet;

    v0 = m[1][0]* m[3][1] - m[1][1]* m[3][0];
    v1 = m[1][0]* m[3][2] - m[1][1]* m[3][0];
    v2 = m[1][0]* m[3][3] - m[1][3]* m[3][0];
    v3 = m[1][1]* m[3][2] - m[1][2]* m[3][1];
    v4 = m[1][1]* m[3][3] - m[1][3]* m[3][1];
    v5 = m[1][2]* m[3][3] - m[1][3]* m[3][2];

    float i02 = (v5* m[0][1] - v4* m[0][2] + v3* m[0][3])* invDet;
    float i12 = -(v5* m[0][0] - v2* m[0][2] + v1* m[0][3])* invDet;
    float i22 = (v4* m[0][0] - v2* m[0][1] + v0* m[0][3])* invDet;
    float i32 = -(v3* m[0][0] - v1* m[0][1] + v0* m[0][2])* invDet;

    v0 = m[2][1]* m[1][0] - m[2][0]* m[1][1];
    v1 = m[2][2]* m[1][0] - m[2][0]* m[1][2];
    v2 = m[2][3]* m[1][0] - m[2][0]* m[1][3];
    v3 = m[2][2]* m[1][1] - m[2][1]* m[1][2];
    v4 = m[2][3]* m[1][1] - m[2][1]* m[1][3];
    v5 = m[2][3]* m[1][2] - m[2][2]* m[1][3];

    float i03 = -(v5* m[0][1] - v4* m[0][2] + v3* m[0][3])* invDet;
    float i13 = (v5* m[0][0] - v2* m[0][2] + v1* m[0][3])* invDet;
    float i23 = -(v4* m[0][0] - v2* m[0][2] + v0* m[0][3])* invDet;
    float i33 = (v3* m[0][0] - v1* m[0][1] + v0* m[0][2])* invDet;

    return Matrix4( i00, i01, i02, i03, i10, i11, i12, i13, i20, i21, i22, i23, i30, i31, i32, i33);
	*/

    float inv[16], det;
    int i;

	Matrix4 sResult;

    inv[0] = f[5] * f[10] * f[15] - f[5] * f[11] * f[14] - f[9] * f[6] * f[15] + f[9] * f[7] * f[14] + f[13] * f[6] * f[11] - f[13] * f[7] * f[10];
    inv[4] = -f[4] * f[10] * f[15] + f[4] * f[11] * f[14] + f[8] * f[6] * f[15] - f[8] * f[7] * f[14] - f[12] * f[6] * f[11] + f[12] * f[7] * f[10];
    inv[8] = f[4] * f[9] * f[15] - f[4] * f[11] * f[13] - f[8] * f[5] * f[15] + f[8] * f[7] * f[13] + f[12] * f[5] * f[11] - f[12] * f[7] * f[9];
    inv[12] = -f[4] * f[9] * f[14] + f[4] * f[10] * f[13] + f[8] * f[5] * f[14] - f[8] * f[6] * f[13] - f[12] * f[5] * f[10] + f[12] * f[6] * f[9];
    inv[1] = -f[1] * f[10] * f[15] + f[1] * f[11] * f[14] + f[9] * f[2] * f[15] - f[9] * f[3] * f[14] - f[13] * f[2] * f[11] + f[13] * f[3] * f[10];
    inv[5] = f[0] * f[10] * f[15] - f[0] * f[11] * f[14] - f[8] * f[2] * f[15] + f[8] * f[3] * f[14] + f[12] * f[2] * f[11] - f[12] * f[3] * f[10];
    inv[9] = -f[0] * f[9] * f[15] + f[0] * f[11] * f[13] + f[8] * f[1] * f[15] - f[8] * f[3] * f[13] - f[12] * f[1] * f[11] + f[12] * f[3] * f[9];
    inv[13] = f[0] * f[9] * f[14] - f[0] * f[10] * f[13] - f[8] * f[1] * f[14] + f[8] * f[2] * f[13] + f[12] * f[1] * f[10] - f[12] * f[2] * f[9];
    inv[2] = f[1] * f[6] * f[15] - f[1] * f[7] * f[14] - f[5] * f[2] * f[15] + f[5] * f[3] * f[14] + f[13] * f[2] * f[7] - f[13] * f[3] * f[6];
    inv[6] = -f[0] * f[6] * f[15] + f[0] * f[7] * f[14] + f[4] * f[2] * f[15] - f[4] * f[3] * f[14] - f[12] * f[2] * f[7] + f[12] * f[3] * f[6];
    inv[10] = f[0] * f[5] * f[15] - f[0] * f[7] * f[13] - f[4] * f[1] * f[15] + f[4] * f[3] * f[13] + f[12] * f[1] * f[7] - f[12] * f[3] * f[5];
    inv[14] = -f[0] * f[5] * f[14] + f[0] * f[6] * f[13] + f[4] * f[1] * f[14] - f[4] * f[2] * f[13] - f[12] * f[1] * f[6] + f[12] * f[2] * f[5];
    inv[3] = -f[1] * f[6] * f[11] + f[1] * f[7] * f[10] + f[5] * f[2] * f[11] - f[5] * f[3] * f[10] - f[9] * f[2] * f[7] + f[9] * f[3] * f[6];
    inv[7] = f[0] * f[6] * f[11] - f[0] * f[7] * f[10] - f[4] * f[2] * f[11] + f[4] * f[3] * f[10] + f[8] * f[2] * f[7] - f[8] * f[3] * f[6];
    inv[11] = -f[0] * f[5] * f[11] + f[0] * f[7] * f[9] + f[4] * f[1] * f[11] - f[4] * f[3] * f[9] - f[8] * f[1] * f[7] + f[8] * f[3] * f[5];
    inv[15] = f[0] * f[5] * f[10] - f[0] * f[6] * f[9] - f[4] * f[1] * f[10] + f[4] * f[2] * f[9] + f[8] * f[1] * f[6] - f[8] * f[2] * f[5];

    det = f[0] * inv[0] + f[1] * inv[4] + f[2] * inv[8] + f[3] * inv[12];

    if ( det == 0 )
        return *this;

    det = 1.f / det;

    for ( i = 0; i < 16; i++ )
		sResult.f[i] = inv[i] * det;

	return sResult;
}

Vector3 Matrix4::ToEulerAngles()
{
	Vector3 vOut;

	vOut.x = atan2f( f[9], f[10] );
	vOut.y = atan2f( -f[8], sqrtf( f[9] * f[9] + f[10] * f[10] ) );
	vOut.z = atan2f( f[4], f[0] );

	return vOut;
}

Quaternion Matrix4::ToQuaternion() const
{
	Quaternion qOut;

	float fWSquaredMinus1 = f[0] + f[5] + f[10];
	float fXSquaredMinus1 = f[0] - f[5] - f[10];
	float fYSquaredMinus1 = f[5] - f[0] - f[10];
	float fZSquaredMinus1 = f[10] - f[0] - f[5];

	int nBiggestIndex = 0;
	float fBiggestSquaredMinus1 = fWSquaredMinus1;

	if ( fXSquaredMinus1 > fBiggestSquaredMinus1 )
	{
		fBiggestSquaredMinus1 = fXSquaredMinus1;
		nBiggestIndex = 1;
	}

	if ( fYSquaredMinus1 > fBiggestSquaredMinus1 )
	{
		fBiggestSquaredMinus1 = fYSquaredMinus1;
		nBiggestIndex = 2;
	}

	if ( fZSquaredMinus1 > fBiggestSquaredMinus1 )
	{
		fBiggestSquaredMinus1 = fZSquaredMinus1;
		nBiggestIndex = 3;
	}

	// 
	float fBiggestValue = sqrt( fBiggestSquaredMinus1 + 1.0f ) * 0.5f;
	float fMult = 0.25f / fBiggestValue;

	// 
	switch ( nBiggestIndex )
	{
		case 0:
			qOut.w = fBiggestValue;
			qOut.x = (f[6] - f[9]) * fMult;
			qOut.y = (f[8] - f[2]) * fMult;
			qOut.z = (f[1] - f[4]) * fMult;
			break;
		case 1:
			qOut.x = fBiggestValue;
			qOut.w = (f[6] - f[9]) * fMult;
			qOut.y = (f[1] + f[4]) * fMult;
			qOut.z = (f[8] + f[2]) * fMult;
			break;
		case 2:
			qOut.y = fBiggestValue;
			qOut.w = (f[8] - f[2]) * fMult;
			qOut.x = (f[1] + f[4]) * fMult;
			qOut.z = (f[6] + f[9]) * fMult;
			break;
		case 3:
			qOut.z = fBiggestValue;
			qOut.w = (f[1] - f[4]) * fMult;
			qOut.x = (f[8] + f[2]) * fMult;
			qOut.y = (f[6] + f[9]) * fMult;
			break;
	}

	return qOut.Flip();
}

void Matrix4::BulkTransposeTo3x4( float* dest, const float* src, unsigned int count )
{
	for( unsigned int i = 0; i < count; ++i )
	{
		__m128 m0 = _mm_loadu_ps( src );
		__m128 m1 = _mm_loadu_ps( src + 4 );
		__m128 m2 = _mm_loadu_ps( src + 8 );
		__m128 m3 = _mm_loadu_ps( src + 12 );
		_MM_TRANSPOSE4_PS( m0, m1, m2, m3 );
		_mm_storeu_ps( dest, m0 );
		_mm_storeu_ps( dest + 4, m2 );
		_mm_storeu_ps( dest + 8, m1 );

		dest += 12;
		src += 16;
	}
}

bool Matrix4::Decompose( Vector3 & vScale, Quaternion & vRotation, Vector3 & vTranslation )
{
	auto x = Vector3( _11, _21, _31 );
	auto y = Vector3( _12, _22, _32 );
	auto z = Vector3( _13, _23, _33 );

	vScale.x = x.Length();
	vScale.y = y.Length();
	vScale.z = z.Length();

	vTranslation.x = _41;
	vTranslation.y = _42;
	vTranslation.z = _43;

	auto vMatrix = *this;

	vMatrix._11 /= vScale.x;
	vMatrix._21 /= vScale.x;
	vMatrix._31 /= vScale.x;

	vMatrix._12 /= vScale.y;
	vMatrix._22 /= vScale.y;
	vMatrix._32 /= vScale.y;

	vMatrix._13 /= vScale.z;
	vMatrix._23 /= vScale.z;
	vMatrix._33 /= vScale.z;

	vRotation = vMatrix.ToQuaternion();

	return true;
}


Matrix4 Matrix4Int::operator*( const Matrix4Int& other ) const
{
	Matrix4 result;

	__m128 r0 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._11 ) ), _mm_set1_ps( 256.f ) );
	__m128 r1 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._21 ) ), _mm_set1_ps( 256.f ) );
	__m128 r2 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._31 ) ), _mm_set1_ps( 256.f ) );
	__m128 r3 = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&other._41 ) ), _mm_set1_ps( 256.f ) );

	__m128 l = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&_11 ) ), _mm_set1_ps( 256.f ) );
	__m128 t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	__m128 t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	__m128 t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	__m128 t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._11, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&_21 ) ), _mm_set1_ps( 256.f ) );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._21, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&_31 ) ), _mm_set1_ps( 256.f ) );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._31, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	l = _mm_div_ps( _mm_cvtepi32_ps( _mm_loadu_si128( (__m128i*)&_41 ) ), _mm_set1_ps( 256.f ) );
	t0 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 0, 0, 0, 0 ) ), r0 );
	t1 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 1, 1, 1, 1 ) ), r1 );
	t2 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 2, 2, 2, 2 ) ), r2 );
	t3 = _mm_mul_ps( _mm_shuffle_ps( l, l, _MM_SHUFFLE( 3, 3, 3, 3 ) ), r3 );
	_mm_storeu_ps( &result._41, _mm_add_ps( _mm_add_ps( t0, t1 ), _mm_add_ps( t2, t3 ) ) );

	return result;
}

}