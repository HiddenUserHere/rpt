#include "stdafx.h"
#include "SQLConnection.h"

std::vector<SQLConnection*> vSQLConnection;

#define SQLSUCCEEDED( ret ) ( ( ret == SQL_SUCCESS ) || ( ret == SQL_SUCCESS_WITH_INFO ) )

BOOL show_errorSQL( unsigned int handletype, const SQLHANDLE& handle )
{
	static SQLCHAR sqlstateSqlError[ 4096 ];
	static SQLCHAR messageSqlError[ 4096 ];
	static char szBufSQLError[ 8192 ];
	ZeroMemory( szBufSQLError, 8192 );
	ZeroMemory( sqlstateSqlError, 4096 );
	ZeroMemory( messageSqlError, 4096 );

	//SQLGetDescRecA()

	if ( SQLSUCCEEDED( SQLGetDiagRecA( handletype, handle, 1, sqlstateSqlError, NULL, messageSqlError, 4096, NULL ) ) )
	{
		STRINGFORMAT( szBufSQLError, "Message: %s\nSQLSTATE: %s\n", messageSqlError, sqlstateSqlError );
		WRITEERR( szBufSQLError );
		return TRUE;
	}


	return FALSE;
}

BOOL ShowSQLDescriptorError( const SQLHANDLE & hDescriptor )
{
	//Get Column name of the error
	SQLCHAR         szName[255];
	SQLSMALLINT     iBufferLength = 255;
	SQLSMALLINT     iStringLength;
	SQLSMALLINT     iType;
	SQLSMALLINT     iSubType;
	SQLLEN          iLength;
	SQLSMALLINT     iPrecision;
	SQLSMALLINT     iScale;
	SQLSMALLINT     iNullable;

	SQLSMALLINT     iRecNumber;

	SQLSMALLINT     iDescriptorCount = 0;


	if ( SQLSUCCEEDED( SQLGetDescField( hDescriptor, 0, SQL_DESC_COUNT, &iDescriptorCount, 0, NULL ) ) )
	{
		for ( iRecNumber = 1; iRecNumber <= iDescriptorCount; iRecNumber++ )
		{
			if ( SQLSUCCEEDED( SQLGetDescRecA( hDescriptor, iRecNumber, szName, iBufferLength, &iStringLength, &iType, &iSubType, &iLength, &iPrecision, &iScale, &iNullable ) ) )
			{
				WRITEERR( "Column[%d][%s]\n", iRecNumber, szName );
			}
		}
	}

	return TRUE;
}

DatabaseStructureData saDatabaseData[]
{
	{ DATABASEID_GameDB,	"GameDB" },
	{ DATABASEID_UserDB,	"UserDB" },
	{ DATABASEID_ServerDB,	"ServerDB" },
	{ DATABASEID_LogDB,		"LogDB" },
	{ DATABASEID_SkillDB,	"SkillDB" },
	{ DATABASEID_EventDB,	"EventDB" },
	{ DATABASEID_ClanDB,	"ClanDB" },
	{ DATABASEID_SkillDBNew,	"SkillDBNew" },
	{ DATABASEID_BotDB,		"BotDB" },

};


SQLConnection::SQLConnection()
{
	eID = DATABASEID_None;

	for (int i = 0; i < MAX_PARAMS; i++)
		baBlock[i] = false;

	for (int i = 0; i < MAX_PARAMS; i++)
		iaBlock[i] = 0;

	//Create mutex
	pcMutex = new CMutex( "SQLConnection" );
}


SQLConnection::~SQLConnection()
{
	DELET( pcMutex );
}

EDatabaseDataType SQLConnection::SQLDataTypeToDatabaseDataType( SQLSMALLINT iDataType )
{
	EDatabaseDataType eDataType = PARAMTYPE_Null;

	switch ( iDataType )
	{
		case SQL_TINYINT:
			eDataType = PARAMTYPE_Byte;
			break;
		case SQL_INTEGER:
			eDataType = PARAMTYPE_Integer;
			break;
		case SQL_BIGINT:
			eDataType = PARAMTYPE_Int64;
			break;
		case SQL_SMALLINT:
			eDataType = PARAMTYPE_Short;
			break;
		case SQL_REAL:
		case SQL_FLOAT:
			eDataType = PARAMTYPE_Float;
			break;
		case SQL_VARCHAR:
			eDataType = PARAMTYPE_String;
			break;
		case SQL_BINARY:
		case SQL_VARBINARY:
			eDataType = PARAMTYPE_Binary;
			break;
		case SQL_TYPE_TIMESTAMP:
			eDataType = PARAMTYPE_Time;
			break;

		default:
			break;
	}

	return eDataType;
}

