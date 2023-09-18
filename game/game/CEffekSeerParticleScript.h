#pragma once

#include "CEffekSeerParticleSystem.h"

class CEffekSeerParticleScript
{
private:
	UINT					uID = 0xFFFFFFFF;

	BOOL					bCache = FALSE;

	std::string				strFileName;

	Effekseer::EffectRef	pcEffect = nullptr;

	std::vector<CEffekSeerParticleSystem *> vParticleSystems;

public:
	CEffekSeerParticleScript( const std::string _strFileName, Effekseer::EffectRef _pcEffect );
	CEffekSeerParticleScript( const CEffekSeerParticleScript & c );
	~CEffekSeerParticleScript();

	void					Clear();

	Effekseer::EffectRef	GetEffect() { return pcEffect; }
	void					SetEffect( Effekseer::EffectRef _pcEffect ) { pcEffect = _pcEffect; }

	void					SetOwner( Engine::ObjectOwner * _pcOwner );

	UINT					GetID() { return uID; }
	void					SetID( UINT _uID );

	void					SetRotateToView( bool b );

	std::string				GetFileName() { return strFileName; }
	void					SetFileName(const std::string _strFileName ) { strFileName = _strFileName; }

	std::vector<CEffekSeerParticleSystem *> & GetParticleSystems() { return vParticleSystems; }
};

