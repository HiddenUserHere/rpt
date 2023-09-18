#pragma once

class CPersonalShopOfflineHandler
{
public:
	CPersonalShopOfflineHandler();

	~CPersonalShopOfflineHandler();

	void						Init();

	void						Shutdown();

	void						HandlePacket( User * pcUser, PacketPersonalShopOfflineItems * psPacket );
	void						HandlePacket( User * pcUser, PacketGetUnitData * psPacket );
	void						HandlePacket( User * pcUser, PacketPersonalShopRequestMessage * psPacket );

	void						OnLoopUsers( User * pcUser );

	void 						SimulateUser( User * pcUser );

private:

	bool						AddUser( User * pcUser, PacketPersonalShopOfflineItems * psPacket );
	bool						AddUser( const PersonalShopOfflineUser & sPersonalShopUser );

	bool						RemoveUser( int iCharacterID );

	void						SaveItemsSQL( User * pcUser );

	bool						IsValidItems( User * pcUser );

	void						ProcessPacket( User * pcUser, PacketPersonalShopOfflineCharacterData * psPacket );

	bool						LoadShops();

	std::vector<PersonalShopOfflineUser> vPersonalShops;
};