DatabaseStructureData * SQLConnection::GetDatabaseData( EDatabaseID eDatabaseID )
{
	if ( GSERVER->GetServerType() != SERVERTYPE_Multi )
	{
		for ( auto & v : saDatabaseData )
		{
			if ( v.eDatabaseID == eDatabaseID )
				return &v;
		}
	}
	else
	{
		for ( auto & v : saDatabaseData )
		{
			if ( v.eDatabaseID == eDatabaseID )
				return &v;
		}
	}

	return NULL;
}

SQLLEN * SQLConnection::BindLengthNext()
{
	for ( int i = 0; i < MAX_PARAMS; i++ )
	{
		if ( baBindLength[i] == false )
		{
			baBindLength[i] = true;
			return iaBindLength[i];
		}
	}

	return nullptr;
}

void SQLConnection::BindLengthFree()
{
	for ( int i = 0; i < MAX_PARAMS; i++ )
		baBindLength[i] = false;
}

SQLINTEGER * SQLConnection::BlockNext()
{
	for ( int i = 0; i < MAX_PARAMS; i++ )
	{
		if ( baBlock[i] == false )
		{
			baBlock[i] = true;
			return &iaBlock[i];
		}
	}

	return NULL;
}

void SQLConnection::BlockFree()
{
	for ( int i = 0; i < MAX_PARAMS; i++ )
		baBlock[i] = false;
}

void SQLConnection::UpdateColumnsInformaton()
{
	SQLSMALLINT iColumnsCount = 0;
	SQLCHAR szColumnName[ 256 ];
	SQLSMALLINT iNameLength = 0;
	SQLSMALLINT iDataType = 0;
	SQLULEN iColumnSize = 0;
	SQLSMALLINT iDecimalDigits = 0;
	SQLSMALLINT iNullable = 0;

	//Get Number of Columns
	SQLNumResultCols( hStatement, &iColumnsCount );

	//Clear Columns information
	vColumns.clear();

	//Get Columns information
	for ( int i = 1; i <= iColumnsCount; i++ )
	{
		//Get Column name
		SQLDescribeColA( hStatement, i, szColumnName, sizeof( szColumnName ), &iNameLength, &iDataType, &iColumnSize, &iDecimalDigits, &iNullable );

		//Add Column information
		vColumns.push_back( SQLColumnData( i, (const char*)szColumnName, SQLDataTypeToDatabaseDataType( iDataType ), iColumnSize ) );
	}
}

std::string SQLConnection::DatabaseTypeToString( EDatabaseDataType eType )
{
	std::string strType = "PARAMTYPE_Unknown";

	switch ( eType )
	{
		case PARAMTYPE_Integer:
			strType = "PARAMTYPE_Integer";
			break;
		case PARAMTYPE_String:
			strType = "PARAMTYPE_String";
			break;
		case PARAMTYPE_Float:
			strType = "PARAMTYPE_Float";
			break;
		case PARAMTYPE_Int64:
			strType = "PARAMTYPE_Int64";
			break;
		case PARAMTYPE_Short:
			strType = "PARAMTYPE_Short";
			break;
		case PARAMTYPE_Time:
			strType = "PARAMTYPE_Time";
			break;
		case PARAMTYPE_Binary:
			strType = "PARAMTYPE_Binary";
			break;
		case PARAMTYPE_Null:
			strType = "PARAMTYPE_Null";
			break;

		case PARAMTYPE_Byte:
			strType = "PARAMTYPE_Byte";
			break;
		default:
			break;
	}

	return strType;
}

BOOL SQLConnection::FetchExInternal( const UINT iArrayCount, const UINT iArraySize )
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::FetchEx(%d)] : %s [%s, %s, %s]", iArrayCount, szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE", bFirstFetch ? "TRUE" : "FALSE" );
		return FALSE;
	}

	if ( bFirstFetch )
	{
		bFirstFetch = false;

		if ( !SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)iArrayCount, 0 ) ) )
			return FALSE;

		if ( !SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)iArraySize, 0 ) ) )
			return FALSE;
	}

	if ( !SQL_SUCCEEDED( SQLFetchScroll( hStatement, SQL_FETCH_NEXT, 0 ) ) )
		return FALSE;

	return TRUE;
}


