#include "stdafx.h"
#include "CPersonalShopOfflineHandler.h"

CPersonalShopOfflineHandler::CPersonalShopOfflineHandler()
{
}

CPersonalShopOfflineHandler::~CPersonalShopOfflineHandler()
{
	vPersonalShops.clear();
}

void CPersonalShopOfflineHandler::Init()
{
	LoadShops();
}

void CPersonalShopOfflineHandler::Shutdown()
{
	vPersonalShops.clear();
}

void CPersonalShopOfflineHandler::HandlePacket( User * pcUser, PacketPersonalShopOfflineItems * psPacket )
{
	if ( pcUser->pcUserData->iPersonalShop )
	{
		//Read Items
		if ( psPacket->IsFirst() )
		{
			pcUser->vPersonalShopItems.clear();
		}

		for ( int i = 0; i < psPacket->iItemCount; i++ )
		{
			pcUser->vPersonalShopItems.push_back( PersonalShopOffline( psPacket->caItemData[i], psPacket->saItemPrice[i] ) );
		}

		if ( psPacket->IsOnly() )
		{
			SaveItemsSQL( pcUser );

			AddUser( pcUser, psPacket );
		}
	}
}

void CPersonalShopOfflineHandler::HandlePacket( User * pcUser, PacketGetUnitData * psPacket )
{
	for ( auto & sPersonalShopUser : vPersonalShops )
	{
		if ( psPacket->iID == sPersonalShopUser.iID )
		{
			USERSERVER->SendUnitInfoToUser( pcUser, sPersonalShopUser.iID, sPersonalShopUser.sCharacterData, sPersonalShopUser.sPosition, sPersonalShopUser.sAngle, false );

			CHATSERVER->SendPersonalShopChat( pcUser->pcUserData, sPersonalShopUser.iID, "%s", sPersonalShopUser.szPersonalShop );

			break;
		}
	}
}

void CPersonalShopOfflineHandler::HandlePacket( User * pcUser, PacketPersonalShopRequestMessage * psPacket )
{
	for ( auto & sPersonalShopUser : vPersonalShops )
	{
		if ( psPacket->iID == sPersonalShopUser.iID )
		{
			CHATSERVER->SendPersonalShopChat( pcUser->pcUserData, sPersonalShopUser.iID, "%s", sPersonalShopUser.szPersonalShop );

			break;
		}
	}
}

void CPersonalShopOfflineHandler::OnLoopUsers( User * pcUser )
{
	PacketPersonalShopOfflineCharacterData sPacket;
	sPacket.iTotalCharacterData = 0;

	for ( const auto & sPersonalShopUser : vPersonalShops )
	{
		if ( pcUser->GetPosition().WithinPTDistance( &sPersonalShopUser.sPosition, DISTANCE_UNIT_PARTY_MAX ) )
		{
			PacketUnitStatusMove sMovePacket;
			sMovePacket.iLength = sizeof( sMovePacket );
			sMovePacket.iHeader = PKTHDR_UnitStatusMove;
			sMovePacket.lID = sPersonalShopUser.iID;
			sMovePacket.sPosition = sPersonalShopUser.sPosition;
			sMovePacket.sAngleX = 0;
			sMovePacket.sAngleY = sPersonalShopUser.sAngle.iY;
			sMovePacket.sAngleZ = 0;
			sMovePacket.sAnimationType = ANIMATIONTYPE_Idle;
			sMovePacket.iFrame = 0;
			sMovePacket.dwHashCharacterName = 0;
			sMovePacket.bSelfUpdate = true;
			sMovePacket.bOfflineShop = true;

			sMovePacket.saEquippedItems[0] = pcUser->saEquippedItems[0];
			sMovePacket.saEquippedItems[1] = pcUser->saEquippedItems[1];
			sMovePacket.saEquippedItems[2] = pcUser->saEquippedItems[2];
			sMovePacket.saEquippedItems[3] = pcUser->saEquippedItems[3];

			USERSERVER->SendUnitStatus( pcUser, &sMovePacket );
		}
	}
}

void CPersonalShopOfflineHandler::SimulateUser( User * pcUser )
{
	PacketPersonalShopOfflineItems sPacket;
	sPacket.sPosition = pcUser->GetPosition();
	sPacket.sAngle = pcUser->GetAngle();
	sPacket.iTotalItems = 0;
	STRINGCOPY( sPacket.szMessage, "OIOIOIOI" );

	AddUser( pcUser, &sPacket );
}

bool CPersonalShopOfflineHandler::AddUser( User * pcUser, PacketPersonalShopOfflineItems * psPacket )
{
	CharacterData sCharacterData;
	if ( CHARACTERDATAHANDLER->GetSQLCharacterDataHandler()->LoadCharacterData( pcUser->iCharacterID, sCharacterData ) )
	{
		return AddUser( PersonalShopOfflineUser( USERSERVER->GetNewUserID(), pcUser->pcUserData->bServerIndexID, pcUser->iCharacterID, sCharacterData, psPacket->sPosition, psPacket->sAngle, psPacket->szMessage, psPacket->iTotalItems, "" ) );
	}

	return true;
}

