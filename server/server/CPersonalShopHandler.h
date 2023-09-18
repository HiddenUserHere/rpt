#pragma once

#include "CPersonalShopOfflineHandler.h"

class CPersonalShopHandler
{
public:
	CPersonalShopHandler();
	virtual ~CPersonalShopHandler();

	CPersonalShopOfflineHandler *		GetPersonalShopOfflineHandler() const { return pcPersonalShopOfflineHandler; }

	void								Init();

	void								Shutdown();

	void								HandlePacket( User * pcUser, PacketChatBoxMessage * psPacket );
	void								HandlePacket( User * pcUser, PacketPersonalShopItemList * psPacket );
	void								HandlePacket( User * pcUser, PacketPersonalShopTradeItems * psPacket );
	void								HandlePacket( User * pcUser, PacketPersonalShopRequestView * psPacket );
	void								HandlePacket( User * pcUser, PacketPersonalShopRequestMessage * psPacket );

private:
	void								ProcessPacket( User * pcUser, PacketPersonalShopItemList * psPacket );
	void								ProcessPacket( User * pcUser, PacketPersonalShopTradeItems * psPacket );
	void								ProcessPacket( User * pcUser, PacketPersonalShopRequestView * psPacket );

	CPersonalShopOfflineHandler			* pcPersonalShopOfflineHandler;
};