void SQLConnection::Init( EDatabaseID eDatabaseID )
{
	//Copy database name to class
	strcpy_s( szDatabaseName, GetDatabaseData( eDatabaseID )->pszDatabaseName );
	eID = eDatabaseID;
	
	//Try alloc Handle SQL ODBC
	if ( SQL_SUCCESS != SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv ) )
		show_errorSQL( SQL_HANDLE_ENV, hStatement );

	//Set Version ODBC
	if ( SQL_SUCCESS != SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, ( SQLPOINTER )SQL_OV_ODBC3, 0 ) )
		return;

	//Try alloc handle connection
	if ( SQL_SUCCESS != SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hConnection ) )
		show_errorSQL( SQL_HANDLE_DBC, hStatement );

	//Set connection timeout
	SQLSetConnectAttrA( hConnection, SQL_LOGIN_TIMEOUT, ( SQLPOINTER )5, SQL_IS_INTEGER );

	char szStringConnectionSQL[ 512 ];
	SQLCHAR strconn[512] = { 0 };
	ZeroMemory( szStringConnectionSQL, 512 );

	//Set string connection
	
	if ( FileExist( "testserver.ini" ) )
	{
		//STRINGFORMAT( szStringConnectionSQL, "Persist Security Info=False; Integrated Security=False; Driver=%s; Server=%s; Database=%s; Uid=%s; Pwd=%s;", "{ODBC Driver 17 for SQL Server}", "DESKTOP-R0EFKGJ", szDatabaseName, "sa", "pt123@" );
		
		//Test Pancake
		STRINGFORMAT( szStringConnectionSQL, "Persist Security Info=False; Integrated Security=False; Driver=%s; Server=%s; Database=%s; Uid=%s; Pwd=%s;", "{ODBC Driver 17 for SQL Server}", "19267", szDatabaseName, "sa", "pt123@" );

		//Test
		//STRINGFORMAT( szStringConnectionSQL, "Persist Security Info=False; Integrated Security=False; Driver=%s; Server=%s; Database=%s; Uid=%s; Pwd=%s;", "{ODBC Driver 17 for SQL Server}", "WIN-IR6E1U9HDQH", szDatabaseName, "sa", "pt123@" );
		//Host
		//STRINGFORMAT( szStringConnectionSQL, "Persist Security Info=False; Integrated Security=False; Driver=%s; Server=%s; Database=%s; Uid=%s; Pwd=%s;", "{ODBC Driver 17 for SQL Server}", "NS560531", szDatabaseName, "sa", "UeQ8uGz66q" );
	}
	else if ( FileExist( "prog.ini" ) )
	{
		//Test Prog
		STRINGFORMAT( szStringConnectionSQL, "Persist Security Info=False; Integrated Security=False; Driver=%s; Server=%s; Database=%s; Uid=%s; Pwd=%s;", "{ODBC Driver 17 for SQL Server}", "DESKTOP-L82Q6PS", szDatabaseName, "sa", "pt123@" );
	}
	else if ( FileExist( "chaos.ini" ) )
	{
		//Test Prog
		STRINGFORMAT( szStringConnectionSQL, "Persist Security Info=False; Integrated Security=False; Driver=%s; Server=%s; Database=%s; Uid=%s; Pwd=%s;", "{ODBC Driver 17 for SQL Server}", "(local)\\SQLRPT", szDatabaseName, "sa", "dev123" );
	}
	else
	{
		//Host Canada
		STRINGFORMAT( szStringConnectionSQL, "Persist Security Info=False; Integrated Security=False; Driver=%s; Server=%s; Database=%s; Uid=%s; Pwd=%s;", "{ODBC Driver 17 for SQL Server}", "WIN-689E2NV54L9", szDatabaseName, "sa", "N0v1nh4" );
	}

	memcpy( strconn, szStringConnectionSQL, 512 );

	SQLCHAR retconstring[ 1024 ];
	
	//Connect to Database
	SQLDriverConnectA( hConnection, NULL, strconn, lstrlenA((char*)strconn), retconstring, 1024, NULL, SQL_DRIVER_NOPROMPT );

	//Create Statement handler
	if ( SQL_SUCCESS != SQLAllocHandle( SQL_HANDLE_STMT, hConnection, &hStatement ) )
		show_errorSQL( SQL_HANDLE_DBC, hConnection );

	//Create Descriptor
	if ( SQL_SUCCESS != SQLAllocHandle( SQL_HANDLE_DESC, hConnection, &hDescriptor ) )
		show_errorSQL( SQL_HANDLE_DBC, hConnection );

	SQLSetStmtAttr( hStatement, SQL_ATTR_CURSOR_SCROLLABLE, (SQLPOINTER)SQL_SCROLLABLE, SQL_IS_INTEGER );

	//Setup for SQLFetchScroll and SQLMoreResults
	{
		if ( SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_KEYSET_DRIVEN, 0 ) ) == FALSE )
			show_errorSQL( SQL_HANDLE_STMT, hStatement );
		
		if ( SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_USE_BOOKMARKS, (SQLPOINTER)SQL_UB_VARIABLE, 0 ) ) == FALSE )
			show_errorSQL( SQL_HANDLE_STMT, hStatement );

		if ( SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROW_STATUS_PTR, iaRowStatus, 0 ) ) == FALSE )
			show_errorSQL( SQL_HANDLE_STMT, hStatement );

		if ( SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROW_BIND_OFFSET_PTR, &iBindOffset, 0 ) ) == FALSE )
			show_errorSQL( SQL_HANDLE_STMT, hStatement );

		if ( SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROWS_FETCHED_PTR, &iRowsFetched, 0 ) ) == FALSE )
			show_errorSQL( SQL_HANDLE_STMT, hStatement );

		if ( SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_LOCK, 0 ) ) == FALSE )
			show_errorSQL( SQL_HANDLE_STMT, hStatement );

		if ( SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROW_BIND_TYPE, SQL_BIND_BY_COLUMN, 0 ) ) == FALSE )
			show_errorSQL( SQL_HANDLE_STMT, hStatement );
	}

	BlockFree();

	BindLengthFree();

	bFirstFetch = true;

	iSQLParamArraySize = 1;

	vSQLConnection.push_back( this );
}

