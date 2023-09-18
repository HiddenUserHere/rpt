#include "PrecompiledHeader.h"
#include "Vector3.h"

#include "Matrix4.h"

namespace Delta3D::Math
{

const Vector3Int Vector3Int::Null( 0, 0, 0 );
const Vector3 Vector3::Null( 0.0f, 0.0f, 0.0f );

std::string Vector3Int::ToString() const
{
	return "X: " + std::to_string( x ) + " Y: " + std::to_string( y ) + " Z: " + std::to_string( z );
}

Vector3 Vector3::Slerp( Vector3 other, float percent ) const
{
    Vector3 v = *this;

    //Dot product - the cosine of the angle between 2 vectors.
    float dot = v.DotProduct( other );

    dot = std::clamp( dot, -1.0f, 1.0f );

    float theta = acosf( dot ) * percent;
    Vector3 RelativeVec = other - v * dot;
    RelativeVec.Normalize();

    return ((v * cosf( theta )) + (RelativeVec * sinf( theta )));
}

void Vector3::Transform( const Vector3& v, const Matrix4& m )
{
	x = m.m[0][0]* v.x + m.m[1][0]* v.y + m.m[2][0]* v.z;
	y = m.m[0][1]* v.x + m.m[1][1]* v.y + m.m[2][1]* v.z;
	z = m.m[0][2]* v.x + m.m[1][2]* v.y + m.m[2][2]* v.z;
}

std::string Vector3::ToString() const
{
	return "X: " + std::to_string( x ) + " Y: " + std::to_string( y ) + " Z: " + std::to_string( z );
}

Matrix4 Vector3::ToMatrixScale() const
{
	Matrix4 r = Matrix4::Identity;
	r.Scale( *this );
	return r;
}

Matrix4 Vector3::ToMatrixTranslation() const
{
	Matrix4 r = Matrix4::Identity;
	r.Translate( *this );
	return r;
}

}