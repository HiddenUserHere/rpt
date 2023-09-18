#pragma once

#include <objbase.h>
#include <vector>
#include <comdef.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

#define MAX_PARAMS		3000
#define MAX_QUERY_SIZE	8192

#define MAX_SQL_BULK_ARRAY_COUNT	200

template<typename T, typename U>
inline bool IsSameEx = std::is_same_v<std::remove_const<std::remove_reference<T>::type>::type, std::remove_const<std::remove_reference<U>::type>::type>;


#define IsSame std::is_same_v

template<typename T, typename U>
inline bool IsSamePtrFix = std::is_same_v<std::remove_const<std::remove_reference<std::remove_pointer<T>::type>::type>::type, std::remove_const<std::remove_reference<std::remove_pointer<U>::type>::type>::type>;


enum EDatabaseID
{
	DATABASEID_None,
	DATABASEID_GameDB,
	DATABASEID_UserDB,
	DATABASEID_ServerDB,
	DATABASEID_LogDB,
	DATABASEID_SkillDB,
	DATABASEID_EventDB,
	DATABASEID_ClanDB,
	DATABASEID_SkillDBNew,
	DATABASEID_BotDB,
};

enum EDatabaseDataType
{
	PARAMTYPE_Integer,
	PARAMTYPE_String,
	PARAMTYPE_Float,
	PARAMTYPE_Int64,
	PARAMTYPE_Short,
	PARAMTYPE_Time,
	PARAMTYPE_Binary,
	PARAMTYPE_Byte,
	PARAMTYPE_Null = 5000,
};

enum EParamWhere
{
	PARAMWHERE_None,
	PARAMWHERE_Equal,
	PARAMWHERE_NotEqual,
	PARAMWHERE_Less,
	PARAMWHERE_LessEqual,
	PARAMWHERE_Greater,
	PARAMWHERE_GreaterEqual,
	PARAMWHERE_Like,
	PARAMWHERE_NotLike,
	PARAMWHERE_In,
	PARAMWHERE_NotIn,
	PARAMWHERE_IsNull,
	PARAMWHERE_IsNotNull,
	PARAMWHERE_Between,
	PARAMWHERE_NotBetween,
};

struct DatabaseStructureData
{
	EDatabaseID									eDatabaseID;
	const char									* pszDatabaseName;
};

struct SQLColumnData
{
	int											iPosition;
	std::string									strName;
	EDatabaseDataType							eDataType;
	int											iSize;

	SQLColumnData()
	{
		iPosition = -1;
		eDataType = PARAMTYPE_Null;
		iSize = 0;
		strName = "";
	}

	SQLColumnData( int _iPosition, const std::string & _strName, EDatabaseDataType _eDataType, int _iSize )
		: iPosition( _iPosition ), strName( _strName ), eDataType( _eDataType ), iSize( _iSize )
	{
	}

	~SQLColumnData()
	{
	}

	inline void operator=( const SQLColumnData & sSQLColumnData )
	{
		iPosition = sSQLColumnData.iPosition;
		strName = sSQLColumnData.strName;
		eDataType = sSQLColumnData.eDataType;
		iSize = sSQLColumnData.iSize;
	}
};

class SQLConnection
{
private:
	DatabaseStructureData						* GetDatabaseData( EDatabaseID eDatabaseID );
	SQLHANDLE									hEnv = INVALID_HANDLE_VALUE;
	SQLHANDLE									hConnection = INVALID_HANDLE_VALUE;
	SQLHANDLE									hStatement = INVALID_HANDLE_VALUE;
	SQLHANDLE									hDescriptor = INVALID_HANDLE_VALUE;
	EDatabaseID									eID;
	char										szDatabaseName[64] = { 0 };

	char										szQuery[MAX_QUERY_SIZE] = { 0 };

	CMutex										* pcMutex = nullptr;

	BOOL										bOpen		= FALSE;
	BOOL										bPrepared	= FALSE;

	void										* pLastAddress = NULL;
	void										* pLastAfterAddress = NULL;

