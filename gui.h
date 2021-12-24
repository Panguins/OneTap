#pragma once

#include <vector>
#include <string>
#include <iterator>
#include "renderer.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class c_gui
{
	enum tabs_t
	{
		TABS_LEGIT,
		TABS_RAGE,
		TABS_PLAYERS,
		TABS_VISUALS,
		TABS_MISC,
		TABS_CONFIG
	};

	std::vector<std::string> m_tabs = 
	{
		// maybe just have
		// trigger tab, aimbot tab
		// and aimbot is just rage aimbot
		// with legit bot options
		// like smoothing, randomness etc
		// not sure how to really format this
		xors( "legit" ),
		xors( "rage" ),
		xors( "players" ),
		xors( "visuals" ),
		xors( "misc" ),
		xors( "config" )
	};

	void tab_legit( );
	void tab_rage( );
	void tab_players( );
	void tab_visuals( );
	void tab_misc( );
	void tab_config( );
public:
	void draw( );
}; extern c_gui gui;