bool CPersonalShopOfflineHandler::AddUser( const PersonalShopOfflineUser & sPersonalShopUser )
{
	vPersonalShops.push_back( sPersonalShopUser );

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "INSERT INTO CharacterPersonalShop([CharacterID], [PositionX], [PositionY], [PositionZ], [AngleY], [ShopMessage], [TotalItems], [ShopCode], [Date]) VALUES(?, ?, ?, ?, ?, ?, ?, ?, GETDATE())" ) )
		{
			Point3D sPosition = sPersonalShopUser.sPosition;
			sPosition >>= 8;

			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &sPersonalShopUser.iCharacterID );
			pcDB->BindParameterInput( 2, PARAMTYPE_Integer, &sPosition.iX );
			pcDB->BindParameterInput( 3, PARAMTYPE_Integer, &sPosition.iY );
			pcDB->BindParameterInput( 4, PARAMTYPE_Integer, &sPosition.iZ );
			pcDB->BindParameterInput( 5, PARAMTYPE_Integer, &sPersonalShopUser.sAngle.iY );
			pcDB->BindParameterInput( 6, PARAMTYPE_String, sPersonalShopUser.szPersonalShop, _countof( sPersonalShopUser.szPersonalShop ) );
			pcDB->BindParameterInput( 7, PARAMTYPE_Integer, &sPersonalShopUser.iTotalItems );
			pcDB->BindParameterInput( 8, PARAMTYPE_String, &sPersonalShopUser.szShopCode, _countof( sPersonalShopUser.szShopCode ) );

			pcDB->Execute();
		}

		pcDB->Close();
	}

	return true;
}

bool CPersonalShopOfflineHandler::RemoveUser( int iCharacterID )
{
	for ( auto it = vPersonalShops.begin(); it != vPersonalShops.end(); )
	{
		if ( it->iCharacterID == iCharacterID )
		{
			it = vPersonalShops.erase( it );
		}
		else
		{
			it++;
		}
	}

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "DELETE FROM CharacterPersonalShop WHERE CharacterID=?" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iCharacterID );
			pcDB->Execute();
		}

		pcDB->Close();
	}

	return true;
}

void CPersonalShopOfflineHandler::SaveItemsSQL( User * pcUser )
{
	//Save Items
	if ( pcUser->vPersonalShopItems.size() > 0 )
	{
		if ( IsValidItems( pcUser ) )
		{

			std::vector<SQLRecordItem> vItems;
			for ( auto & sPersonalShopItem : pcUser->vPersonalShopItems )
			{
				SQLRecordItem sItem;
				sItem.sRecordItem.iItemPosition = sPersonalShopItem.cItemData.iItemSlot;
				sItem.sRecordItem.sItemPosition = sPersonalShopItem.cItemData.sPosition;
				sItem.sRecordItem.sItem = sPersonalShopItem.cItemData.sItem;
				vItems.push_back( sItem );
			}

			CHARACTERDATAHANDLER->GetSQLCharacterItemHandler()->SaveCharacterItems( pcUser->iAccountID, pcUser->iCharacterID, vItems, ECharacterItemDataID::CHARACTERITEMDATAID_PersonalShopOffline );
		}
	}
}

bool CPersonalShopOfflineHandler::IsValidItems( User * pcUser )
{
	for ( const auto & cItemData : pcUser->vPersonalShopItems )
	{
		if ( ITEMSERVER->CheckValidItem( &cItemData.cItemData.sItem ) == FALSE )
		{
			return false;
		}
	}

	return true;
}

void CPersonalShopOfflineHandler::ProcessPacket( User * pcUser, PacketPersonalShopOfflineCharacterData * psPacket )
{
	if ( (psPacket->iTotalCharacterData > 0) && (psPacket->iTotalCharacterData <= MAX_PERSONALSHOPOFFLINE_CHARACTERDATA) )
	{
		psPacket->iLength = (sizeof( PersonalShopOfflineCharacterData ) * psPacket->iTotalCharacterData) + sizeof( Packet ) + sizeof( int );
		psPacket->iHeader = PKTHDR_PersonalShopOfflineCharacterData;

		SENDPACKET( pcUser, psPacket );

		psPacket->iTotalCharacterData = 0;
	}
}

bool CPersonalShopOfflineHandler::LoadShops()
{
	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "SELECT [CharacterID], [PositionX], [PositionY], [PositionZ], [AngleY], [ShopMessage], [TotalItems], [ShopCode] FROM CharacterPersonalShop" ) )
		{
			if ( pcDB->Execute() )
			{
				if ( pcDB->Fetch() )
				{

				}
			}
		}

		pcDB->Close();
	}

	return false;
}
