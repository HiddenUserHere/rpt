// stdafx.cpp : source file that includes just the standard includes
// server.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

BOOL bBlockNew = FALSE;



bool NEW_IN_USE = false;

char * ALLOC_SAFE_TO_USE = nullptr;

void AllocNeeded()
{
	if ( ALLOC_SAFE_TO_USE == nullptr )
	{
		//25MB
		ALLOC_SAFE_TO_USE = new char[1024 * 1024 * 25];
	}
}

//Get Module of Address ex: Server.exe+0x123456
std::string GetModuleBaseAddress( DWORD dwAddress )
{
	MODULEENTRY32 me32;
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetCurrentProcessId() );

	if ( hSnapshot == INVALID_HANDLE_VALUE )
	{
		return 0;
	}

	me32.dwSize = sizeof( MODULEENTRY32 );

	if ( Module32First( hSnapshot, &me32 ) )
	{
		do
		{
			if ( dwAddress >= (DWORD)me32.modBaseAddr && dwAddress <= ((DWORD)me32.modBaseAddr + me32.modBaseSize) )
			{
				CloseHandle( hSnapshot );
				
				char szModuleName[MAX_PATH] = {0};
				char szModuleAddrName[128] = {0};
				GetModuleFileNameA( me32.hModule, szModuleName, MAX_PATH );

				//Remove Path
				char * p = strrchr( szModuleName, '\\' );
				if ( p )
				{
					strcpy_s( szModuleName, p + 1 );
				}

				STRINGFORMAT( szModuleAddrName, "%s+0x%X", szModuleName, dwAddress - (DWORD)me32.modBaseAddr );

				return szModuleAddrName;
			}
		} while ( Module32Next( hSnapshot, &me32 ) );
	}

	CloseHandle( hSnapshot );
	return "";
}

void * __cdecl OverrideNew( std::size_t s )
{
	void * pReturnAddress = _ReturnAddress();
	try
	{
		void * p = malloc(s);

		if ( p )
		{
			return p;
		}

		throw std::bad_alloc();
	}
	catch ( std::bad_alloc & )
	{
		if ( NEW_IN_USE == false )
		{
			NEW_IN_USE = true;

			if ( ALLOC_SAFE_TO_USE )
			{
				free(ALLOC_SAFE_TO_USE);
				ALLOC_SAFE_TO_USE = nullptr;
			}

			WRITEERR( "Bad Alloc Size[%d] ReturnAddr[%s]", s, GetModuleBaseAddress( (DWORD)pReturnAddress ).c_str() );

			NEW_IN_USE = false;

			ExitProcess( 0 );
		}
	}

	return nullptr;
}

void __cdecl OverrideDelete( void * p )
{
	if ( p )
	{
		free(p);
	}
}

BYTE baOldAddressNew[5] = {0};
BYTE baOldAddressDelete[5] = {0};

void HookOperatorsNewDelete()
{
	AllocNeeded();

	CopyMemory( baOldAddressNew, (LPVOID)0x005C6F40, 0x5 );
	CopyMemory( baOldAddressDelete, (LPVOID)0x005C7B20, 0x5 );

	DWORD dwOldProtect;
	VirtualProtect( (LPVOID)0x005C6F40, 0x8, PAGE_EXECUTE_READWRITE, &dwOldProtect );

	//JMP 005C6F40 to OverrideNew
	*(BYTE *)0x005C6F40 = 0xE9;
	*(DWORD *)(0x005C6F40 + 0x1) = (DWORD)OverrideNew - (0x005C6F40 + 0x5);

	VirtualProtect( (LPVOID)0x005C6F40, 0x8, dwOldProtect, &dwOldProtect );

	VirtualProtect( (LPVOID)0x005C7B20, 0x8, PAGE_EXECUTE_READWRITE, &dwOldProtect );

	//JMP 005C7B20 to OverrideDelete
	*(BYTE *)0x005C7B20 = 0xE9;
	*(DWORD *)(0x005C7B20 + 0x1) = (DWORD)OverrideDelete - (0x005C7B20 + 0x5);

	VirtualProtect( (LPVOID)0x005C7B20, 0x8, dwOldProtect, &dwOldProtect );
}

void UnhookOperatorsNewDelete()
{
	DWORD dwOldProtect;
	VirtualProtect( (LPVOID)0x005C6F40, 0x8, PAGE_EXECUTE_READWRITE, &dwOldProtect );

	CopyMemory( (LPVOID)0x005C6F40, baOldAddressNew, 0x5 );

	VirtualProtect( (LPVOID)0x005C7B20, 0x8, PAGE_EXECUTE_READWRITE, &dwOldProtect );

	CopyMemory( (LPVOID)0x005C7B20, baOldAddressDelete, 0x5 );
}

void * operator new[]( std::size_t s ) throw(std::bad_alloc)
{
	typedef void * (__cdecl * tfnNewOperator)(std::size_t s);
	tfnNewOperator fnNewOperator = (tfnNewOperator)0x005C6F40;
	return fnNewOperator( s );
}
void operator delete[]( void * p ) throw()
{
	typedef void( __cdecl * tfnDeleteOperator )(void * p);
	tfnDeleteOperator fnDeleteOperator = (tfnDeleteOperator)0x005C7B20;
	fnDeleteOperator( p );
}

void * operator new(std::size_t s) throw(std::bad_alloc)
{
	typedef void * (__cdecl * tfnNewOperator)(std::size_t s);
	tfnNewOperator fnNewOperator = (tfnNewOperator)0x005C6F40;
	return fnNewOperator( s );
}
void operator delete(void * p) throw()
{
	typedef void( __cdecl * tfnDeleteOperator )(void * p);
	tfnDeleteOperator fnDeleteOperator = (tfnDeleteOperator)0x005C7B20;
	fnDeleteOperator( p );
}
void TestAlloc()
{
	while ( true )
	{
		char * p = (char*)OverrideNew(1024 * 1024 * 100);

		Sleep( 1 );
	}
}