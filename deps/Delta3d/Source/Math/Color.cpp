#include "PrecompiledHeader.h"
#include "Color.h"

namespace Delta3D::Math
{

const Color Color::White( 1.0f, 1.0f, 1.0f, 1.0f );

bool Color::Blend( Color & r, const Color v1, const Color v2 )
{
	r = Color( (v1.r + v2.r) * 0.5f, (v1.g + v2.g) * 0.5f, (v1.b + v2.b) * 0.5f, (v1.a + v2.a) * 0.5f );

	return true;
}

const unsigned int Color::ToUInt() const
{
	unsigned int r_ = std::clamp( (int)(r* 255.0f), 0, 255 );
	unsigned int g_ = std::clamp( (int)(g* 255.0f), 0, 255 );
	unsigned int b_ = std::clamp( (int)(b* 255.0f), 0, 255 );
	unsigned int a_ = std::clamp( (int)(a* 255.0f), 0, 255 );
	return (a_ << 24u) | (r_ << 16u) | (g_ << 8u) | b_;
}

void Color::FromUInt( unsigned int color )
{
	a = ( (color >> 24u)& 0xffu ) / 255.0f;
	r = ( (color >> 16u)& 0xffu ) / 255.0f;
	g = ( (color >> 8u )& 0xffu ) / 255.0f;
	b = ( (color >> 0u )& 0xffu ) / 255.0f;
}

std::string Color::ToString() const
{
	return "R: " + std::to_string( r ) + " G: " + std::to_string( g ) + " B: " + std::to_string( b ) + " A: " + std::to_string( a );
}

}
