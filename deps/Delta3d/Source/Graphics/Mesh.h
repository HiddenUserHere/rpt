#pragma once

#include "Graphics.h"

#include "../IO/SMD/Frame.h"
#include "../IO/SMD/Header.h"
#include "../IO/SMD/ObjectInfo.h"
#include "../IO/SMD/KeyPosition.h"
#include "../IO/SMD/KeyScale.h"
#include "../IO/SMD/KeyRotation.h"
#include "../IO/SMD/Face.h"
#include "../IO/SMD/Vertex.h"
#include "../IO/SMD/TextureLink.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4.h"
#include "../Math/BoundingBox.h"
#include "../Math/Sphere.h"
#include "../Math/Quaternion.h"

#include "../Legacy/LegacyMesh.h"


namespace Delta3D::Graphics
{
class IndexBuffer;
class VertexBuffer;
class Shader;
class Model;
class Material;
class MeshPart;

struct MeshAnimationDataOld
{
	Math::Vector3			vBaseScale;
	Math::Quaternion		vBaseQuaternion;
	Math::Vector3			vBasePosition;

	Math::Vector3			vLastScale;
	Math::Quaternion		vLastQuaternion;
	Math::Vector3			vLastPosition;
};



enum class MeshRenderResult
{
	Undefined,
	Render,
	NotRender,
	Transparent,
	Opacity,
};

struct MeshKeyIndexFinder
{
	int				iKeyIndex;

    int				iBeginFrame;
    int				iEndFrame;

	MeshKeyIndexFinder( int _iKeyIndex, int _iBeginFrame, int _EndFrame )
	{
		iKeyIndex	= _iKeyIndex;
		iBeginFrame = _iBeginFrame;
		iEndFrame	= _EndFrame;
	};

	~MeshKeyIndexFinder() {};
};

struct MeshSkinningVertex
{
	Vector3		sVertex;
	
	float		faBoneIndice[4];
	float		faWeight[4];

    MeshSkinningVertex( Vector3 & v, float _faBoneIndice[4], float _faWeight[4] )
    {
        sVertex = v;
        for ( int i = 0; i < 4; i++ )
        {
			faBoneIndice[i] = abs( _faBoneIndice[i] );
            faWeight[i] = abs( _faWeight[i] );
        }
    };

    MeshSkinningVertex( Vector3 & v, unsigned short _saBoneIndice[4], float _faWeight[4] )
    {
        sVertex = v;
        for ( int i = 0; i < 4; i++ )
        {
            faBoneIndice[i] =  abs( (float)_saBoneIndice[i] );
            faWeight[i] = abs( _faWeight[i] );
        }
    };
	MeshSkinningVertex() {};
	~MeshSkinningVertex() {};
};

const UINT MAX_MESHKEY_FINDER = 32;

class Mesh : public GraphicsImpl
{
public:
	//! Default Constructor for Mesh.
	Mesh();
	
	//! Construct a Mesh with a number of Vertices and Faces specified.
	Mesh( int verticesCount_, int facesCount_ );

	//! Deconstructor.
	~Mesh();

	/**
	 * Add a Vertex to Mesh
	 * @param x X Coordinate from Vertex
	 * @param y Y Coordinate from Vertex
	 * @param z Z Coordinate from Vertex
	 * @return Total Mesh Vertices
	 */
	int AddVertex( int x, int y, int z );

	/**
	 * Add a Face to Mesh
	 * @param a Vertex A from Triangle Face
	 * @param b Vertex B from Triangle Face
	 * @param c Vertex C from Triangle Face
	 * @param materialID Material ID for Face
	 * @param texA Texture coordinates for Vertex A
	 * @param texB Texture coordinates for Vertex B
	 * @param texC Texture coordinates for Vertex C
	 * @return Total Mesh Faces
	 */
	int AddFace( int a, int b, int c, int materialID = 0, Math::Vector2 texA = Math::Vector2::Null, Math::Vector2 texB = Math::Vector2::Null, Math::Vector2 texC = Math::Vector2::Null );

	/**
	 * Add a Texture Coordinate on Mesh List
	 * @param faceIndex Index of desired Face
	 * @param texA Texture coordinates for Vertex A
	 * @param texB Texture coordinates for Vertex B
	 * @param texC Texture coordinates for Vertex C
	 * @return Total Textures Coordinates
	 */
	int AddTextureCoord( int faceIndex, Math::Vector2 texA = Math::Vector2::Null, Math::Vector2 texB = Math::Vector2::Null, Math::Vector2 texC = Math::Vector2::Null );

	/**
	 * Apply a Rotation Transform to Animation Matrix
	 */
	void ApplyRotationTransform( Math::Matrix4& out, Math::Matrix4* previousRotation_, IO::SMD::KeyRotation* frameRotation_, int frame_, IO::SMD::FrameInfo* frameInfo = nullptr );

	/**
	 * Apply a Scaling Transform to Animation Matrix
	 */
	void ApplyScalingTransform( Math::Matrix4& out, IO::SMD::KeyScale* frameScaling_, int frame_ );

