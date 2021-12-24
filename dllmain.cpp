#include "context.h"
#include <VersionHelpers.h>

c_context ctx;

// to get rid of a string in microsoft crt
static void __cdecl std::_Xlength_error( const char *error )
{

}

void hack( HINSTANCE bin )
{
	// setup each interface, hook each function, grab all netvars
	factory::init( &csgo );

	while ( true )
	{
		if ( ctx.m_panic )
		{
			// unhook
			factory::release( &csgo );

			// need to call fclose on win10
			fclose( stdout );
			FreeConsole( );		

			// ...
			FreeLibraryAndExitThread( bin, 0 );

			break;
		}

		// doesn't need to be ran very often
		// but catching CHLClient::Shutdown is important
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}

void fuck_skids( )
{
	// stop most debuggers from working as breakpoint is patched to exit process call

	ulong old_protection = 0;

	uintptr_t process               = (uintptr_t)GetModuleHandleA( nullptr );
	uintptr_t exit_process          = (uintptr_t)GetProcAddress( GetModuleHandleA( xors( "kernel32.dll" ) ), xors( "ExitProcess" ) );
	uintptr_t dbg_ui_remote_breakin = (uintptr_t)GetProcAddress( GetModuleHandleA( xors( "ntdll.dll" ) ), xors( "DbgUiRemoteBreakin" ) );
	uintptr_t dbg_break_point       = (uintptr_t)GetProcAddress( GetModuleHandleA( xors( "ntdll.dll" ) ), xors( "DbgBreakPoint" ) );

	// fuck DbgUiRemoteBreakin
	VirtualProtect( (void*)dbg_ui_remote_breakin, 6, PAGE_EXECUTE_READWRITE, &old_protection );

	*(uint8_t*)( dbg_ui_remote_breakin ) = 0x68; // push
	*(uintptr_t*)( dbg_ui_remote_breakin + 1 ) = exit_process;
	*(uint8_t*)( dbg_ui_remote_breakin + 5 ) = 0xC3; // ret

	VirtualProtect( (void*)dbg_ui_remote_breakin, 6, old_protection, &old_protection );

	// fuck DbgBreakPoint
	VirtualProtect( (void*)dbg_break_point, 6, PAGE_EXECUTE_READWRITE, &old_protection );

	*(uint8_t*)( dbg_break_point ) = 0x68; // push
	*(uintptr_t*)( dbg_break_point + 1 ) = exit_process;
	*(uint8_t*)( dbg_break_point + 5 ) = 0xC3; // ret

	VirtualProtect( (void*)dbg_break_point, 6, old_protection, &old_protection );

	// break tools like scylla and such as they check signatures on dos/nt header

	// fuck dos header signature
	*(uint16_t*)( process ) = 0;

	// fuck nt header signature
	*(uint32_t*)( process + ( (IMAGE_DOS_HEADER*)process )->e_lfanew ) = 0;
}

bool __stdcall DllMain( HINSTANCE instance, ulong32_t reason, void* reserved )
{
	if ( reason == DLL_PROCESS_ATTACH )
	{
		DisableThreadLibraryCalls( instance );

	#ifdef TESTBUILD
		if ( AllocConsole( ) )
		{
			hwnd_t                con_hwndw{ GetConsoleWindow( ) };
			RECT                  con_bound{ 904 + 219, 420 };
			RECT                  wndw_rect{};

			SetConsoleTitle( xors( "csgo" ) );

			GetWindowRect( con_hwndw, &wndw_rect );
			MoveWindow( con_hwndw, wndw_rect.left, wndw_rect.top, con_bound.left, con_bound.top, true );

			SetWindowLong( con_hwndw, GWL_STYLE, GetWindowLong( con_hwndw, GWL_STYLE ) | WS_BORDER );
			SetWindowLong( con_hwndw, GWL_EXSTYLE, GetWindowLong( con_hwndw, GWL_EXSTYLE ) | WS_EX_LAYERED );

			SetLayeredWindowAttributes( con_hwndw, 0, 230, 2 );
			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY );

			freopen( xors( "CONOUT$" ), xors( "w" ), stdout );
		}
	#endif

		CreateThread( 0, 0, LPTHREAD_START_ROUTINE( hack ), instance, 0, 0 );

		return true;
	}

	return false;
}