BOOL SQLConnection::SQLTimeStampToSystemTime( SYSTEMTIME * psSystemTime, SQL_TIMESTAMP_STRUCT * psSQLTimeStamp )
{
	if ( !psSystemTime || !psSQLTimeStamp )
		return FALSE;

	auto GetDayNum = []( int d, int m, int y ) -> int
	{
		static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
		y -= m < 3;
		return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
	};

	psSystemTime->wYear			= psSQLTimeStamp->year;
	psSystemTime->wMonth		= psSQLTimeStamp->month;
	psSystemTime->wDayOfWeek	= GetDayNum( psSQLTimeStamp->day, psSQLTimeStamp->month, psSQLTimeStamp->year );
	psSystemTime->wDay			= psSQLTimeStamp->day;
	psSystemTime->wHour			= psSQLTimeStamp->hour;
	psSystemTime->wMinute		= psSQLTimeStamp->minute;
	psSystemTime->wSecond		= psSQLTimeStamp->second;
	psSystemTime->wMilliseconds = 0;

	return TRUE;
}

void SQLConnection::SQLSystemTimeToTimeStamp( SYSTEMTIME * in, SQL_TIMESTAMP_STRUCT * out )
{
	out->year = in->wYear;
	out->month = in->wMonth;
	out->day = in->wDay;
	out->hour = in->wHour;
	out->minute = in->wMinute;
	out->second = in->wSecond;
	out->fraction = 0;
}

BOOL SQLConnection::Open()
{
	if ( bOpen == FALSE )
		pLastAddress = _ReturnAddress();

	pcMutex->Lock( 1000 + szDatabaseName[0] );

	bOpen = TRUE;
	
	pLastAfterAddress = _ReturnAddress();

	uCurrentBindPosition = 1;

	return TRUE;
}

