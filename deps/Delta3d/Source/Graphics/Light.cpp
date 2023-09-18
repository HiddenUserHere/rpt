#include "PrecompiledHeader.h"
#include "Light.h"

namespace Delta3D::Graphics
{
void Light::GenerateViewMatrix()
{
	D3DXVECTOR3 up;

	//Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Create the view matrix from the three vectors.
	D3DXMATRIX m;
	D3DXMatrixLookAtLH( &m, &position.Get(), &lookAt.Get(), &up );

	memcpy_s( &viewMatrix, sizeof( Math::Matrix4 ), &m, sizeof( D3DXMATRIX ) );
}

void Light::GenerateOrthoMatrix( float width, float depthPlane, float nearPlane )
{
	//Create the orthographic matrix for the light.
	D3DXMATRIX m;
	D3DXMatrixOrthoLH( &m, width, width, nearPlane, depthPlane );

	memcpy_s( &orthoMatrix, sizeof( Math::Matrix4 ), &m, sizeof( D3DXMATRIX ) );
}
}