	std::vector<SQLColumnData>					vColumns;
	
	UINT										uCurrentBindPosition = 1;

	SQLUSMALLINT								iaRowStatus[MAX_SQL_BULK_ARRAY_COUNT];
	SQLLEN										baBindLength[MAX_PARAMS];
	SQLLEN										iaBindLength[MAX_PARAMS][MAX_SQL_BULK_ARRAY_COUNT];

	SQLLEN										* BindLengthNext();
	void										BindLengthFree();

	SQLUINTEGER									iSQLParamArraySize;
	SQLLEN										iBindOffset;
	SQLLEN										iRowsFetched;
	bool										bFirstFetch = true;

	bool										baBlock[MAX_PARAMS];
	SQLINTEGER									iaBlock[MAX_PARAMS];
	SQLINTEGER									* BlockNext();
	void										BlockFree();

	EDatabaseDataType							SQLDataTypeToDatabaseDataType( SQLSMALLINT iDataType );

	void										UpdateColumnsInformaton();

	static std::string							DatabaseTypeToString( EDatabaseDataType eType );

	BOOL										FetchExInternal( const UINT iArrayCount, const UINT iArraySize );

public:

	UINT										GetRowsFetchedCount() { return iRowsFetched; };

	void										Init( EDatabaseID eDatabaseID );

	BOOL										SQLTimeStampToSystemTime( SYSTEMTIME * psSystemTime, SQL_TIMESTAMP_STRUCT * psSQLTimeStamp );
	void										SQLSystemTimeToTimeStamp( SYSTEMTIME * in, SQL_TIMESTAMP_STRUCT * out );
	BOOL										Open();
	BOOL										Prepare( const char * pszQuery );
	BOOL										Execute();
	BOOL										Execute( const char * pszQuery );

	BOOL										UpdateInputArraySize( const UINT iArrayCount, const UINT iArraySize );

	BOOL										BindParameterInput( int iPosition, EDatabaseDataType eDataType, const void * pParameter, int iSize = 0 );
	BOOL										GetData( int iPosition, EDatabaseDataType eDataType, void * pParameter, int iSize = 0 );
	BOOL										GetData( const std::string & strName, EDatabaseDataType eDataType, void * pParameter, int iSize = 0 );

	BOOL										BindData( int iPosition, EDatabaseDataType eDataType, void * pParameter, int iSize = 0 );

	template<typename T>
	T										GetData( int iPosition, SYSTEMTIME * pOutBuffer = nullptr )
	{
		static_assert(IsSame<T, int> || IsSame<T, unsigned int> || IsSame<T, long> || IsSame<T, unsigned long> ||
			IsSame<T, short> || IsSame<T, unsigned short> || 
			IsSame<T, INT64> || IsSame<T, UINT64> || 
			IsSame<T, float> ||
			IsSame<T, char> || IsSame<T, unsigned char>,
			"Invalid type for GetData");

		SQLColumnData sSQLColumnData;
		if ( GetColumnData( iPosition, sSQLColumnData ) )
		{
			switch ( sSQLColumnData.eDataType )
			{
				case PARAMTYPE_Integer:
				{
					if constexpr ( IsSame<T, int> || IsSame<T, unsigned int> || IsSame<T, long> || IsSame<T, unsigned long> )
					{
						int iValue = 0;
						GetData( iPosition, sSQLColumnData.eDataType, &iValue, sizeof( int ) );
						return (T)iValue;
					}
				}
				break;

				case PARAMTYPE_Short:
				{
					if constexpr ( IsSame<T, short> || IsSame<T, unsigned short> )
					{
						short sValue = 0;
						GetData( iPosition, sSQLColumnData.eDataType, &sValue, sizeof( short ) );
						return (T)sValue;
					}
				}
				break;

				case PARAMTYPE_Float:
				{
					if constexpr( IsSame<T, float> )
					{
						float fValue = 0;
						GetData( iPosition, sSQLColumnData.eDataType, &fValue, sizeof( float ) );
						return (T)fValue;
					}
				}
				break;

				case PARAMTYPE_Int64:
				{
					if constexpr ( IsSame<T, INT64> || IsSame<T, UINT64> )
					{
						INT64 iValue = 0;
						GetData( iPosition, sSQLColumnData.eDataType, &iValue, sizeof( INT64 ) );
						return (T)iValue;
					}
				}
				break;

				case PARAMTYPE_Byte:
				{
					if constexpr ( IsSame<T, char> || IsSame<T, unsigned char> )
					{
						BYTE bValue = 0;
						GetData( iPosition, sSQLColumnData.eDataType, &bValue, sizeof( BYTE ) );
						return (T)bValue;
					}
				}
				break;

				default:
					break;
			}
		}

		return T();
	};