BOOL SQLConnection::Prepare( const char * pszQuery )
{
	if ( !bOpen )
	{
		WRITEERR( "[SQLConnection::Prepare()] : %s [%s]", pszQuery, bOpen ? "TRUE" : "FALSE" );
		return FALSE;
	}

	szQuery[0] = 0;
	
	//Copy Query
	strcpy_s( szQuery, pszQuery );

	//Check SQLI
	for ( int i = 0; i < _countof( szQuery ); i++ )
	{
		//End string? break it
		if ( szQuery[i] == 0 )
			break;

		//Have quote mark? SQLI, return error
		if ( szQuery[i] == 0x27 )
		{
			//Query Not secure
			WRITEERR("Query not secure!");
			return FALSE;
		}
	}

	//Prepare Query error? return
	if ( SQLPrepareA( hStatement, (SQLCHAR*)szQuery, SQL_NTS ) == SQL_ERROR )
	{
		WRITEERR( "SQLPrepareA() - %s", szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}

	bPrepared = TRUE;

	//Success
	return TRUE;
}

BOOL SQLConnection::Execute()
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::Execute()] : %s [%s,%s]", szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	SQLRETURN iRetCode = SQL_SUCCESS;

	//Try Execute query
	if ( (iRetCode = SQLExecute( hStatement )) == SQL_ERROR )
	{
		WRITEERR( "[SQLConnection::Execute()] %s - %s", szDatabaseName, szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		ShowSQLDescriptorError( hDescriptor );
		return FALSE;
	}

	UpdateColumnsInformaton();

	return TRUE;
}

BOOL SQLConnection::Execute( const char * pszQuery )
{
	if ( !bOpen )
	{
		WRITEERR( "[SQLConnection::Execute()] : %s [%s,%s]", szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	SQLRETURN iRetCode = SQL_SUCCESS;

	//Try Execute query
	if ( (iRetCode = SQLExecDirectA( hStatement, (SQLCHAR*)pszQuery, SQL_NTS )) == SQL_ERROR )
	{
		WRITEERR( "[SQLConnection::Execute()] %s - %s", szDatabaseName, szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		ShowSQLDescriptorError( hDescriptor );
		return FALSE;
	}

	UpdateColumnsInformaton();

	return TRUE;
}

BOOL SQLConnection::UpdateInputArraySize( const UINT iArrayCount, const UINT iArraySize )
{
	if ( iArrayCount > MAX_SQL_BULK_ARRAY_COUNT )
	{
		WRITEERR( "[SQLConnection::UpdateInputArraySize()] : [%d,%d,%d]", iArrayCount, iArraySize, MAX_SQL_BULK_ARRAY_COUNT );
		return FALSE;
	}
	
	iSQLParamArraySize = iArrayCount;
	if ( !SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_PARAMSET_SIZE, (SQLPOINTER)iSQLParamArraySize, 0 ) ) )
	{
		WRITEERR( "[SQLConnection::UpdateInputArraySize()] : [%d,%d,%d]", iArrayCount, iArraySize, MAX_SQL_BULK_ARRAY_COUNT );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}

	if ( !SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_PARAM_BIND_TYPE, (SQLPOINTER)iArraySize, 0 ) ) )
	{
		WRITEERR( "[SQLConnection::UpdateInputArraySize()] : [%d,%d,%d]", iArrayCount, iArraySize, MAX_SQL_BULK_ARRAY_COUNT );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}

	return TRUE;
}

