#pragma once
class CEffekSeerParticleSystem
{
private:
	Effekseer::EffectRef			pcEffect = nullptr;
	Effekseer::Handle			iHandle = -1;

	UINT						iID = 0;

	Engine::ObjectOwner			* pcOwner = nullptr;

	BOOL						bKill = FALSE;

	bool						bRotateToViewPositon = false;

public:
	CEffekSeerParticleSystem( Effekseer::EffectRef _pcEffect );
	CEffekSeerParticleSystem( const CEffekSeerParticleSystem & c );
	~CEffekSeerParticleSystem();

	Effekseer::EffectRef		GetEffect() { return pcEffect; }

	Effekseer::Handle			GetHandle() { return iHandle; }
	void						SetHandle( Effekseer::Handle _iHandle ) { iHandle = _iHandle; }

	UINT						GetID() { return iID; }
	void						SetID( UINT _iID ) { iID = _iID; }

	Engine::ObjectOwner			* GetOwner() { return pcOwner; }
	void						SetOwner( Engine::ObjectOwner * _pcOwner );

	UINT						GetOwnerID();

	void						Kill( BOOL bForce );

	void						Render( Effekseer::ManagerRef pcManager, BOOL bBack );

	BOOL						Update( float fTime );

	void						SetRotateToView( bool b ) { bRotateToViewPositon = b; }

	bool						IsRotateToView() { return bRotateToViewPositon; }
};