	/*
	* @param strColumnName - Column name
	* @param pOutBuffer - Pointer to SYSTEMTIME structure
	*/
	template<typename T>
	T 											GetData( const std::string strColumnName, SYSTEMTIME * pOutBuffer = nullptr )
	{
		return GetData<T>( GetColumnIndex( strColumnName.c_str() ), pOutBuffer );
	};

	std::string						GetDataString( int iPosition )
	{
		SQLColumnData sSQLColumnData;
		if ( GetColumnData( iPosition, sSQLColumnData ) )
		{
			switch ( sSQLColumnData.eDataType )
			{
				case PARAMTYPE_String:
				{
					std::string strValue;
					strValue.resize( sSQLColumnData.iSize );
					GetData( iPosition, sSQLColumnData.eDataType, (void *)strValue.data(), sSQLColumnData.iSize );
					return strValue;
				}
				break;

				default:
					break;
			}
		}

		return std::string();
	};

	std::string						GetDataString( const std::string & strColumnName )
	{
		return GetDataString( GetColumnIndex( strColumnName.c_str() ) );
	};

	template<typename T>
	void										AddData( const EDatabaseDataType iParamType, const std::string & strColumnName, T & tParameter )
	{
		if ( (iParamType == PARAMTYPE_Integer) ||
		     (iParamType == PARAMTYPE_Short) || 
			 (iParamType == PARAMTYPE_Float) ||
			 (iParamType == PARAMTYPE_Int64) ||
			 (iParamType == PARAMTYPE_Time) ||
			 (iParamType == PARAMTYPE_Byte) )
		{
			BindParameterInput( uCurrentBindPosition++, iParamType, &tParameter, sizeof( T ) );
		}
	};

	void										AddDataString( const std::string & strColumnName, const char * tParameter )
	{
		BindParameterInput( uCurrentBindPosition++, PARAMTYPE_String, tParameter );
	};

	void										AddDataBinary( const std::string & strColumnName, void * tParameter, int iSize )
	{
		BindParameterInput( uCurrentBindPosition++, PARAMTYPE_Binary, tParameter, iSize );
	};

	BOOL										GetColumnName( int iPosition, void * pParameter, int iSize );
	BOOL										GetColumnCount( int & iCount );
	EDatabaseDataType							GetColumnType( const char * pszTableName, const char * pszColumnName );
	int											GetColumnIndex( const char * pszColumnName );
	bool										GetColumnData( const char * pszColumnName, SQLColumnData & sSQLColumnData );
	bool										GetColumnData( int iPosition, SQLColumnData & sSQLColumnData );

	BOOL										Fetch();

	template<const UINT iArrayCount>
	constexpr BOOL								FetchEx( const UINT iArraySize = 0 )
	{
		static_assert(iArrayCount <= MAX_SQL_BULK_ARRAY_COUNT, "iArrayCount cannot be greater than MAX_SQL_BULK_ARRAY_COUNT");
		return FetchExInternal( iArrayCount, iArraySize );
	};

	BOOL										Close();
	BOOL										Clear();

	static SQLConnection						* GetConnection( EDatabaseID eDatabaseID );

	SQLConnection();
	virtual ~SQLConnection();
};