/*
* Note: If bulk update/insert, you must call UpdateInputArraySize() before call this function
* Note: If you want to use bulk update/insert with string, the Size must be set to count of bytes, not count of characters
*
*/
BOOL SQLConnection::BindParameterInput( int iPosition, EDatabaseDataType eDataType, const void * pParameter, int iSize )
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::BindParameterInput()] : %s [%s,%s]", szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	int iType		= 0;
	int iSQLType	= 0;
	int iLen		= 0;

	SQLINTEGER * piInputOutput = BlockNext();
	SQL_TIMESTAMP_STRUCT sTimeStamp;
	int iExtra = 0;
	int icbMax = 0;

	void * pNewParameter = (void*)pParameter;

	//Set Data Type
	switch ( eDataType )
	{
		case PARAMTYPE_Integer:
			iType = SQL_C_LONG;
			iSQLType = SQL_INTEGER;
			*piInputOutput = 0;
			break;
		case PARAMTYPE_String:
			iType = SQL_C_CHAR;
			iSQLType = SQL_VARCHAR;
			iLen = lstrlenA( (char*)pParameter );
			*piInputOutput = SQL_NTS;

			if ( iSize > 0 )
			{
				if ( iSQLParamArraySize > 1 )
				{
					iLen = iSize;
					icbMax = iSize;
				}
			}

			break;
		case PARAMTYPE_Float:
			iType = SQL_C_FLOAT;
			iSQLType = SQL_REAL;
			*piInputOutput = 0;
			break;
		case PARAMTYPE_Short:
			iType = SQL_C_SSHORT;
			iSQLType = SQL_SMALLINT;
			*piInputOutput = 0;
			break;
		case PARAMTYPE_Int64:
			iType = SQL_C_SBIGINT;
			iSQLType = SQL_BIGINT;
			*piInputOutput = 0;
			break;
		case PARAMTYPE_Byte:
			iType = SQL_C_STINYINT;
			iSQLType = SQL_TINYINT;
			*piInputOutput = 0;
			break;
		case PARAMTYPE_Time:
			SQLSystemTimeToTimeStamp( (SYSTEMTIME *)pParameter, &sTimeStamp );

			//Fuckin ugly code
			{
				BOOL bDateTimeMMDDYYYY = FALSE;

				//Get Datetime Format from Windows
				DWORD iBufferSize = 8192;
				char szValue[255];
				RegGetValue( HKEY_CURRENT_USER, "Control Panel\\International", "sShortDate", RRF_RT_ANY, NULL, (PVOID)&szValue, &iBufferSize );

				//DateTime format its mm/dd/yyyy?
				if ( szValue[0] == 'M' || szValue[0] == 'm' )
					bDateTimeMMDDYYYY = TRUE;

				//Format Date into String
				char szDateTime[MAX_PATH] = { 0 };
				STRINGFORMAT( szDateTime, "%02d/%02d/%04d %02d:%02d:%02d", bDateTimeMMDDYYYY ? sTimeStamp.month : sTimeStamp.day, bDateTimeMMDDYYYY ? sTimeStamp.day : sTimeStamp.month, sTimeStamp.year, sTimeStamp.hour, sTimeStamp.minute, sTimeStamp.second );

				std::string strDate( szDateTime );

				*piInputOutput = SQL_NTS;
				iType = SQL_C_CHAR;
				iSQLType = SQL_CHAR;
				iLen = 48;
				iExtra = 0;
				icbMax = strDate.size();
				pNewParameter = (char *)strDate.c_str();
			}
			break;
		case PARAMTYPE_Binary:
			iType = SQL_C_BINARY;
			iSQLType = SQL_BINARY;
			iLen = iSize;
			*piInputOutput = iSize;
			break;
		default:
			break;
	}

	if ( iSQLParamArraySize > 1 )
	{
		piInputOutput = nullptr;
	}
	
	//Bind parameters
	if ( SQLBindParameter( hStatement, iPosition, SQL_PARAM_INPUT, iType, iSQLType, iLen, iExtra, pNewParameter, icbMax, piInputOutput ) == SQL_ERROR )
	{
		WRITEERR( "[SQLConnection::SQLBindParameter(%d, %s)] : %s - %s", iPosition, SQLConnection::DatabaseTypeToString( eDataType ).c_str(), szDatabaseName, szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}
	
	return TRUE;
}

BOOL SQLConnection::GetData( int iPosition, EDatabaseDataType eDataType, void * pParameter, int iSize )
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::GetData(%d, %s)] : %s [%s,%s]", iPosition, SQLConnection::DatabaseTypeToString( eDataType ).c_str(), szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	int iType				= 0;
	SQLINTEGER iDataSize	= 0;
	void * pParam			= pParameter;
	int iSizeParam			= iSize;
	SQL_TIMESTAMP_STRUCT	sTimeStamp;

	//Set Data Type
	switch ( eDataType )
	{
		case PARAMTYPE_Integer:
			iType = SQL_C_LONG;
			break;
		case PARAMTYPE_Short:
			iType = SQL_C_SHORT;
			break;

		case PARAMTYPE_String:
			iType = SQL_C_CHAR;
			break;
		case PARAMTYPE_Float:
			iType = SQL_C_FLOAT;
			break;
		case PARAMTYPE_Int64:
			iType = SQL_C_SBIGINT;
			break;
		case PARAMTYPE_Time:
			iType		= SQL_C_TYPE_TIMESTAMP;
			pParam		= &sTimeStamp;
			iSizeParam	= 23;
			break;
		case PARAMTYPE_Binary:
			iType = SQL_C_BINARY;
			break;
		case PARAMTYPE_Byte:
			iType = SQL_C_STINYINT;
			break;
		default:
			break;
	}

	//Get Data
	if ( SQLGetData( hStatement, iPosition, iType, pParam, iSizeParam, &iDataSize ) == SQL_ERROR )
	{
		WRITEERR( "[SQLConnection::GetData(%d, %s)] : %s - %s", iPosition, SQLConnection::DatabaseTypeToString( eDataType ).c_str(), szDatabaseName, szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}

	if ( eDataType == PARAMTYPE_Time )
		SQLTimeStampToSystemTime( (SYSTEMTIME*)pParameter, &sTimeStamp );
	
	return TRUE;
}

