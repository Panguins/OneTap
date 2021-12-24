#pragma once
#include <string>
#include <vector>
#include <deque>

enum resolver_steps_t
{
	STEP_FORCED_LAST_MOVING,
	STEP_LAST_MOVING,
	STEP_LBY,
	STEP_FREESTAND,
	STEP_UP,
	STEP_NETWORKED,
	STEP_MAXSTEP
};

struct resolver_set_t
{
	std::vector<resolver_steps_t> steps;

	int has_step( resolver_steps_t step )
	{
		for ( size_t i = 0; i < steps.size( ); i++ )
		{
			if ( steps.at( i ) == step )
				return i;
		}

		return -1;
	}

	int increment( int shots ) const
	{
		return ( shots + 1 ) % steps.size( );
	}

	int wrap( int shots ) const
	{
		return ( shots ) % steps.size( );
	}

	resolver_steps_t get_step( int shots )
	{
		shots = wrap( shots );
		return steps.at( shots );
	}
};

static resolver_set_t spread_set =
{
	{
		STEP_FORCED_LAST_MOVING,
		STEP_LBY,
		STEP_FREESTAND,
	}
};

static resolver_set_t nospread_set =
{
	{
		STEP_FORCED_LAST_MOVING,
		STEP_LBY,
		STEP_FREESTAND,
		STEP_UP,
	}
};

static resolver_set_t networked_set
{
	{
		STEP_NETWORKED,
	}
};

static resolver_set_t* get_current_set( )
{
	int autism_level = 0;

	if ( autism_level == 0 )
		return &spread_set;

	if ( autism_level == 88 )
		return &nospread_set;

	return &networked_set;
}