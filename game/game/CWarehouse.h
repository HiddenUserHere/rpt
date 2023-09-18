#pragma once
#include "CWareHouseWindow.h"

class CWarehouse
{
protected:
	class CWarehouseWindow						* pcWarehouseWindow = NULL;

	const int									MAX_RETRY_WAREHOUSE = 3;

	int											iSending = 0;
	DWORD										dwTimeSending = 0;

public:
	CWarehouse();
	virtual ~CWarehouse();

	class CWarehouseWindow						* GetWindow() { return pcWarehouseWindow; };

	void										Init();

	void										Update();

	BOOL										OnMouseClick( class CMouse * pcMouse );

	int											GetPage() { return (*(int*)0x3693304) + 1; };
	void										SetPage(int iPage);

	int											OnRenderTime(struct Skill * psSkill );

	void										OpenNPCQuest();

	ItemData									* GetItemData();

	void										HandlePacket( struct PacketWarehouse * psPacket );

	BOOL										OnUnitKilled( class UnitData * pcUnit );

	void										RestoreItems();

	BOOL										IsWarehouseWindowOpen() { return (*(BOOL*)0x036932FC) || (iSending > 0); }

	ItemData									* GetMouseItemHover();

	void										OnWareHouseRetry();

	BOOL										UpdateWarehouseTime();

	void										OnWareHouseSaved();
};