BOOL SQLConnection::GetData( const std::string & strColumnName, EDatabaseDataType eDataType, void * pParameter, int iSize )
{
	return GetData( GetColumnIndex( strColumnName.c_str() ), eDataType, pParameter, iSize );
}

BOOL SQLConnection::BindData( int iPosition, EDatabaseDataType eDataType, void * pParameter, int iSize )
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::BindData(%d, %s)] : %s [%s,%s]", iPosition, SQLConnection::DatabaseTypeToString( eDataType ).c_str(), szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	int iType = 0;
	SQLINTEGER * piDataSize = BindLengthNext();
	void * pParam = pParameter;
	int iSizeParam = iSize;
	SQL_TIMESTAMP_STRUCT	sTimeStamp;

	//Set Data Type
	switch ( eDataType )
	{
		case PARAMTYPE_Integer:
			iType = SQL_C_LONG;
			break;
		case PARAMTYPE_Short:
			iType = SQL_C_SHORT;
			break;

		case PARAMTYPE_String:
			iType = SQL_C_CHAR;
			break;
		case PARAMTYPE_Float:
			iType = SQL_C_FLOAT;
			break;
		case PARAMTYPE_Int64:
			iType = SQL_C_SBIGINT;
			break;
		case PARAMTYPE_Time:
			iType = SQL_C_TYPE_TIMESTAMP;
			pParam = &sTimeStamp;
			iSizeParam = 23;
			break;
		case PARAMTYPE_Binary:
			iType = SQL_C_BINARY;
			break;
		case PARAMTYPE_Byte:
			iType = SQL_C_STINYINT;
			break;
		default:
			break;
	}

	//Get Data
	if ( !SQL_SUCCEEDED( SQLBindCol( hStatement, iPosition, iType, pParam, iSizeParam, piDataSize ) ) )
	{
		WRITEERR( "[SQLConnection::BindData(%d, %s)] : %s - %s", iPosition, SQLConnection::DatabaseTypeToString( eDataType ).c_str(), szDatabaseName, szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}

	if ( eDataType == PARAMTYPE_Time )
		SQLTimeStampToSystemTime( (SYSTEMTIME *)pParameter, &sTimeStamp );

	return TRUE;
}

BOOL SQLConnection::GetColumnName( int iPosition, void * pParameter, int iSize )
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::GetColumnName(%d, %d)] : %s [%s,%s]", iPosition, iSize, szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	SQLSMALLINT cbSize = 0;
	SQLSMALLINT cbDataType = 0;
	SQLUINTEGER cbColumn = 0;
	SQLSMALLINT cbScale = 0;
	SQLSMALLINT uNullType = 0;

	if ( SQLDescribeColA( hStatement, iPosition, (SQLCHAR*)pParameter, iSize, &cbSize, &cbDataType, &cbColumn, &cbScale, &uNullType ) == SQL_ERROR )
	{
		WRITEERR( "error %d - %s - %s", iPosition, szDatabaseName, szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}

	return TRUE;
}

BOOL SQLConnection::GetColumnCount( int & iCount )
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::GetColumnCount()] : %s [%s,%s]", szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	iCount = 0;

	SQLSMALLINT cbCount = 0;

	if ( SQLNumResultCols( hStatement, &cbCount ) == SQL_ERROR )
	{
		WRITEERR( "error %s - %s", szDatabaseName, szQuery );
		show_errorSQL( SQL_HANDLE_STMT, hStatement );
		return FALSE;
	}

	iCount = (int)cbCount;

	return TRUE;
}