	/**
	 * Apply a Translation Transform to Animation Matrix
	 */
	void ApplyTranslationTransform( Math::Matrix4& out, IO::SMD::KeyPosition* framePosition_, int frame_ );

	/**
	 * Find Animation of Position specified by Frame
	 * @param frame Frame desired to find
	 * @return Index of Animation Frame Info
	 */
	int FindAnimationPosition( int frame );

	/**
	 * Find Animation of Rotation specified by Frame
	 * @param frame Frame desired to find
	 * @return Index of Animation Frame Info
	 */
	int FindAnimationRotation( int frame );

	/**
	 * Find Animation of Scaling specified by Frame
	 * @param frame Frame desired to find
	 * @return Index of Animation Frame Info
	 */
	int FindAnimationScaling( int frame );

	/**
	 * Animate Model
	 * @param frame_ Frame desired to make Animation
	 * @param rotation_ Rotation to Model
	 * @param frameInfo Animation info
	 */
	void Animate( int frame_ = 0, Math::Vector3Int rotation_ = Math::Vector3Int::Null, IO::SMD::FrameInfo* frameInfo = nullptr );

	/**
	 * Set a Position and Rotation for Mesh
	 * @param position_ Position desired
	 * @param rotation_ Rotation desired
	 */
	void SetPositionRotation( Math::Vector3* position_, Math::Vector3Int* rotation_ );

	/**
	 * Update Bounding Volumes from Mesh
	 */
	void UpdateBoundingVolumes();

	//! Find a similar Vertex and return your Index.
	inline int FindSimilarVertex( IO::SMD::PackedVertex& packed, std::map<IO::SMD::PackedVertex, unsigned int> & cache, unsigned short& out );

	//! Check if Mesh can be renderer now.
	MeshRenderResult CanRender();

	//! Software Skinning.
	inline void Skinning();

	//! Update Mesh.
	void Update( float timeElapsed );

	//! Render Mesh.
	int Render();

	//! Build Mesh. (File should be already opened)
	bool Build( Model* skeleton = nullptr, Legacy::Mesh* legacyMesh_ = nullptr );

private:
	void ClipInfluenceEdges( MeshInfluenceVertex & sVertex1, MeshInfluenceVertex & sVertex2 );

	bool GetWorldMatrix( int frame_, Math::Vector3Int rotation_, IO::SMD::FrameInfo * frameInfo, Math::Matrix4 & sWorld );

	bool GetMatricesAnimation( int frame_, Math::Vector3Int rotation_, IO::SMD::FrameInfo * frameInfo, Math::Matrix4 & sRotation, Math::Matrix4 & sScale, Math::Matrix4 & sTranslation );

public:

	std::string strName;

	unsigned int verticesCount;
	unsigned int facesCount;
	unsigned int texturesCoordCount;

	Math::BoundingBox worldBoundingBox;	//!< World Bounding Box
	Math::Sphere boundingSphere;	//!< Bounding Sphere
	Math::Matrix4 translation;	//!< Mesh Translation Transformation
	Math::Matrix4 rotationMatrix;

	Math::Matrix4 transformationMatrix;
	Math::Matrix4 inverseMatrix;
	Math::Matrix4 offsetMatrix;


	Math::Matrix4 world;	//!< World Transform Matrix
	Math::Matrix4 local;	//!< Local Transform Matrix

	std::shared_ptr<VertexBuffer> vertexPositionBuffer;	//!< Mesh Vertex Buffer
    std::shared_ptr<VertexBuffer> vertexNormalBuffer;	//!< Mesh Normals Buffer
    std::shared_ptr<VertexBuffer> vertexTangentBuffer;	//!< Mesh Tangent Buffer
    std::shared_ptr<VertexBuffer> vertexBitangentBuffer;	//!< Mesh Bitangent Buffer
    std::shared_ptr<VertexBuffer> vertexBlendIndicesBuffer;	//!< Mesh Blend Indices Buffer
    std::shared_ptr<VertexBuffer> vertexBlendWeightsBuffer;	//!< Mesh Blend Weights Buffer
	std::vector<std::shared_ptr<VertexBuffer>> textureCoordsBuffer;	//!< Textures Coordinates Buffer

	std::vector<int> skinnedVerticesIndex;	//!< Skinned Vertices Index (if Skinned Mesh)
	std::vector<MeshSkinningVertex> vertexData;	//!< Used for Software Skinning

	std::unordered_map<Material*, MeshPart*> meshParts;	//!< Mesh Parts (by material)

	Model* modelParent;	//!< Pointer to Model Parent from this Mesh

	bool postRender;	//!< Flag to render mesh after everything

	Legacy::Mesh* legacyMesh;

    std::vector<MeshKeyIndexFinder> vKeyPositionFinder;
    std::vector<MeshKeyIndexFinder> vKeyRotationFinder;
    std::vector<MeshKeyIndexFinder> vKeyScaleFinder;

	Vector3					vLastScale;
	Quaternion				vLastQuaternion;
	Vector3					vLastPosition;

	float					fAnimationBlendingTime;
	MeshAnimationDataOld	vMeshAnimationOld;
};
}