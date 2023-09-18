#pragma once

#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"

#include "../IO/SMD/KeyPosition.h"
#include "../IO/SMD/KeyScale.h"
#include "../IO/SMD/KeyRotation.h"
#include "../IO/SMD/Frame.h"
#include "../IO/SMD/Vertex.h"
#include "../IO/SMD/Face.h"
#include "../IO/SMD/TextureLink.h"

struct MeshInfluenceVertex
{
    short			saBoneIndex[4];
    float			faWeight[4];
};

struct MeshVertexFloatOld
{
    Delta3D::Math::Vector3 sVertex;
    Delta3D::Math::Vector3 sNormal;

    Delta3D::Math::Vector3 sTangent;
	Delta3D::Math::Vector3 sBitangent;
};

namespace Delta3D::Legacy
{

class Mesh
{
public:
	DWORD Head;

	IO::SMD::Vertex* Vertex;
	IO::SMD::Face* Face;
	IO::SMD::TextureLink* TexLink;

	Mesh** Physique;

	IO::SMD::Vertex ZeroVertex;

	int maxZ, minZ;
	int maxY, minY;
	int maxX, minX;

	int dBound;
	int Bound;

	int MaxVertex;
	int MaxFaces;

	int nVertex;
	int nFace;

	int nTexLink;

	int ColorEffect;
	DWORD ClipStates;

	Math::Vector3Int Posi;
	Math::Vector3Int CameraPosi;
	Math::Vector3Int Angle;
	int	Trig[8];

	char NodeName[32];
	char NodeParent[32];
	Mesh* pParent;

	Math::Matrix4Int Tm;
	Math::Matrix4Int TmInvert;

	Math::Matrix4 TmResult;
	Math::Matrix4Int TmRotate;

	Math::Matrix4Int mWorld;
	Math::Matrix4Int mLocal;

	int	lFrame;

	float qx, qy, qz, qw;
	int sx, sy, sz;
	int px, py, pz;

	IO::SMD::KeyRotation *TmRot;
	IO::SMD::KeyPosition *TmPos;
	IO::SMD::KeyScale *TmScale;

	Math::Matrix4* TmPrevRot;

	int TmRotCnt;
	int TmPosCnt;
	int TmScaleCnt;

	IO::SMD::Frame TmRotFrame[32];
	IO::SMD::Frame TmPosFrame[32];
	IO::SMD::Frame TmScaleFrame[32];
	int TmFrameCnt;

	void * newMesh;

    BOOL								bUseInfluences;
    std::vector<MeshInfluenceVertex>	vInfluences;

    std::vector<MeshVertexFloatOld>			vFloatVertices;
    Math::Matrix4						sTransformationMatrixF;
};
};