EDatabaseDataType SQLConnection::GetColumnType( const char * pszTableName, const char * pszColumnName )
{
	EDatabaseDataType i = PARAMTYPE_Null;

	char szResult[64] = { 0 };

	if ( Prepare( "SELECT DATA_TYPE FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = ? AND COLUMN_NAME = ?" ) )
	{
		BindParameterInput( 1, PARAMTYPE_String, (char*)pszTableName, STRLEN( pszTableName ) );
		BindParameterInput( 2, PARAMTYPE_String, (char*)pszColumnName, STRLEN( pszColumnName ) );

		if ( Execute() && Fetch() )
		{
			GetData( 1, PARAMTYPE_String, szResult, _countof( szResult ) );

			if ( STRINGCOMPAREI( szResult, "varchar" ) )
				i = PARAMTYPE_String;
			else if ( STRINGCOMPAREI( szResult, "int" ) )
				i = PARAMTYPE_Integer;
			else if ( STRINGCOMPAREI( szResult, "real" ) )
				i = PARAMTYPE_Float;
			else if ( STRINGCOMPAREI( szResult, "bigint" ) )
				i = PARAMTYPE_Int64;
		}
	}

	return i;
}

int SQLConnection::GetColumnIndex( const char * pszColumnName )
{
	SQLColumnData sSQLColumnData;
	if ( GetColumnData( pszColumnName, sSQLColumnData ) )
		return sSQLColumnData.iPosition;

	return -1;
}

bool SQLConnection::GetColumnData( const char * pszColumnName, SQLColumnData & sSQLColumnData )
{
	for ( const auto & sColumn : vColumns )
	{
		if ( sColumn.strName.compare( pszColumnName ) == 0 )
		{
			sSQLColumnData = sColumn;
			return true;
		}
	}

	return false;
}

bool SQLConnection::GetColumnData( int iPosition, SQLColumnData & sSQLColumnData )
{
	if ( iPosition <= 0 || iPosition > (int)vColumns.size() )
		return false;

	sSQLColumnData = vColumns[iPosition - 1];

	return true;
}

BOOL SQLConnection::Fetch()
{
	if ( !bOpen || !bPrepared )
	{
		WRITEERR( "[SQLConnection::Fetch()] : %s [%s,%s]", szQuery, bOpen ? "TRUE" : "FALSE", bPrepared ? "TRUE" : "FALSE" );
		return FALSE;
	}

	if ( bFirstFetch )
	{
		bFirstFetch = false;

		if ( !SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)1, 0 ) ) )
			return FALSE;

		if ( !SQL_SUCCEEDED( SQLSetStmtAttr( hStatement, SQL_ATTR_ROW_BIND_TYPE, (SQLPOINTER)0, 0 ) ) )
			return FALSE;
	}

	//Fetch ( Next Row )
	if ( !SQL_SUCCEEDED( SQLFetch( hStatement ) ) )
		return FALSE;

	return TRUE;
}

BOOL SQLConnection::Close()
{
	if ( !bOpen )
		return FALSE;

	Clear();

	bOpen		= FALSE; 

	pcMutex->Unlock();

	return TRUE;
}

BOOL SQLConnection::Clear()
{
	SQLCancel( hStatement );
	SQLFreeStmt( hStatement, SQL_CLOSE );
	SQLFreeStmt( hStatement, SQL_UNBIND );
	SQLFreeStmt( hStatement, SQL_RESET_PARAMS );

	if ( iSQLParamArraySize > 1 )
	{
		UpdateInputArraySize( 1, SQL_PARAM_BIND_BY_COLUMN );
	}

	BlockFree();

	bFirstFetch = true;

	BindLengthFree();

	vColumns.clear();

	uCurrentBindPosition = 1;

	//Clear Query
	//ZeroMemory( szQuery, _countof( szQuery ) );

	bPrepared	= FALSE;

	return TRUE;
}

SQLConnection * SQLConnection::GetConnection( EDatabaseID eDatabaseID )
{
	//Find connection pointer
	for ( const auto v : vSQLConnection )
	{
		//Same DatabaseID? return pointer of class
		if ( v->eID == eDatabaseID )
			return v;
	}
	return NULL;
}